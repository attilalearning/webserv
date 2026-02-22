/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 12:49:10 by mosokina          #+#    #+#             */
/*   Updated: 2026/02/13 15:15:21 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(int fd, const sockaddr_in &clientAddr, Server *server) : _connectFd(fd), _clientAddr(clientAddr), _server(server)
{
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
