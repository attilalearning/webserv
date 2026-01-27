/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 14:46:16 by mosokina          #+#    #+#             */
/*   Updated: 2026/01/27 00:49:24 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h> // for htons, inet_addr, sockaddr_in
#include <fcntl.h> // fcntl, O_NONBLOCK
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <unistd.h>

#include "ConfigStructs.hpp"
#include "Utils.hpp"

class Server{
	public:
		Server(const ServerConfig config);
		~Server();
		
		void setupServer(); //bind(), socket setup
		int getListenFd() const;

	private:
		// Rule of Three: Private and Unimplemented
		Server(const Server& other);
		Server& operator=(const Server& other);
		
		ServerConfig _config;
		int _listenFd;
		struct sockaddr_in _address;
};

#endif
