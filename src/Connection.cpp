#include "Connection.hpp"

Connection::Connection(int fd, sockaddr_in clientAddr) : _connectFd(fd), _clientAddr(clientAddr) {}

Connection::~Connection()
{
	if (_connectFd != -1)
	{
		std::cout << "Closing fd " << _connectFd << std::endl;
		close(_connectFd);
		_connectFd = -1;
	}
}

