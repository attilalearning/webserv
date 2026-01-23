/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 19:03:57 by aistok            #+#    #+#             */
/*   Updated: 2026/01/23 15:13:19 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "WebServ.hpp"

/* public section ----------------------------- */

WebServ::WebServ()
{
	std::cout << "This is gooooing to be the " << _name << "!" << std::endl;
}

WebServ::~WebServ()
{
	for (size_t i = 0; i < _servers.size(); ++i) {
	delete _servers[i]; // This triggers the Server destructor and closes the FD
	}
	std::cout << _name << ": bye!" << std::endl;
}

// WebServ::WebServ(const WebServ &other)
// {
// 	(void) other;
// 	//..
// }

// WebServ &WebServ::operator=(const WebServ &other)
// {
// 	(void) other;
// 	//..
// 	return (*this);
// }

std::vector<Server*> WebServ::getServers() const
{
	return _servers;
}

void WebServ::run(void)
{
	std::cout << _name << ": running (not quite just yer)" << std::endl;
	while (true) {
		int ret = poll(&_poll_fds[0], _poll_fds.size(), -1);
		// Handle events...
	}
}

void WebServ::setup(std::vector<ServerConfig>& configs){
	for (size_t i = 0; i < configs.size(); ++i) {
		Server *newServer = new Server(configs[i]);
		newServer->setupServer(); // Creates the socket, bind, listen
	
	try{
		_servers.push_back(newServer);
		
		// Add the listening socket to our master poll vector
		struct pollfd pfd;
		pfd.fd = newServer->getListenFd();
		pfd.events = POLLIN;
		pfd.revents = 0; // Good practice to initialize this

		_pollFds.push_back(pfd);
		_fdToServerMap[pfd.fd] = newServer;			
		} catch (...){
			delete newServer;
			throw;
		}
	}
}

/* protected section -------------------------- */

//...

/* private section ---------------------------- */

const std::string WebServ::_name = "WebServ";

