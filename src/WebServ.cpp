/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 19:03:57 by aistok            #+#    #+#             */
/*   Updated: 2026/03/20 21:56:54 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "WebServ.hpp"
#include "ErrorPages.hpp"

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
			std::cerr << "Failed to setup server " << configs[i].host << ":" << configs[i].port << ": " << e.what() << std::endl;
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
		_checkConnTimeouts();
		int ret = poll(&_pollFds[0], _pollFds.size(), POLL_TIMEOUT);
		if (ret == 0)
			continue; // Poll Timeout
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
			if (_pollFds[i].revents & POLLIN)
			{
				if (_isListener(_pollFds[i].fd))
					this->_acceptNewConnection(_pollFds[i].fd);
				else
					_readRequest(&i);
			}

			// 2. HANDLE WRITES
			else if (_pollFds[i].revents & POLLOUT)
			{
				_sendResponse(&i);
			}
			// 3. HANDLE ERRORS (Lowest Priority)
			// This is a fallback. Standard closures are usually handled by
			// recv() returning 0 in step 1. This catches abnormal errors.
			else if (_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				_closeConnection(i);
				i--;
			}
		}
	}
}


void WebServ::_addNewFdtoPool(int newFd, short events)
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
	if (setNonBlocking(connFd) == false)
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
		delete _fdToConnMap[fd]; // trigger closing fd
		_fdToConnMap.erase(fd);
	}
	// 2. REMOVE FROM FD POOL (USING SWAP/POP, O(1) efficiency)
	_pollFds[index] = _pollFds.back();
	_pollFds.pop_back();

	std::cout << "Closed connection on FD: " << fd << std::endl;
}


void WebServ::_readRequest(size_t *index)
{
	char tempBuffer[BUFFER_SIZE] = {0};
	int fd = _pollFds[*index].fd;
	int bytesRead = recv(fd, tempBuffer, sizeof(tempBuffer), 0);
	if (bytesRead > 0)
	{
		std::map<int, Connection *>::iterator it = _fdToConnMap.find(fd);
		if (it == _fdToConnMap.end())
		{
			std::cerr << "[WebServ] Critical: No connection object for FD " << fd << std::endl;
			_closeConnection(*index);
			(*index)--;
		}
		Connection *conn = it->second;
		conn->resetTimeout();
		conn->handleRead(tempBuffer, bytesRead);

		if (conn->getState() == Connection::REQUEST_READY)
		{
			std::cout << "[WebServ] Request complete on FD " << fd << ". Flipping to POLLOUT." << std::endl;
			
			conn->prepareResponse();
			_updateEvent(*index, POLLOUT, POLLIN);
		}
		else if (conn->getState() == Connection::ERROR)
		{
			conn->prepareResponse();
			_updateEvent(*index, POLLOUT, POLLIN);
		}
		return ;
	}
	// CASE B: Soft Error (Try again later + EINTR - signals case)
	else if (bytesRead == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
		return ;
	// CASE C: Client Closed Connection (FIN) OR Error (-1)
	else
	{
		if (bytesRead == 0)
			std::cout << "[WebServ] Client closed connection on FD: " << fd << std::endl;

		else
			std::cerr << "[WebServ] Fatal recv error on FD: " << fd << ". Closing." << std::endl;
		_closeConnection(*index);
		(*index)--;
		return ;
	}
}

void WebServ::_sendResponse(size_t *index)
{
	int fd = _pollFds[*index].fd;
	std::map<int, Connection *>::iterator it = _fdToConnMap.find(fd);
	if (it == _fdToConnMap.end())
	{
		std::cerr << "[WebServ] Critical: No connection object for FD " << fd << std::endl;
		_closeConnection(*index);
		(*index)--;
		return ;
	}
	Connection *conn = it->second;
	conn->resetTimeout();
	bool finished = conn->handleWrite();
	if (finished)
	{
		// Decide whether to close or keep open (Keep-Alive)
		if (conn->shouldClose()) {
			std::cout << "[WebServ] Closing connection on FD " << fd << std::endl;
			_closeConnection(*index);
			(*index)--;
			return ;
		}
		else //keep open (Keep-Alive)
		{
			std::cout << "[WebServ] Keeping connection alive on FD " << fd << std::endl;
			conn->resetForNextRequest();
			_updateEvent(*index, POLLIN, POLLOUT);
			return ;
		}
	}
}


void WebServ::_checkConnTimeouts()
{
	time_t now = std::time(NULL);
	std::map<int, Connection*>::iterator it = _fdToConnMap.begin();

	while (it != _fdToConnMap.end())
	{
		int fd = it->first;
		Connection *conn = it->second;
		std::map<int, Connection*>::iterator next = it;
		++next;

		if (conn->isTimedOut(now, CONNECTION_TIMEOUT))
		{
			// 1. Find the index in _pollFds once
			int pollIdx = -1;
			for (size_t i = 0; i < _pollFds.size(); ++i) {
				if (_pollFds[i].fd == fd) {
					pollIdx = i;
					break;
				}
			}

			if (pollIdx == -1) { 
				it = next; // Should never happen, but safety first
				continue; 
			}

			// 2. Decide: Polite Goodbye (408) or Silent Goodbye (Close)
			if (conn->getState() == Connection::READING_HEADERS || 
				conn->getState() == Connection::READING_BODY)
			{
				std::cout << "[WebServ] 408 Request Timeout on FD: " << fd << std::endl;    
				conn->forceTimeoutError();
				conn->prepareResponse();
				
				// Flip to write mode to send the 408
				_updateEvent(pollIdx, POLLOUT, POLLIN);
				
			}
			//TO-DO: if the state is WAITING_FOR_CGI - send a 504 Gateway Timeout
			else 
			{
				std::cout << "[WebServ] Connection idle timeout (closing) on FD: " << fd << std::endl;
				_closeConnection(pollIdx);
			}
		}
		it = next;
	} 
}

void WebServ::_updateEvent(size_t index, short enable, short disable)
{
	if (index >= _pollFds.size())
		return;

	// Turn ON the bits you want
	_pollFds[index].events |= enable;

	// Turn OFF the bits you don't want
	_pollFds[index].events &= ~disable;
}
