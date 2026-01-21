/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:56:02 by mosokina          #+#    #+#             */
/*   Updated: 2026/01/21 00:31:48 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ConfigParser.hpp"
#include <fstream>    // file streams
#include <sstream>    // string streams

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
	_validateBraces();
	for (size_t i = 0; i < _tokens.size(); ++i){
		if (_tokens[i] == "server") {
			_parseServer(i); // this function increments i to the '}'
		} else {
			throw std::runtime_error("Unknown keyword outside server block: " + _tokens[i]);
		}
	}
}

//example: ["listen", "8080", ";"]
void Config::_tokenize(const std::string &configPath){
	std::ifstream confFile(configPath.c_str());
	if (!confFile.is_open()){
		throw std::runtime_error("Could not open config file");
	}
	
	std::string line;
	while(std::getline(confFile, line))
	{
		//1 - Remove comments(line starting with #)
		size_t commentInx = line.find('#');
		if (commentInx != std::string::npos){
			line = line.substr(0, commentInx);
		}
		//2 - Add spaces to {, }, and ;
		std::string newLine;
		for (size_t i = 0; i < line.length(); ++i){
			if (line[i] == '{' || line[i] == '}' || line[i] == ';'){
				newLine += ' ';
				newLine += line[i];
				newLine +=' ';
			}
			else{
				newLine += line[i];
			}
		}
		//3 - Extract Tokens
		std::istringstream iss(newLine); //input only
		std::string token;
		while (iss >> token){
			_tokens.push_back(token);
		}
		confFile.close();
		if (this->_tokens.empty()){
			throw std::runtime_error("Empty config file");
		}
	}
}

// global check of "Brace Balance"  '{' and '}'
void Config::_validateBraces() {
	int count = 0;
	for (size_t i = 0; i < _tokens.size(); ++i) {
		if (_tokens[i] == "{") {
			count++;
		} else if (_tokens[i] == "}") {
			count--;
		}
		// If count ever goes below 0, it means a '}' appeared before a '{'
		if (count < 0) {
			throw std::runtime_error("Closing brace '}' found without opening brace");
		}
	}
	if (count != 0) {
		throw std::runtime_error("Missing closing brace '}' somewhere in the file");
	}
}

void Config::_parseServer(size_t& i){
	// 1 - loops inside { } looking for listen, host, etc
	// 2 - _parseListen - port as int;
	// 3 - _parseLocation;
	// logic check of "braces balance" - throw an error if it reaches the end of the tokens and never found a }, or it finds a } in wrong place
}

void Config::_parseLocation(size_t& i){
	// 1 - Loops inside { }
	// 2 - Keyword (location) -> Path (/images) -> Opening Bracket ({).
	// 3 - Longest Prefix Match
	// 4 -  Allowed metthods ??
	//logic check of "braces balance" - throw an error if it reaches the end of the tokens and never found a }, or it finds a } in wrong place
}

/*
## Resources
1. "How nginx processes a request"
This is the most important page. It explains how Nginx decides which server block to use and which location block matches the URL.

Key Concept: The Host header (for choosing the server) and the longest prefix match (for choosing the location).

Search for: “nginx how it processes a request”

2. "Beginner’s Guide"
This covers the basic syntax: blocks ({}), directives (ending in ;), and the hierarchy.

Key Concept: The difference between a "Simple Directive" and a "Block Directive."

Search for: “nginx beginners guide configuration file”*/


/*More checks for parser:
1 - No Duplicate Ports etc:
If more then one server blocks with the same host:port and the same server_name, throw an error.

*/

