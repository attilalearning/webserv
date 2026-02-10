/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 14:48:56 by mosokina          #+#    #+#             */
/*   Updated: 2026/02/10 00:14:01 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const ServerConfig &config) : _config(config), _listenFd(-1)
{
	_address = sockaddr_in();
}

Server::~Server()
{
	if (_listenFd != -1)
	{
		std::cout << "Closing fd " << _listenFd << std::endl;
		close(_listenFd);
		_listenFd = -1;
	}
}

int Server::getListenFd() const
{
	return _listenFd;
}

void Server::setupServer()
{
	// 1. Create the socket (IPv4, TCP)
	_listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenFd == -1)
	{
		throw std::runtime_error("Failed to open listen fd");
	}
	// 2. Set SO_REUSEADDR (Allows immediate restart of the server)
	int opt = 1;
	if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		throw std::runtime_error("Failed to set setsockopt");
	}
	// 3. Set to Non-Blocking (Crucial for WebServ requirements)
	if (fcntl(_listenFd, F_SETFL, O_NONBLOCK) == -1)
	{
		throw std::runtime_error("Failed to set non-blocking mode");
	}
	// 4. Prepare address structure
	_address = _getSocketAddress(_config.host, _config.port);
	// 5. Bind socket to the address/port
	if (bind(_listenFd, (struct sockaddr *)&_address, sizeof(_address)) == -1)
	{
		// throw std::runtime_error("Failed to bind to port " + toString(_config.port));
		throw std::runtime_error("Failed to bind to port " + toString(_config.port) + ": " + std::strerror(errno));
	}
	// 6. Start listening for incoming connections
	if (listen(_listenFd, 128) == -1)
	{
		throw std::runtime_error("Failed to listen on socket");
	}
	std::cout << "[Server] Listening on " << _config.host << ":" << _config.port << std::endl;
}

// Helper function to resolve host/port to sockaddr_in
struct sockaddr_in Server::_getSocketAddress(const std::string &host, int port)
{
	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	struct addrinfo *result;
	hints.ai_family = AF_INET;		 // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_flags = AI_PASSIVE;

	std::string portStr = toString(port);

	const char* hostPtr = (host.empty() || host == "0.0.0.0") ? NULL : host.c_str();
	int status = getaddrinfo(hostPtr, portStr.c_str(), &hints, &result);
	if (status != 0)
	{
		throw std::runtime_error("DNS Error: " + std::string(gai_strerror(status)));
	}
	if (!result)
	{
		throw std::runtime_error("DNS Error: No address found for " + host);
	}
	struct sockaddr_in addr = *(struct sockaddr_in *)result->ai_addr;
	freeaddrinfo(result);
	return addr;
}
