#ifndef SERVER_HPP
#define SERVER_HPP

#include "Config.hpp"
#include "Client.hpp"
#include <vector>
#include <map>
#include <poll.h>

class Server
{
    public:
        Server();
        explicit Server(const std::string& config_file);
        ~Server();

        void init(const std::string& config_file);
        void run();
        void stop();

    private:
        Config _config;
        // std::vector<int> _listen_fds;
        // std::map<int, Client*> _clients;
        // std::vector<struct pollfd> _poll_fds;

        //Socket setup

        //Client management

        //Request processing

        //Helpers
        bool _running;

};

#endif