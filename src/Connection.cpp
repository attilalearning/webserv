/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 12:49:10 by mosokina          #+#    #+#             */
/*   Updated: 2026/02/27 22:26:38 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(int fd, const sockaddr_in &clientAddr, Server *server) : _connectFd(fd), _clientAddr(clientAddr), _server(server)
{
	_lastActive = std::time(NULL);
}

Connection::~Connection()
{
	if (_connectFd != -1)
	{
		std::cout << "Closing fd " << _connectFd << std::endl;
		close(_connectFd);
		_connectFd = -1;
	}
}

void Connection::resetTimeout()
{
	_lastActive = std::time(NULL);
}

bool Connection::isTimedOut(time_t now, int limit) const
{
	return (std::difftime(now, _lastActive) >= limit);
}
HTTP::Request &Connection::getRequest()
{
	return (_request);
}

HTTP::Response &Connection::getResponse()
{
	return (_response);
}

Server *Connection::getServer()
{
	return (_server);
}
