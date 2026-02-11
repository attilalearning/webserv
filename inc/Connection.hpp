#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <iostream>
#include <unistd.h>		// close
#include "Server.hpp"

class Connection {
public:
	Connection(int fd, sockaddr_in clientAddr, Server* server);
	~Connection();

private:
	// Rule of Three: Private and Unimplemented to prevent copying
	Connection(const Connection &other);
	Connection &operator=(const Connection &other);

	// void _appendRequest(char *buffer, int bytesRead); // TO-DO
	// bool _isRequestComplete(); // TO-DO

	int _connectFd;
	sockaddr_in _clientAddr;
	Server* _server;
	// std::string _clientIP;
	// time_t _lastActivity; // Great for timeout logic!

	// Parsed data
	// HttpRequest     _request; 
	// HttpResponse    _response;

};

#endif