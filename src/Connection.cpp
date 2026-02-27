/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 12:49:10 by mosokina          #+#    #+#             */
/*   Updated: 2026/02/25 14:19:53 by mosokina         ###   ########.fr       */
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
