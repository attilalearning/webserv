/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 19:03:57 by aistok            #+#    #+#             */
/*   Updated: 2026/01/23 15:27:51 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef WEBSERV_HPP
# define WEBSERV_HPP
# include <iostream>
# include <vector>
# include <map>
# include <poll.h> // For struct pollfd

#include "ConfigStructs.hpp"
#include "Server.hpp"

/* Why Pointers <Server *>? 
 1. Memory Stability: std::vector reallocates memory as it grows. Using 
 pointers ensures Server objects stay at a fixed address, preventing 
 dangling pointers in our _fdToServerMap or poll system.
 2. Polymorphism: Allows storing different types of Server subclasses 
 if the project expands.
 3. Efficiency: Avoids expensive "deep copies" of Server objects 
 during vector resizing.
 */

class WebServ {

	public:

		WebServ();
		~WebServ();
		// WebServ(const WebServ &other);
		// WebServ &operator=(const WebServ &other);
		
		// Disabling copy logic to prevent memory double-frees
		WebServ(const WebServ &other) = delete;
		WebServ &operator=(const WebServ &other) = delete;
		
		std::vector<Server*> getServers() const; // all server instances

		void setup(std::vector<ServerConfig>& configs);
		void run();

	protected:

		//...

	private:
		std::vector<Server*>	_servers; // all server instances
		std::vector<struct pollfd> _pollFds; // poll array for the while program
		std::map<int, Server*> _fdToServerMap; // helps quickly find which server owns which FD
		static const std::string _name;

};

#endif // WEBSERV_HPP
