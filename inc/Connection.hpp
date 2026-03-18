/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 12:49:22 by mosokina          #+#    #+#             */
/*   Updated: 2026/03/12 01:22:57 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <iostream>
#include <unistd.h> // close
#include <ctime>

#include "Server.hpp"
#include "HTTP/HTTP.hpp"

class Connection
{
public:
	Connection(int fd, const sockaddr_in &clientAddr, Server *server); // check later -  const for Server)
	~Connection();

	void resetTimeout();
	bool isTimedOut(time_t now, int limit) const;
	HTTP::Request &getRequest();
	HTTP::Response &getResponse();
	Server *getServer();
	void appendRawRequest(const char *buffer, ssize_t bytesRead);
	bool isHeadersComplete();
	void handleRead(const char *buffer, ssize_t bytesRead);


	enum ConnectionState
	{
		READING_HEADERS,
		READING_BODY,
		REQUEST_READY,
		RESPONDING
	};

private:
	// Rule of Three: Private and Unimplemented to prevent copying
	Connection(const Connection &other);
	Connection &operator=(const Connection &other);

	// bool _isRequestComplete(); // TO-DO
	ConnectionState _state;
	int _connectFd;
	sockaddr_in _clientAddr;
	Server *_server; // for getting  client_max_body_size from the server config
	std::string _rawRequest; // as a buffer to accumulate data from multiple recv() calls
	// std::string _clientIP;
	time_t _lastActive; // Great for timeout logic!

	// Parsed data
	HTTP::Request     _request;
	HTTP::Response    _response;
};

#endif