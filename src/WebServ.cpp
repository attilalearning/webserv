/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 19:03:57 by aistok            #+#    #+#             */
/*   Updated: 2026/02/24 20:19:38 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "WebServ.hpp"
#include "HTTP.hpp"

/* public section ----------------------------- */

WebServ::WebServ()
{
}

WebServ::~WebServ()
{
	// 1. Servers (Static)
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		delete _servers[i]; // Triggers ~Server() -> close(_listenFd)
	}
	_servers.clear();
	// 2. Connections (Dynamic)
	std::map<int, Connection *>::iterator it;
	for (it = _fdToConnMap.begin(); it != _fdToConnMap.end(); ++it)
	{
		delete it->second; // Triggers ~Connection() -> close(_connectFd)
	}
	_fdToConnMap.clear();
	_pollFds.clear();
	std::cout << "[WebServ] All sockets closed. Cleanup complete." << std::endl;
}

std::vector<Server *> WebServ::getServers() const
{
	return _servers;
}

void WebServ::setup(std::vector<ServerConfig> &configs)
{
	for (size_t i = 0; i < configs.size(); ++i)
	{
		Server *newServer = NULL;
		try
		{
			newServer = new Server(configs[i]);
			newServer->initSocket(); // Creates the socket, bind, listen
			int listenFd = newServer->getListenFd();

			_servers.push_back(newServer);
			_addNewFdtoPool(listenFd, POLLIN);
			_fdToServerMap[listenFd] = newServer;
			std::cout << "[WebServ] New Server setted up on FD: " << listenFd << std::endl; // log

		}
		catch (const std::exception &e)
		{
			std::cerr << "Failed to setup server " << configs[i].host << ":" << configs[i].port  << ": "<< e.what() << std::endl;
			if (newServer)
				delete newServer;
		}
	}
}

/*The Rule:
1 - READ (POLLIN): Always check this first. Call recv().
If recv() returns > 0, we got data.
If recv() returns 0, this is signal that the client closed the connection.

2 - WRITE (POLLOUT): Check this if you have data queued to send.

3 - ERRORS (POLLERR, POLLNVAL, POLLHUP): Use these only as a "fallback" or for fatal system errors.*/

void WebServ::run(void)
{
	while (g_server_running)
	{
		int ret = poll(&_pollFds[0], _pollFds.size(), POLL_TIMEOUT);
		if (ret == 0)
			continue; // Timeout
		// Handle Poll Errors
		if (ret < 0)
		{
			if (errno == EINTR)
				continue;			   // Interrupted by other system calls than Ctrl + C
			if (g_server_running == 0) // Interrupted by Ctrl + C
				break;
			throw std::runtime_error(std::string("Poll failed: ") + std::strerror(errno));
		}
		// Loop through FDs
		for (size_t i = 0; i < _pollFds.size(); ++i)
		{
			if (_pollFds[i].revents == 0)
				continue;

			// 1. HANDLE READS (Highest Priority)
			// We check this FIRST. If a client sends data and closes immediately,
			// we read the data before noticing the HUP.
			if (_pollFds[i].revents & POLLIN)
			{
				if (_isListener(_pollFds[i].fd))
					this->_acceptNewConnection(_pollFds[i].fd);
				else if (this->_readRequest(i) == false) // if close connection
					i--; // <--- THIS is what makes the Swap & Pop safe!
			}

			// 2. HANDLE WRITES
			// Use 'else if' to prevent operating on a closed FD if step 1 closed it.
			// Only check POLLOUT if you know you have data to send.
			else if (_pollFds[i].revents & POLLOUT)
			{
				// this->_sendResponse(i);

				// JUST FOR TEST:
				// std::string msg = "Hello World!\n";
//				std::string msg = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello World!\n";
				HTTP::Response hResp(HTTP::Status::NOT_FOUND, "404 - Sorry, the page was not found... :(\n\r");
				std::string data_to_send = hResp.toString();
				std::cout << "Sending below response of " << data_to_send.size() << " bytes" << std::endl;
				std::cout << ESC_YELLOW_HOLLOW;
				std::cout << "----------------------------------------------------\n";
				std::cout << hResp;
//				std::cout << msg;
				std::cout << "----------------------------------------------------";
				std::cout << ESC_END << "\n\n";
				send(_pollFds[i].fd, data_to_send.c_str(), data_to_send.size(), 0);
//				send(_pollFds[i].fd, msg.c_str(), msg.size(), 0);
				_pollFds[i].events &= ~POLLOUT;
			}

			// 3. HANDLE ERRORS (Lowest Priority)
			// This is a fallback. Standard closures are usually handled by
			// recv() returning 0 in step 1. This catches abnormal errors.
			else if (_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				_closeConnection(i);
				// If your _closeConnection removes the element from the vector
				// by swapping with the last element, you must decrement i
				// to re-check the swapped element in the next iteration.
				i--;
			}
		}
	}
}

/* private section ---------------------------- */
void WebServ::_addNewFdtoPool(int newFd, short events) // short - data type for bitmask of events
{
	if (newFd < 0)
		return;

	pollfd pfd;
	pfd.fd = newFd;
	pfd.events = events;
	pfd.revents = 0;
	_pollFds.push_back(pfd);

	std::cout << "[Poll] Added FD " << newFd << " to monitoring pool." << std::endl; // log
}

bool WebServ::_isListener(int fd)
{
	return _fdToServerMap.find(fd) != _fdToServerMap.end();
}

void WebServ::_acceptNewConnection(int listenFd)
{
	sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	// 1. ACCEPT CONNECTION
	int connFd = accept(listenFd, (sockaddr *)&clientAddr, &clientLen);
	if (connFd < 0)
	{	
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
			 return;
		std::cerr << "Accept failed on fd " << listenFd << std::endl;
		return;
	}
	// 2. SET NON-BLOCKING
	if (setNonBlocking(connFd)  == false)
	{
		close(connFd);
		connFd = -1;
		return;
	}
	// 3. CREAT CONNECTION OBJECT
	Connection *newConn = NULL;
	try
	{
		std::map<int, Server *>::iterator it = _fdToServerMap.find(listenFd);
		if (it == _fdToServerMap.end())
		{
			std::cerr << "[WebServ] Critical Error: Listener FD " << listenFd << " not associated with any Server." << std::endl;
    		close(connFd);
    		return;
		}
		Server *server = it->second;
		newConn = new Connection(connFd, clientAddr, server); // can throw exception
		_addNewFdtoPool(connFd, POLLIN);
		_fdToConnMap[connFd] = newConn;
		std::cout << "[WebServ] New connection accepted on FD: " << connFd << " (Listener FD " << listenFd << ")" << std::endl; // log
	}
	catch (const std::exception &e)
	{
		std::cerr << "[WebServ] Failed to create connection: " << e.what() << std::endl;
		close(connFd);
	}
}

void WebServ::_closeConnection(size_t index)
{
	int fd = _pollFds[index].fd;

	// 1. CLEAN UP CONNECTION
	if (_fdToConnMap.count(fd))
	{
		delete _fdToConnMap[fd]; //trigger closing fd
		_fdToConnMap.erase(fd);
	}
	// 2. REMOVE FROM FD POOL (USING SWAP/POP, O(1) efficiency)
	_pollFds[index] = _pollFds.back();
	_pollFds.pop_back();

	std::cout << "Closed connection on FD: " << fd << std::endl;
}

bool WebServ::_readRequest(size_t index)
{
	char buffer[BUFFER_SIZE] = {0};
	int fd = _pollFds[index].fd;
	// int bytesRead = recv(fd, buffer, sizeof(buffer), 0);

	std::map<int, Connection *>::iterator it = _fdToConnMap.find(fd);
	if (it == _fdToConnMap.end())
	{
		std::cerr << "[WebServ] Critical: No connection object for FD " << fd << std::endl;
		_closeConnection(index);
		return false;
	}
	Connection *conn = it->second;
	(void)conn;

	int bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0); // JUST FOR TEST

	// CASE A: Data Received
	if (bytesRead > 0)
	{
		// JUST FOR TESTS:
		buffer[bytesRead] = '\0';
		std::cout << "Received " << bytesRead << " bytes from FD " << fd << std::endl;
		HTTP::Request hRequest(buffer, bytesRead);
		std::cout << ESC_VIOLET_HOLLOW;
		std::cout << "----------------------------------------------------\n";
		std::cout << hRequest;
		std::cout << "----------------------------------------------------";
		std::cout << ESC_END << "\n\n";
		
		// conn->_appendRequest(buffer, bytesRead); 
		// Only flip to POLLOUT after full parcing, valid request
		//if (conn->_isRequestComplete())
		// For now, we leave it for testing:
		_pollFds[index].events |= POLLOUT; 
		return true;
	}
	// CASE B: Soft Error (Try again later)
	else if (bytesRead == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) //EINTR - signals case
		return true;
	// CASE C: Client Closed Connection (FIN) OR Error (-1)
	else
	{
		if (bytesRead == 0)
			std::cout << "[WebServ] Client closed connection on FD: " << fd << std::endl;
			
		else
			std::cerr << "[WebServ] Fatal recv error on FD: " << fd << ". Closing." << std::endl;
		_closeConnection(index);
		return false;
	}
}