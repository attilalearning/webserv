/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 14:48:56 by mosokina          #+#    #+#             */
/*   Updated: 2026/01/27 00:42:17 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

Server::Server(const ServerConfig config) : _config(config), _listenFd(-1){
	std::memset(&_address, 0, sizeof(_address));
}

Server::~Server() {
	if (_listenFd != -1) {
		std::cout << "Closing fd " << _listenFd << std::endl;
		close(_listenFd);
		_listenFd = -1; 
	}
}

int Server::getListenFd() const{
	return _listenFd;
}

void Server::setupServer() {
	// 1. Create the socket (IPv4, TCP)
	_listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenFd == -1) {
		throw std::runtime_error("Failed to create socket");
	}

	// 2. Set SO_REUSEADDR (Allows immediate restart of the server)
	int opt = 1;
	if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		throw std::runtime_error("Failed to set setsockopt");
	}

	// 3. Set to Non-Blocking (Crucial for WebServ requirements)
	if (fcntl(_listenFd, F_SETFL, O_NONBLOCK) == -1) {
		throw std::runtime_error("Failed to set non-blocking mode");
	}

	// 4. Prepare address structure
	_address.sin_family = AF_INET;
	_address.sin_port = htons(_config.port); // Convert port to network byte order
	_address.sin_addr.s_addr = inet_addr(_config.host.c_str()); // Convert IP string

	// 5. Bind socket to the address/port
	if (bind(_listenFd, (struct sockaddr *)&_address, sizeof(_address)) == -1) {
		throw std::runtime_error("Failed to bind to port " + toString(_config.port));
	}

	// 6. Start listening for incoming connections
	if (listen(_listenFd, 128) == -1) {
		throw std::runtime_error("Failed to listen on socket");
	}

	std::cout << "[Server] Listening on " << _config.host << ":" << _config.port << std::endl;
}
