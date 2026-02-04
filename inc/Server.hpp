/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 14:46:16 by mosokina          #+#    #+#             */
/*   Updated: 2026/02/04 00:10:13 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>  // htons, inet_addr
#include <fcntl.h>      // fcntl, O_NONBLOCK
#include <netdb.h>      // getaddrinfo, addrinfo
#include <sys/socket.h> // socket, bind, listen
#include <sys/types.h>  // types for sockets
#include <unistd.h>     // close
#include <iostream>
#include <stdexcept>

#include "ConfigStructs.hpp"
#include "Utils.hpp"

class Server{
	public:
		Server(const ServerConfig &config);
		~Server();
		
		void setupServer(); //bind(), socket setup
		int getListenFd() const;

	private:
		// Rule of Three: Private and Unimplemented to prevent copying
		Server(const Server& other);
		Server& operator=(const Server& other);

		struct sockaddr_in _getSocketAddress(const std::string& host, int port);

		ServerConfig _config;
		int _listenFd;
		struct sockaddr_in _address;
};

#endif
