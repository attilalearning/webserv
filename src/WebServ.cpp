/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 19:03:57 by aistok            #+#    #+#             */
/*   Updated: 2026/02/11 01:27:17 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "WebServ.hpp"

/* public section ----------------------------- */

WebServ::WebServ()
{}

WebServ::~WebServ()
{
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		delete _servers[i]; // This triggers the Server destructor and closes the FD
	}
	_servers.clear();
	_pollFds.clear();
	// TODO - Clear any dynamically allocated client objects
	std::cout << "All sockets closed. Cleanup complete." << std::endl;
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
			newServer->setupServer(); // Creates the socket, bind, listen
			int listenFd = newServer->getListenFd();

			_servers.push_back(newServer);
			_addNewFdtoPool(listenFd, POLLIN);
			_fdToServerMap[listenFd] = newServer;
		}
		catch (const std::exception &e)
		{
			delete newServer;
			std::cerr << "Failed to setup server: " << e.what() << std::endl;
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
		int ret = poll(&_pollFds[0], _pollFds.size(), 1000); // 1000ms timeout
		if (ret == 0) continue; // Timeout
		// Handle Poll Errors
		if (ret < 0)
		{
			if (errno == EINTR) continue; // System call interrupted
			if (g_server_running == 0) break;
			throw std::runtime_error(std::string("Poll failed: ") + std::strerror(errno));
		}
		// Loop through FDs
		for (size_t i = 0; i < _pollFds.size(); ++i)
		{
			if (_pollFds[i].revents == 0) continue;

			// 1. HANDLE READS (Highest Priority)
			// We check this FIRST. If a client sends data and closes immediately, 
			// we read the data before noticing the HUP.
			if (_pollFds[i].revents & POLLIN)
			{
				if (_isListener(_pollFds[i].fd))
					this->_acceptNewConnection(_pollFds[i].fd);
				else
					if (this->_readRequest(i) == false) //if close connection
						i--;
			}
			
			// 2. HANDLE WRITES 
			// Use 'else if' to prevent operating on a closed FD if step 1 closed it.
			// Only check POLLOUT if you know you have data to send.
			else if (_pollFds[i].revents & POLLOUT)
			{
				// this->_sendResponse(i);

				//JUST FOR TEST:
				std::string msg = "Hello World!\n";
				send(_pollFds[i].fd, msg.c_str(), msg.length(), 0);
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
	if (newFd < 0) return;

	struct pollfd pfd;
	pfd.fd = newFd;
	pfd.events = events;
	pfd.revents = 0;
	_pollFds.push_back(pfd);

	std::cout << "[Poll] Added FD " << newFd << " to monitoring pool." << std::endl; //log
}

bool WebServ::_isListener(int fd)
{
	return _fdToServerMap.find(fd) != _fdToServerMap.end();
}

void WebServ::_acceptNewConnection(int listenFd)
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	//1. ACCEPT CONNECTION
	int connFd = accept(listenFd, (struct sockaddr *)&clientAddr, &clientLen);
	if (connFd < 0)
	{
		std::cerr << "Accept failed on fd " << listenFd << std::endl;
		return;
	}
	//2. SET NON-BLOCKING
	if (fcntl(connFd, F_SETFL, O_NONBLOCK) < 0)
	{
			close(connFd);
			return;
	}
	//3. CREAT CONNECTION OBJECT
	Connection* newConn = NULL;
	try
	{
		Server* server = _fdToServerMap[listenFd];
		newConn = new Connection(connFd, clientAddr, server); //can throw exception
		_addNewFdtoPool(connFd, POLLIN);
		_fdToConnMap[connFd] = newConn;
		std::cout << "[WebServ] New connection accepted on FD: " << connFd << std::endl; // log	
	} 
	catch (const std::exception &e)
	{
		std::cerr << "Failed to create connection: " << e.what() << std::endl;
		if (newConn) delete newConn;
		close(connFd);
		std::cerr << "Memory allocation failed for new connection" << std::endl;
	}
}

void WebServ::_closeConnection(size_t index)
{
	int fd = _pollFds[index].fd;

	//1. CLEAN UP CONNECTION
	if (_fdToConnMap.count(fd))
	{
		delete _fdToConnMap[fd];
		_fdToConnMap.erase(fd);
	}
	//2. CLOSE FD
	if (fd != -1)
		close(fd);
	//3. REMOVE FROM FD POOL (USING SWAP/POP, O(1) efficiency)
	_pollFds[index] = _pollFds.back();
	_pollFds.pop_back();

	std::cout << "Closed connection on FD: " << fd << std::endl;
}

bool WebServ::_readRequest(size_t index)
{
	char buffer[1024] = {0}; // CHANGE TO BUFFER_SIZE??
	int fd = _pollFds[index].fd;
	// int bytesRead = recv(fd, buffer, sizeof(buffer), 0);
	int bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0); // JUST FOR TEST

	
	// CASE A: Data Received
	if (bytesRead > 0)
	{
		// Locate the connection object
		if (_fdToConnMap.count(fd))
		{
			// // Append data to your Connection object's internal buffer
			// _fdToConnMap[fd]->_appendRequest(buffer, bytesRead);
			// // Optional: Check if request is complete immediately
			// if (_fdToConnMap[fd]->_isRequestComplete())
				 _pollFds[index].events |= POLLOUT; // Enable writing

			//JUST FOR TESTS:
			buffer[bytesRead] = '\0'; // Explicitly null-terminate
			std::cout << "Received from client: " << buffer << std::endl;
		}
		return true;
	}
	// CASE B: Client Closed Connection (FIN) OR Error (-1)
	else 
	{
		if (bytesRead == 0)
			std::cout << "[WebServ] Client closed connection on FD: " << fd << std::endl;
		else
			std::cout << "[WebServ] Recv returned -1 on FD: " << fd << ". Closing." << std::endl;;

		_closeConnection(index);
		return false;
	}
}