/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:56:02 by mosokina          #+#    #+#             */
/*   Updated: 2026/01/20 13:15:52 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ConfigParser.hpp"

Config::Config() {}

Config::Config(const Config& other) 
	: _servers(other._servers), _tokens(other._tokens) {
}

Config& Config::operator=(const Config& other) {
	if (this != &other) {
		this->_servers = other._servers;
		this->_tokens = other._tokens;
	}
	return *this;
}

Config::~Config() {}

const std::vector<ServerConfig>& Config::getConfigs() const {
	return this->_servers;
}


void Config::parse(const std::string &configPath){
	_tokenize(configPath);
	for (size_t i = 0; i < _tokens[i]; ++i){
		if (_tokens[i] == "server") {
			_parseServer(i); // This function should increment i to the '}'
		} else {
			throw std::runtime_error("Unknown keyword outside server block: " + _tokens[i]);
		}
	}
}

void Config::_tokenize(const std::string &configPath){
	
}

void Config::_parseServer(size_t& i){
	
}



// void Config::parse(const std::string& path) {
//     tokenize(path);
//     for (size_t i = 0; i < _tokens.size(); ++i) {
//         if (_tokens[i] == "server") {
//             parseServer(i);
//         } else {
//             throw std::runtime_error("Unexpected global token: " + _tokens[i]);
//         }
//     }
// }