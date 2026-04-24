/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_ResponseBuilder.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:48:39 by aistok            #+#    #+#             */
/*   Updated: 2026/04/24 10:34:05 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RESPONSEBUILDER_HPP
#define HTTP_RESPONSEBUILDER_HPP

#include "Utils.hpp"
#include "Config.hpp"
#include "HTTP/HTTP_Request.hpp"
#include "HTTP/HTTP_Response.hpp"
#include "ErrorPages.hpp"
#include "DirectoriesToHTML.hpp"

/*
 *	TO-DO: orthodox canonical form
 */
class HTTP_ResponseBuilder
{
public:
	HTTP_ResponseBuilder();
	HTTP_ResponseBuilder(const ServerConfig &sc);
	//HTTP_ResponseBuilder(const HTTP_ResponseBuilder &other);
	//HTTP_ResponseBuilder &operator=(const HTTP_ResponseBuilder &other);
	~HTTP_ResponseBuilder();

	void build(HTTP_Response &response, HTTP_Request &request);
	void reset();

private:
	ServerConfig _serverConfig;
	LocationConfig _location;
	std::string _pathOnServer;
	PathType _pathType;

	HTTP_ResponseBuilder(const HTTP_ResponseBuilder &other);
	HTTP_ResponseBuilder &operator=(const HTTP_ResponseBuilder &other);

	void build_response_for_GET_or_HEAD(HTTP_Response &response, HTTP_Request &request);
	void build_response_for_POST(HTTP_Response &response, HTTP_Request &request);
	void build_response_for_DELETE(HTTP_Response &response, HTTP_Request &request);
	void build_response_by_CGI(HTTP_Response &response, HTTP_Request &request);

	bool locationHasMethod(std::string method);
	const LocationConfig &locationGetBestMatch(const HTTP_Request &request);
	std::string translateUriToPath(const HTTP_Request &request, bool asAlias);

	void setResponse(HTTP_Response &response, const HTTP_StatusPair &status);

	void setResponseRedirect(HTTP_Response &response, const int statusCode, const std::string &url);
};

#endif // HTTP_RESPONSEBUILDER_HPP
