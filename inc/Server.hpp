/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 14:46:16 by mosokina          #+#    #+#             */
/*   Updated: 2026/02/13 15:24:37 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

// TO-DO update based on fun used in the code!!

#include <arpa/inet.h>	// htons
#include <fcntl.h>		// fcntl, O_NONBLOCK
#include <netdb.h>		// getaddrinfo, freeaddrinfo
#include <sys/socket.h> // socket, bind, listen
#include <sys/types.h>	// types for sockets
#include <unistd.h>		// close
#include <iostream>
#include <stdexcept>
#include <cerrno>
#include <cstring>

#include "ConfigStructs.hpp"
#include "Utils.hpp"

class Server
{
public:
	Server(const ServerConfig &config);
	~Server();

	void setupServer(); // bind(), socket setup
	int getListenFd() const;

private:
	// Rule of Three: Private and Unimplemented to prevent copying
	Server(const Server &other);
	Server &operator=(const Server &other);

	sockaddr_in _getSocketAddress(const std::string &host, int port) const;

	ServerConfig _config;
	int _listenFd;
	sockaddr_in _address;

	static const int BACKLOG = 128; // limit of connections in the socket's listen queue
};

#endif
