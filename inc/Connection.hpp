/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 12:49:22 by mosokina          #+#    #+#             */
/*   Updated: 2026/02/16 17:49:47 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <iostream>
#include <unistd.h> // close
#include "Server.hpp"

class Connection
{
public:
	Connection(int fd, const sockaddr_in &clientAddr, Server *server); // check later -  const for Server)
	~Connection();

private:
	// Rule of Three: Private and Unimplemented to prevent copying
	Connection(const Connection &other);
	Connection &operator=(const Connection &other);

	// void _appendRequest(char *buffer, int bytesRead); // TO-DO
	// bool _isRequestComplete(); // TO-DO

	int _connectFd;
	sockaddr_in _clientAddr;
	Server *_server; // for getting  client_max_body_size from the server config
	std::string _rawRequest; // as a buffer to accumulate data from multiple recv() calls
	// std::string _clientIP;
	// time_t _lastActivity; // Great for timeout logic!

	// Parsed data
	// HttpRequest     _request;
	// HttpResponse    _response;
};

#endif