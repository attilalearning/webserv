#include "Server.hpp"
#include <iostream>
#include <cstdlib>
// #include <csignal>

// Server* g_server = NULL;

// void signalHandler(int signum) {
//     std::cout << "\nReceived signal " << signum << std::endl;
//     if (g_server) {
//         g_server->stop();
//     }
//     exit(signum);
// }

int main(int ac, char **av) {
    // std::string config_file = "config/default.conf";
    std::string config_file = "config/advanced.conf";
    // std::string config_file = "config/test_duplicate_directive.conf";
    // std::string config_file = "config/test_invalid_method.conf";
    // std::string config_file = "config/test_missing_brace.conf";
    // std::string config_file = "config/test_missing_semicolon.conf";

    if (ac > 1)
        config_file = av[1];

    //Set up signal handlers
    // signal(SIGINT, signalHandler);
    // signal(SIGTERM, signalHandler);

    try {
        Server server(config_file);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return (1);
    }
    return (0);
}