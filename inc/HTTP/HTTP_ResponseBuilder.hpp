/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_ResponseBuilder.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:48:39 by aistok            #+#    #+#             */
/*   Updated: 2026/04/09 03:08:11 by aistok           ###   ########.fr       */
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

/*	this class should not be instantiable
 *	and should only contain static methods
 *
 *	TO-DO: orthodox canonical form
 */
class HTTP_ResponseBuilder
{
public:
	static void build(
		HTTP_Response &response,
		HTTP_Request &request,
		const ServerConfig &sc);

private:
	static void build_response_for_GET_or_HEAD(
		HTTP_Response &response,
		HTTP_Request &request,
		const ServerConfig &sc);

	static void build_response_for_POST(
		HTTP_Response &response,
		HTTP_Request &request,
		const ServerConfig &sc);

	static void build_response_for_DELETE(
		HTTP_Response &response,
		HTTP_Request &request,
		const ServerConfig &sc);

	static bool locationHasMethod(LocationConfig &loc, std::string method);

	static const LocationConfig &locationGetBestMatch(
		const ServerConfig &serverConfig, const HTTP_Request &request);

	static std::string translateUriToPath(
		const HTTP_Request &request,
		const LocationConfig &location,
		const ServerConfig &sc,
		bool asAlias);

	static void setResponse(
		HTTP_Response &response,
		const HTTP_StatusPair &status,
		const ServerConfig &sc);

	static void setResponseRedirect(
		HTTP_Response &response,
		const LocationConfig &loc);
	static void setResponseRedirect(
		HTTP_Response &response,
		const int statusCode,
		const std::string url);
};

#endif // HTTP_RESPONSEBUILDER_HPP
