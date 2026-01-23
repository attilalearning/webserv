/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 14:48:56 by mosokina          #+#    #+#             */
/*   Updated: 2026/01/23 15:24:12 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
Server::Server(const ServerConfig config) : _config(config), _listenFd(-1){
	std::memset(&_address, 0, sizeof(_address));
}

Server::Server(const Server& other) : _config(other._config){
	// Crucial: The new copy starts with no active socket
	_listenFd = -1; 
	std::memset(&_address, 0, sizeof(_address));
}

Server& Server::operator=(const Server& other) {
	if (this != &other) {
		if (_listenFd != -1) {
			close(_listenFd);
		}
		_config = other._config;
		_listenFd = -1; //we don't copy fd
		std::memset(&_address, 0, sizeof(_address));
	}
	return *this;
}

Server::~Server() {
	if (_listenFd != -1) {
		std::cout << "Closing fd " << _listenFd << std::endl;
		close(_listenFd);
		_listenFd = -1; 
	}
}

void Server::setupServer(){
	//_config.port to call bind()
	//_config.host to call socket setup
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
		throw std::runtime_error("Failed to bind to port " + _config.port);
	}

	// 6. Start listening for incoming connections
	if (listen(_listenFd, 128) == -1) {
		throw std::runtime_error("Failed to listen on socket");
	}

	std::cout << "[Server] Listening on " << _config.host << ":" << _config.port << std::endl;
}