/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 19:03:57 by aistok            #+#    #+#             */
/*   Updated: 2026/02/06 15:27:54 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "../inc/WebServ.hpp"

/* public section ----------------------------- */

WebServ::WebServ()
{
	std::cout << "This is gooooing to be the " << _name << "!" << std::endl;
}

WebServ::~WebServ()
{
	for (size_t i = 0; i < _pollFds.size(); ++i)
	{
		if (_pollFds[i].fd >= 0)
		{
			close(_pollFds[i].fd);
		}
	}
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		delete _servers[i]; // This triggers the Server destructor and closes the FD
	}
	// Clear any dynamically allocated client objects
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
			_servers.push_back(newServer);

			// Add the listening socket to our master poll vector
			struct pollfd pollFd;
			// pollFd.fd = newServer->getListenFd();
			pollFd.events = POLLIN;
			pollFd.revents = 0;

			_pollFds.push_back(pollFd);
			_fdToServerMap[_pollFds.back().fd] = newServer;
		}
		catch (const std::exception &e)
		{
			delete newServer;
			std::cerr << "Failed to setup server: " << e.what() << std::endl;
		}
	}
}

void WebServ::run(void)
{
	// to be added later;
}

/* protected section -------------------------- */

//...

/* private section ---------------------------- */

const std::string WebServ::_name = "WebServ";

/*The Rule: If revents != 0, something happened. You must check for:
1 - Errors (POLLERR, POLLHUP, POLLNVAL) first.
2- Reads (POLLIN) second.
3 - Writes (POLLOUT) third.*/

void WebServ::run(void)
{
	while (g_server_running)
	{
		int ret = poll(_pollFds.data(), _pollFds.size(), 1000); // 1000ms timeout
		if (ret < 0)
		{ 
			// Only throw if it's NOT a signal interruption.
			if (errno == EINTR)
				continue;
			if (g_server_running == 0) break;
			
			throw std::runtime_error(std::string("Poll failed: ") + std::strerror(errno));
		}
		if (ret == 0) continue; // Timeout
		for (size_t i = 0; i < _pollFds.size(); ++i)
		{
			if (_pollFds[i].revents == 0)
				continue;

			// Handle Errors (Disconnects)
			if (_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				// TO-DO: close(_pollFds[i].fd); 
				// TO-DO: Remove from _pollFds vector (careful with 'i' decrement!)				continue;
				continue;
			}
			// Handle Incoming Data (Ready to Read)
			if (_pollFds[i].revents & POLLIN)
			{
				if (_isListener(_pollFds[i].fd))
				{
					this->_acceptNewConnection(_pollFds[i].fd);

				// else
				// {
				// 	this->_readRequest(i, _pollFds);
				// }
			}

			// // Handle Outgoing Data (Ready to Write)
			// if (_pollFds[i].revents & POLLOUT)
			// {
			// 	this->_sendResponse(i, _pollFds);
			// }
			}
		}
	}
}

bool WebServ::_isListener(int fd)
{
	return _fdToServerMap.find(fd) != _fdToServerMap.end();
}

void WebServ::_acceptNewConnection(int listenFd)
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);

	int clientFd = accept(listenFd, (struct sockaddr *)&clientAddr, &clientLen);
	if (clientFd < 0)
		return; // Handle error (e.g., EAGAIN)


		
	// Make it non-blocking
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0) {
			close(clientFd);
			return;
		}
	// Add the client socket to our master poll vector
	struct pollfd pollFd;
	pollFd.fd = clientFd;
	pollFd.events = POLLIN | POLLOUT; //??
	pollFd.revents = 0;
	_pollFds.push_back(pollFd);

	//JUST FOR TESTS:
	std::cout << "Connection accepted! FD: " << clientFd << std::endl;
	std::string msg = "Hello World! TEST MESSAGE\n";
	send(clientFd, msg.c_str(), msg.length(), 0);
	
	// // Create a Client object to track this specific connection
	// _clients[clientFd] = Client(clientFd, clientAddr);
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
