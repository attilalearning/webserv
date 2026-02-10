#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <iostream>
#include <unistd.h>		// close

class Connection {
public:
	Connection(int fd, sockaddr_in clientAddr);
	~Connection();

	void setupConnection(); // bind(), socket setup
	int getConnectFd() const;

private:
	// Rule of Three: Private and Unimplemented to prevent copying
	Connection(const Connection &other);
	Connection &operator=(const Connection &other);

	int _connectFd;
	sockaddr_in _clientAddr;
	// std::string _clientIp;
	// time_t _lastActivity; // Great for timeout logic!

	// Parsed data
	// HttpRequest     _request; 
	// HttpResponse    _response;
	Connection(const Connection& other); // Non-copyable
	Connection& operator=(const Connection& other);

};

#endif