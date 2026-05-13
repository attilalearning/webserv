/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_ResponseBuilder.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:48:39 by aistok            #+#    #+#             */
/*   Updated: 2026/05/13 17:28:48 by aistok           ###   ########.fr       */
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
#include "CGI.hpp"

#include <exception>
#include <string>

class HTTP_ResponseBuilder
{
public:

	class Exception : public std::exception
	{
	public:
		Exception(const HTTP_StatusPair &status, const std::string &msg);

		virtual ~Exception() throw();

		virtual const char *what() const throw();

		HTTP_StatusPair getStatus() const;

	private:
		HTTP_StatusPair _status;
		std::string _message;
	};

	HTTP_ResponseBuilder();
	HTTP_ResponseBuilder(const ServerConfig &sc);
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

	bool locationHasMethod(const LocationConfig &location, std::string method);
	const LocationConfig &locationGetBestMatch(const HTTP_Request &request);
	std::string translateUriToPath(const HTTP_Request &request);

	void setResponse(HTTP_Response &response, const HTTP_StatusPair &status);

	void setResponseRedirect(HTTP_Response &response, const int statusCode, const std::string &url);
};

#endif // HTTP_RESPONSEBUILDER_HPP
