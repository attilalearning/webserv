/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 19:03:57 by aistok            #+#    #+#             */
/*   Updated: 2026/02/10 00:55:33 by mosokina         ###   ########.fr       */
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

/*The Rule: If revents != 0, something happened. You must check for:
1 - Errors (POLLERR, POLLHUP, POLLNVAL) first.
2- Reads (POLLIN) second.
3 - Writes (POLLOUT) third.*/

void WebServ::run(void)
{
	while (g_server_running)
	{
		int ret = poll(_pollFds.data(), _pollFds.size(), 1000); // 1000ms timeout
		if (ret == 0) continue; // Timeout
		else if (ret < 0)
		{ 
			if (errno == EINTR) continue;
			if (g_server_running == 0) break;
			throw std::runtime_error(std::string("Poll failed: ") + std::strerror(errno));
		}
		else
		{
			for (size_t i = 0; i < _pollFds.size(); ++i)
			{
				if (_pollFds[i].revents == 0) continue;
				int currectFd = _pollFds[i].fd;

            	// 1. HANDLE ERRORS OR DISCONNECTS
				if (_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
				{
					_closeConnection(i);
                	--i; // Check the new element shifted into this index
				}
				// 2. HANDLE READS
				else if (_pollFds[i].revents & POLLIN)
				{
					if (_isListener(_pollFds[i].fd))
						this->_acceptNewConnection(_pollFds[i].fd);
					// else
					// 	this->_readRequest(i, _pollFds);
				}
				// 3. HANDLE WRITES
				// else if (_pollFds[i].revents & POLLOUT)
				// {
				// 	this->_sendResponse(i, _pollFds);
				// }
			}
		}

	}
}

/* private section ---------------------------- */
void WebServ::_addNewFdtoPool(int newFd, short events) // short for data type used for the bitmask of events
{
	// Sanity check: Ensure the FD is valid
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

	int connFd = accept(listenFd, (struct sockaddr *)&clientAddr, &clientLen);
	if (connFd < 0)
	{
		std::cerr << "Accept failed on fd " << listenFd << std::endl;
		return;
	}
	if (fcntl(connFd, F_SETFL, O_NONBLOCK) < 0) // non-blocking
	{
			close(connFd);
			return;
	}
	_addNewFdtoPool(connFd, POLLIN);
	try
	{
		_fdToConnMap[connFd] = new Connection(connFd, clientAddr); //can throw exception
		std::cout << "[WebServ] New connection accepted on FD: " << connFd << std::endl; // log
	} 
	catch (const std::exception &e)
	{
		close(connFd);
		_pollFds.pop_back();
		std::cerr << "Memory allocation failed for new connection" << std::endl;
	}

	// //JUST FOR TESTS:
	// std::cout << "Connection accepted! FD: " << connFd << std::endl;
	// std::string msg = "Hello World! TEST MESSAGE\n";
	// send(connFd, msg.c_str(), msg.length(), 0);
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

// // void WebServ::_readRequest(size_t indexClientFd)
// // {
// // 	char buffer[1024] = {0};										// BUFFER_SIZE??
// // 	int bytesRead = recv(_pollFds[indexClientFd], buffer, 1023, 0); // 1023 to save space for null ???

// // 	if (bytesRead <= 0)
// // 	{
// // 		close(_pollFds[indexClientFd]);
// // 		return;
// // 	}

// // 	buffer[bytesRead] = '\0'; // Null-terminate for safe printing
// // 	// std::cout << "TEST::Received from client: " << buffer << std::endl;

// // 	// add client request to the client object
// // 	// check is it full
// // 	if (REQUEST COMPLITED)
// // 		_pollFds[indexClientFd].events = POLLOUT;
// // }

// // void Cluster::readRequest(size_t index, std::vector<pollfd> &fds)
// // {
// // 	char buffer[BUFFER_SIZE];
// // 	int client_fd = fds[index].fd;

// // 	int bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);

// // 	if (bytes_read <= 0)
// // 	{
// // 		// bytes_read == 0: Client closed connection
// // 		// bytes_read < 0: Error
// // 		this->closeConnection(index, fds);
// // 		return;
// // 	}

// // 	// Append data to this specific client's request buffer
// // 	_clients[client_fd].request.append(buffer, bytes_read);

// // 	// Check if we have the full header
// // 	if (_clients[client_fd].request.isComplete())
// // 	{
// // 		// We have the whole request!
// // 		// Now we stop listening for data and wait for the socket to be ready for sending
// // 		fds[index].events = POLLOUT;
// // 	}
// // }
