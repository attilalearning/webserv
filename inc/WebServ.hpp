/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 19:03:57 by aistok            #+#    #+#             */
/*   Updated: 2026/02/06 13:15:52 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <vector>
#include <map>
#include <poll.h> // For struct pollfd
#include <csignal>

#include "ConfigStructs.hpp"
#include "Server.hpp"


extern volatile sig_atomic_t g_server_running;

/* Why Pointers <Server *>:
 1. Memory Stability: std::vector reallocates memory as it grows. Using
 pointers ensures Server objects stay at a fixed address, preventing
 dangling pointers in our _fdToServerMap or poll system.
 2. Polymorphism: Allows storing different types of Server subclasses
 if the project expands.
 3. Efficiency: Avoids expensive "deep copies" of Server objects
 during vector resizing.
 */

class WebServ
{

public:
	WebServ();
	~WebServ();

	std::vector<Server *> getServers() const;
	void setup(std::vector<ServerConfig> &configs);
	void run();

protected:
	//...

private:
	// Rule of Three: Private and Unimplemented
	WebServ(const WebServ &other);
	WebServ &operator=(const WebServ &other);

	bool _isListener(int fd);


	std::vector<Server *> _servers; // all server instances
	std::vector<struct pollfd> _pollFds; // poll array for the whole program
	std::map<int, Server*> _fdToServerMap; // helps quickly find which server owns which FD
	static const std::string _name;
};

#endif // WEBSERV_HPP
