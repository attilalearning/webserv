/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:12:31 by mosokina          #+#    #+#             */
/*   Updated: 2026/01/20 13:10:26 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "ConfigStructs.hpp"
#include <string>
#include <vector>


class Config {
	public:
		Config();
		Config(const Config& other);
		Config& operator=(const Config& other);
		~Config();
		
		const std::vector<ServerConfig>& getConfigs() const;
		void parse(const std::string &configPath);
		
	private:
		std::vector<ServerConfig> _servers;
		std::vector<std::string> _tokens;
		void _tokenize(const std::string &configPath);
		void _parseServer(size_t& i);
};

#endif

	// Helper methods for the parsing process
	// void tokenize(const std::string& path);
	// void parseServer(size_t& i);
	// void parseLocation(size_t& i, ServerConfig& server);

#endif