/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 19:02:21 by aistok            #+#    #+#             */
/*   Updated: 2026/02/06 13:03:20 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <csignal>

#include "../inc/WebServ.hpp"
#include "../inc/ConfigStructs.hpp"
#include "../inc/Server.hpp"

/*JUST FOR TESTING (getMockConfig() should be replaced by ConfigParser)*/
std::vector<ServerConfig> getMockConfig()
{
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

// FOR TESTING (runTemporaryTest() should be replaced by run() with poll() approach):
void runTemporaryTest(WebServ &ws)
{
	if (ws.getServers().empty())
	{
		std::cerr << "Error: No servers initialized to test." << std::endl;
		return;
	}

	Server *firstServer = ws.getServers()[0];
	int listenFd = firstServer->getListenFd();

	// Safety check: Did the socket actually open?
	if (listenFd == -1)
	{
		std::cerr << "Error: Listen FD is invalid (-1). Check bind errors." << std::endl;
		return;
	}

	std::cout << "--- [STARTING TEMPORARY TEST] ---" << std::endl;
	std::cout << "Waiting for telnet on the first configured port..." << std::endl;

	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);

	while (true)
	{
		int clientFd = accept(listenFd, (struct sockaddr *)&clientAddr, &clientLen);

		if (clientFd == -1)
		{
			// Since it's O_NONBLOCK, we sleep to prevent 100% CPU usage
			usleep(10000);
			continue;
		}

		std::cout << "Connection accepted! FD: " << clientFd << std::endl;

		// 1. Send the greeting
		std::string msg = "Hello World! Type something and hit Enter:\n";
		send(clientFd, msg.c_str(), msg.length(), 0);

		// 2. Read user input
		char buffer[1024] = {0};
		int bytesRead = recv(clientFd, buffer, 1023, 0); // 1023 to save space for null

		if (bytesRead > 0)
		{
			buffer[bytesRead] = '\0'; // Null-terminate for safe printing
			std::cout << "Received from client: " << buffer << std::endl;
		}

		// 3. Close the connection
		close(clientFd);
		std::cout << "Connection closed. Ready for next test..." << std::endl;
	}
}

/*How to Test:
1 - Run it: ./webserv
2 - Open a new terminal and type: telnet localhost 8080 (or whatever port you used).
3 - Type anything and press Enter.
4 - Result: we can see "Hello World!" in the telnet window,
and the server terminal should log the connection and disconnection.*/


volatile sig_atomic_t g_server_running = 1;
void handleSigint(int sig)
{
	void()sig;
	g_server_running = 0;
}

int main(int argc, char **argv)
{
	signal(SIGPIPE, SIG_IGN); // Prevents crash on broken pipe
	signal(SIGINT, handle_sigint); // Handles Ctrl+C
	
	if (argc > 2)
	{
		std::cerr << "Usage: ./webserv [config_file]" << std::endl;
		return 1;
	}
	std::string configPath = (argc == 2) ? argv[1] : "../default.conf";
	try
	{

		// FOR TESTING (getMockConfig() should be replaced by ConfigParser)
		std::vector<ServerConfig> configs = getMockConfig();

		WebServ ws;
		ws.setup(configs);

		// ws.run();
		// FOR TESTING (runTemporaryTest() should be replaced by run() with poll() approach):
		runTemporaryTest(ws);
		std::cout << "\nServer shutting down cleanly..." << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << '\n';
		return 1;
	}
	return (0);
}
