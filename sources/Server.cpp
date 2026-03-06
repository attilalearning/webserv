#include "Server.hpp"
#include "Utils.hpp"
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>


Server::Server() : _running(false) {}

Server::Server(const std::string& config_file) : _running(false) {
    init(config_file);
}

Server::~Server() {
    //stop();
}



/*Server init modified to print loaded config file.*/
void Server::init(const std::string& config_file) {
    _config.load(config_file);
    /*Trying to print loaded config from memory*/
    std::vector<ServerConfig> loaded_servers = _config.getServers();

    for (size_t i = 0; i < loaded_servers.size(); ++i)
    {
        /*Printing ports*/
        if (!loaded_servers[i].server_names.empty())
            std::cout << "Servername is: " << loaded_servers[i].server_names[0];
        else
            std::cout << "Server " << i + 1 << ":";
        std::vector<int> loaded_ports = loaded_servers[i].ports;


        std::vector<LocationConfig> loaded_locations = loaded_servers[i].locations;

        for (size_t j = 0; j < loaded_locations.size(); ++j) {
            std::cout << "location path: " << loaded_locations[j].path;
            for (size_t k = 0; k < loaded_locations[j].allowed_methods.size(); k++)
                std::cout << "allowed_methods: " << loaded_locations[j].allowed_methods[k] << std::endl;
        }
        for (size_t i = 0; i < loaded_ports.size(); i++)
            std::cout << " port" << i + 1 << ": " << loaded_ports[i];
        std::cout << std::endl;
    }    
    std::cout << loaded_servers.size() << "servers loaded."  <<std::endl;
}

void Server::run() {
    //
}

void Server::stop() {
    //
}