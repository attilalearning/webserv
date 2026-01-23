/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 14:46:16 by mosokina          #+#    #+#             */
/*   Updated: 2026/01/23 15:26:09 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdexcept>
#include <cstring>
#include <iostream>

#include "ConfigStructs.hpp"

class Server{
	public:
		Server(const ServerConfig config);
		~Server();
		void setupServer(); //bind(), socket setup
		int getListenFd() const;

	private:
		ServerConfig _config;
		int _listenFd;
		struct sockaddr_in _address;
}

#endif
