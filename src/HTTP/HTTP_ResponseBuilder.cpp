/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_ResponseBuilder.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:48:39 by aistok            #+#    #+#             */
/*   Updated: 2026/02/25 11:55:59 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP/HTTP_ResponseBuilder.hpp"

std::string HTTP_ResponseBuilder::serverBasePath = std::string("./");

HTTP_Response HTTP_ResponseBuilder::build(ServerConfig &sc, HTTP_Request &hReq)
{
	(void) sc;
	if (hReq.parseStatus == HTTP_Request::BAD_REQUEST)
		return (HTTP_Response(HTTP_Status::BAD_REQUEST)); //(build(BAD_REQUEST, sc));

	if (hReq.parseStatus == HTTP_Request::INCOMPLETE)
	{
		// throw exception ?
	}

	if (hReq.method == HTTP_Method::GET)
	{
		return (HTTP_Response()); //(build_response_for_GET(sc, hReq));
	}
	else if (hReq.method == HTTP_Method::POST)
	{
		return (HTTP_Response()); //(build_response_for_POST(sc, hReq));
	}
	else if (hReq.method == HTTP_Method::DELETE)
	{
		return (HTTP_Response()); //(build_response_for_DELETE(sc, hReq));
	}
	return (HTTP_Response(HTTP_Status::FORBIDDEN));
}

HTTP_Response HTTP_ResponseBuilder::build_response_for_GET(
	std::vector<ServerConfig> configs, HTTP_Request hReq)
{
	HTTP_Response hResp;
	ServerConfig *choosenSC;
	Location *loc;

	try
	{
		loc = &locationGetBestMatch(configs, hReq.url, hReq.headers[HTTP_FieldName::HOST], &choosenSC);
	}
	catch (std::exception &e)
	{
		// need to somehow load a default error page from server
		// or from serverConfig if there is any for the server or for the location
		return (HTTP_Response(HTTP_Status::NOT_FOUND,
			ErrorPages::generate(HTTP_Status::NOT_FOUND)));
	}

	std::vector<std::string>::iterator method_it = loc->methods.begin();
	for (; method_it != loc->methods.end(); ++method_it)
	{
		if (*method_it == HTTP_Method::GET)
			break;
	}
	if (method_it == loc->methods.end())
	{
		// the GET method was not found for the location
		// response 403 forbidden needs to be returned
		return (hResp);
	}

	if (resourceIsDir(configs, *loc, hReq.url))
	{
		if (!loc->autoindex)
		{
			// 403 forbidden
			return (hResp);
		}
		// read dir content of requestUrl
		//
		// if content not reachable because of permissions -> 403 forbidden
		//
		// add content to hResp.body, adjust hResp.bodyLen,
		// add required headers, ex. content-length (OR transfer-encoding)
		return (hResp);
	}
	else
	{
		// resource is a file
		//
		// read file content of requestUrl
		//
		// if content not reachable because of permissions -> 403 forbidden
		//
		// add content to hResp.body, adjust hResp.bodyLen,
		// add required headers, ex. content-length (and mime-type?)
		return (hResp);
	}

	/*
	 *	response is based on sc.path, sc.root, sc.index and autoindex
	 *		and requestedUrl
	 *
	 *	we have to go through all the locations in serverConfig and check if
	 *	the requestUrl starts with any of the sc.path's
	 *	the locations in the serverConfig need to be sorted from the longest to
	 *	the shortest, ex:
	 *		/home/pages/contact			1st loc entry
	 *		/home/companyDescription	2nd loc entry
	 *		/contactUs					3rd loc entry
	 *		/							4th loc entry
	 *	and we need to match the longest such path to then check properties in
	 *	ex. for requestedUrl = "/home/pages/contact/email-form" the 1st entry should be matched
	 *		for requestedUrl = "/contactUs/ITDepartment/teams" the 3rd entry should be matched
	 *
	 *	if there is a match, if the requestUrl is a directory && autoindex = off -> 403 forbidden
	 *														  && autoindex = on -> generate directory listing
	 *										      a file, open file and copy contents into the response body
	 *						if directory or file is not accessible (no permissions) -> 403 forbidden
	 *
	 * 	if there is no match, check if autoindex is 'on' and if so, proceed with
	 *	obtaining the directory listing, generate a HTML as body for the response
	 *
	 *	if there is no match and autoindex = off, send 403 Forbidden
	 *
	 */

	hResp.setStatus(HTTP_Status::OK); // ?
	return (hResp);
}

Location &HTTP_ResponseBuilder::locationGetBestMatch(
	std::vector<ServerConfig> configs,
	std::string requestedUrl,
	std::string hostFromRequest,
	ServerConfig **matchingServerConfig)
{
	ServerConfig *serverConfig = NULL;

	// QUESTION: shall we ever test if configs.size() == 0 ? (probably not)

	std::vector<ServerConfig>::iterator it;
	if (configs.size() > 1)
	{
		for (it = configs.begin(); it != configs.end(); ++it)
		{
			*serverConfig = *it;
			if (serverConfig->host == hostFromRequest)
				break; // we found the serverConfig matching hostFromRequest

			std::vector<std::string>::iterator sn_it = serverConfig->server_names.begin();
			for (; sn_it != serverConfig->server_names.end(); ++sn_it)
			{
				if (*sn_it == hostFromRequest)
					break; // we found the serverConfig matching hostFromRequest
			}
		}
	}
	else
	{
		// if only one ServerConfig, use that one (there is no ambiguity)
		serverConfig = &configs[0];
	}

	if (it == configs.end())
	{
		// hostFromRequest wasn't found in any of the servers configs
		// just use the 1st one defined
		serverConfig = &configs[0];
	}

	// now, go through the locations and match the best one
	Location *selected = NULL;
	std::vector<Location>::iterator loc_it = serverConfig->locations.begin();
	for (; loc_it != serverConfig->locations.end(); ++loc_it)
	{
		Location location = *loc_it;
		if (location.path.find(requestedUrl) == 0)
		{
			if (!selected)
				selected = &location;
			else if (location.path.length() > selected->path.length())
				selected = &location;
		}
	}

	if (!selected)
		throw std::runtime_error("No suitable server/location found!");
	*matchingServerConfig = serverConfig;
	return (*selected);
}

bool HTTP_ResponseBuilder::resourceIsDir(std::vector<ServerConfig> configs, Location &location, std::string requestUrl)
{
	(void) configs;
	(void) location;
	(void) requestUrl;

	//...

	return (true);
}
