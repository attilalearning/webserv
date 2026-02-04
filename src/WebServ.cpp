/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 19:03:57 by aistok            #+#    #+#             */
/*   Updated: 2026/02/04 11:03:49 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "../inc/WebServ.hpp"

/* public section ----------------------------- */

WebServ::WebServ()
{
	std::cout << "This is gooooing to be the " << _name << "!" << std::endl;
}

WebServ::~WebServ()
{
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		delete _servers[i]; // This triggers the Server destructor and closes the FD
	}
	std::cout << _name << ": bye!" << std::endl;
}

std::vector<Server *> WebServ::getServers() const
{
	return _servers;
}

void WebServ::setup(std::vector<ServerConfig> &configs)
{
	for (size_t i = 0; i < configs.size(); ++i)
	{
		Server *newServer = NULL;
		try
		{
			newServer = new Server(configs[i]);
			newServer->setupServer(); // Creates the socket, bind, listen
			_servers.push_back(newServer);

			// // // Add the listening socket to our master poll vector
			// struct pollfd pfd;
			// pfd.fd = newServer->getListenFd();
			// pfd.events = POLLIN;
			// pfd.revents = 0; // Good practice to initialize this

			// _pollFds.push_back(pfd);
			// _fdToServerMap[pfd.fd] = newServer;
		}
		catch (const std::exception &e)
		{
			delete newServer;
			std::cerr << "Failed to setup server: " << e.what() << std::endl;
		}
	}
}

void WebServ::run(void)
{
	// to be added later;
}

/* protected section -------------------------- */

//...

/* private section ---------------------------- */

const std::string WebServ::_name = "WebServ";
