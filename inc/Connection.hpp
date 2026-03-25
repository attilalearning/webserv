/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 12:49:22 by mosokina          #+#    #+#             */
/*   Updated: 2026/03/25 02:02:47 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <iostream>
#include <unistd.h> // close
#include <ctime>
#include <cstdlib>

#include "Server.hpp"
#include "HTTP/HTTP.hpp"

class Connection
{
public:
	Connection(int fd, Server *server); // check later -  const for Server)
	~Connection();

	enum ConnectionState
	{
		READING_HEADERS,
		READING_BODY,
		REQUEST_READY,
		WAITING_FOR_CGI,
		ERROR
	};
	
	void resetTimeout();
	bool isTimedOut(time_t now, int limit) const;
	
	HTTP::Request &getRequest();
	HTTP::Response &getResponse();
	Server *getServer();
	int getState() const;
    std::string getRawRequest() const;
	std::string getRawResponse() const;

	void handleRead(const char *buffer, ssize_t bytesRead);
	bool handleWrite();
	bool shouldClose() const;
	void prepareResponse();
	void forceTimeoutError();


	void resetForNextRequest();

	bool hasBufferedData() const ;
	// bool hasCompleteRequestInBuffer() const;

	static const int MAX_HEADER_SIZE = 16384; // 16KB


private:
	Connection(const Connection &other);
	Connection &operator=(const Connection &other);

	ConnectionState _state;
	int _connectFd;
	Server *_server; // for getting  client_max_body_size from the server config
	
	std::string _rawRequest;
	std::string	_chunkedAccumulator;
	std::string _rawResponse;


	size_t	_expectedBodySize;
    bool _isChunked;
	size_t _bytesSent;
	time_t _lastActive;

	HTTP::Request _request;
	HTTP::Response _response;

	// sockaddr_in _clientAddr;
	// std::string _clientIP;

	void _handleHeaders();
	void _setupBodyReading();
    void _handleStandardBody();
    void _handleChunkedBody();
	bool _isValidHex(const std::string& s) const;


};

#endif