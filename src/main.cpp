/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 19:02:21 by aistok            #+#    #+#             */
/*   Updated: 2026/01/23 15:15:32 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <iostream>
#include "../inc/WebServ.hpp"
#include "../inc/ConfigStructs.hpp"
#include "../inc/ConfigParser.hpp"
#include "../inc/Server.hpp"


/*JUST FOR TESTING 2ND PART*/
std::vector<ServerConfig> getMockConfig() {
	std::vector<ServerConfig> configs;

	// --- SERVER 1 (Port 8080) ---
	ServerConfig s1;
	s1.port = 8080;
	s1.host = "127.0.0.1";
	s1.max_body_size = 1048576; // 1MB

	// Location / (Root)
	Location loc1;
	loc1.path = "/";
	loc1.root = "./www/html";
	loc1.index = "index.html";
	loc1.methods.push_back("GET");
	loc1.autoindex = true;
	s1.locations.push_back(loc1);

	configs.push_back(s1);

	// --- SERVER 2 (Port 9090) ---
	ServerConfig s2;
	s2.port = 9090;
	s2.host = "127.0.0.1";
	s2.max_body_size = 5000; // Small limit for testing 413 errors

	// Location /uploads
	Location loc2;
	loc2.path = "/uploads";
	loc2.root = "./www/uploads";
	loc2.methods.push_back("POST");
	loc2.methods.push_back("DELETE");
	s2.locations.push_back(loc2);

	configs.push_back(s2);

	return configs;
}

int main(int argc, char **argv)
{
	if (argc > 2){
		std::cerr << "Usage: ./webserv [config_file]" << std::endl;
		return 1;
	}
	
	std::string configPath = (argc == 2) ? argv[1] : "../default.conf";

	try
	{
		Config parser;
		parser.parse(configPath);
		std::vector<ServerConfig> configs = parser.getMockConfig();
		
		WebServ ws;
		
		ws.setup(configs);


		// TEMPORARY TEST in main.cpp
		std::cout << "Waiting for telnet on port " << configs[0].port << "..." << std::endl;

		// Very basic accept loop just to see if it works:
		int listenFd = ws.getServers()[0]->getListenFd();
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);

		while (true) {
			int client_fd = accept(listenFd, (struct sockaddr *)&client_addr, &client_len);
			if (client_fd != -1) {
				std::cout << "Connection accepted! FD: " << client_fd << std::endl;
				close(client_fd); // Hang up immediately for now
			}
		}

		// ws.run();
	}
	catch(const std::exception& e)
	{
		std::cerr  << "Error: " << e.what() << '\n';
		return 1;
	}
	return (0);
}


// TEMPORARY TEST in main.cpp
std::cout << "Waiting for telnet on port " << configs[0].port << "..." << std::endl;

// Very basic accept loop just to see if it works:
int listen_fd = ws.getServers()[0]->getListenFd();
struct sockaddr_in client_addr;
socklen_t client_len = sizeof(client_addr);

while (true) {
    int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd != -1) {
        std::cout << "Connection accepted! FD: " << client_fd << std::endl;
        close(client_fd); // Hang up immediately for now
    }
}