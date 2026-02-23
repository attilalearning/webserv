/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 19:03:57 by aistok            #+#    #+#             */
/*   Updated: 2026/02/23 23:19:00 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#define ESC_VIOLET_HOLLOW "\001\033[45;97m\002"
#define ESC_YELLOW_HOLLOW "\001\033[43;97m\002"
#define ESC_CYAN "\001\033[36m\002"
#define ESC_GREEN "\001\033[32m\002"
#define ESC_RED "\001\033[31m\002"
#define ESC_END "\001\033[0m\002"

#include <iostream>
#include <vector>
#include <map>
#include <poll.h> // For struct pollfd
#include <csignal>
#include <cerrno>
#include <cstring>

#include "ConfigStructs.hpp"
#include "Server.hpp"
#include "Connection.hpp"

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

private:
	// Rule of Three: Private and Unimplemented
	WebServ(const WebServ &other);
	WebServ &operator=(const WebServ &other);

	void _addNewFdtoPool(int newFd, short events);

	bool _isListener(int fd);
	void _acceptNewConnection(int listenFd);
	void _closeConnection(size_t index);
	bool _readRequest(size_t index); // return status of connection (opened/closed)

	static const int POLL_TIMEOUT = 1000;	// Wait up to 1 sec for events
	static const int BUFFER_SIZE = 4096;
	std::vector<Server *> _servers;			// all server instances
	std::vector<pollfd> _pollFds;			// poll array for the whole program
	std::map<int, Server *> _fdToServerMap; // helps quickly find which server owns which FD
	std::map<int, Connection *> _fdToConnMap;
};

#endif // WEBSERV_HPP
