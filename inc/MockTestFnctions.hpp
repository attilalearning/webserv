#ifndef MOCKTESTFUNCTIONS_HPP
#define MOCKTESTFUNCTIONS_HPP

#include <iostream>
#include <csignal>

#include "../inc/WebServ.hpp"
#include "../inc/ConfigStructs.hpp"
#include "../inc/Server.hpp"

std::vector<ServerConfig> getMockConfig();
void runTemporaryTest(WebServ &ws);

#endif