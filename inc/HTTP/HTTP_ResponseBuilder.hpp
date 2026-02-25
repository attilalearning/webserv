/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_ResponseBuilder.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:48:39 by aistok            #+#    #+#             */
/*   Updated: 2026/02/25 11:55:50 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RESPONSEBUILDER_HPP
#define HTTP_RESPONSEBUILDER_HPP

#include "ConfigStructs.hpp"
#include "HTTP/HTTP_Request.hpp"
#include "HTTP/HTTP_Response.hpp"
#include "ErrorPages.hpp"

/*	this class should not be instantiable
 *	and should only contain static methods
 *
 *	TO-DO: orthodox canonical form
 */
class HTTP_ResponseBuilder
{
public:
	/*
	 *	This is temporary, can be moved later to other struct or class
	 *
	 *	if webserv is given a config file as argument,
	 *	this variable will be overwritten by the the path to that config file
	 *
	 *	if no argument is present, this path will be the dafault,
	 *	where the WebServ will try to look for a config file.
	 *
	 */
	static std::string serverBasePath; // ?

	HTTP_Response build(
		ServerConfig &sc, HTTP_Request &hReq);

	HTTP_Response build_response_for_GET(
		std::vector<ServerConfig> configs, HTTP_Request hReq);

	Location &locationGetBestMatch(
		std::vector<ServerConfig> configs,
		std::string requestedUrl,
		std::string hostFromRequest,
		ServerConfig **matchingServerConfig);
	
	bool resourceIsDir(std::vector<ServerConfig> configs, Location &location, std::string requestUrl);
};

#endif // HTTP_RESPONSEBUILDER_HPP
