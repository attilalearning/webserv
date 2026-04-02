/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_ResponseBuilder.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:48:39 by aistok            #+#    #+#             */
/*   Updated: 2026/04/02 10:37:54 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP/HTTP_ResponseBuilder.hpp"

void HTTP_ResponseBuilder::build(
	HTTP_Response &response,
	HTTP_Request &request,
	const ServerConfig &sc)
{
	if (request.getParseStatus() == HTTP_Request::BAD_REQUEST)
	{
		setResponse(response, HTTP_Status::BAD_REQUEST, sc);
		return;
	}

	if (request.getParseStatus() == HTTP_Request::INCOMPLETE)
	{
		// throw exception ?
		return; //?
	}

	if (request.getMethod() == HTTP_Method::GET)
		build_response_for_GET(response, request, sc);

	else if (request.getMethod() == HTTP_Method::POST)
		build_response_for_POST(response, request, sc);

	else if (request.getMethod() == HTTP_Method::DELETE)
		build_response_for_DELETE(response, request, sc);

	else
		setResponse(response, HTTP_Status::FORBIDDEN, sc);
}

void HTTP_ResponseBuilder::setResponse(
	HTTP_Response &response,
	const HTTP_StatusPair &status,
	const ServerConfig &sc)
{
	response.setStatus(status);
	response.setContent(ErrorPages::getContent(sc, status));
}

bool HTTP_ResponseBuilder::locationHasMethod(LocationConfig &loc, std::string method)
{
	std::vector<std::string>::iterator method_it = loc.allowed_methods.begin();
	for (; method_it != loc.allowed_methods.end(); ++method_it)
	{
		if (*method_it == method)
			return (true);
	}
	return (false);
}

void HTTP_ResponseBuilder::build_response_for_GET(
	HTTP_Response &response,
	HTTP_Request &request,
	const ServerConfig &sc)
{
	LocationConfig location;

	try
	{
		location = locationGetBestMatch(sc, request);
	}
	catch (std::exception &e)
	{
		std::cout << "HTTP_ResponseBuilder::build_response_for_GET - location"
				  << std::endl
				  << e.what() << std::endl;

		setResponse(response, HTTP_Status::NOT_FOUND, sc);
		return;
	}

	// TO-DO ???
	// need to consider the redirects from the config file, if there are any
	// like:
	// location / {
	//      [method = GET]
	// 		return 301 [POST] /some/url/here
	// }

	if (!locationHasMethod(location, HTTP_Method::GET))
	{
		// the GET method was not found for the location
		setResponse(response, HTTP_Status::FORBIDDEN, sc);
		return;
	}

	std::string pathOnServer;
	try
	{
		pathOnServer = translateUriToPath(location, request, true);
		std::cout << "pathOnServer: " << pathOnServer << "\n";
	}
	catch (std::exception &e)
	{
		setResponse(response, HTTP_Status::BAD_REQUEST, sc);
		return;
	}

	PathType pathType = getPathType(pathOnServer);

	if (pathType == PATH_NONE)
	{
		setResponse(response, HTTP_Status::NOT_FOUND, sc);
		return;
	}
	else if (pathType == PATH_FILE)
	{
		response.setStatus(HTTP_Status::OK);
		response.setContent(Utils::getFileContent(pathOnServer));
		return;
	}
	else if (pathType == PATH_DIRECTORY)
	{
		std::string directoryURL = request.getURL();
		char lastChar = *(directoryURL.rbegin());
		if (lastChar != '/')
		{
			// URL normalization or path canonicalization
			// redirect; the directory exists, but the client did not request
			// it properly, there was a missing '/' at the end
			response.setStatus(HTTP_Status::MOVED_PERMANENTLY);
			response.getHeaders()[HTTP_FieldName::LOCATION] = directoryURL + "/";
			response.setContent("");
			// std::cout << "URL normalization: " << directoryURL << " -> "
			// 	<< (directoryURL + "/") << "\n";
			// std::cout << "Seding response:" << std::endl;
			return;
		}

		if (location.index != "")
		{
			std::string indexOnServer = pathOnServer + location.index;
			PathType indexType = getPathType(indexOnServer);

			if (indexType == PATH_FILE)
			{
				try
				{
					response.setContent(Utils::getFileContent(indexOnServer));
					response.setStatus(HTTP_Status::OK);
				}
				catch (std::exception &e)
				{
					setResponse(response, HTTP_Status::FORBIDDEN, sc);
				}
				return;
			}
		}

		if (!location.autoindex)
		{
			setResponse(response, HTTP_Status::FORBIDDEN, sc);
			return;
		}

		// go ahead and list directory content
		std::string htmlDirectories =
			DirectoriesToHTML::generate(
				Utils::getDirectoryList(pathOnServer), request.getURL());

		response.setStatus(HTTP_Status::OK);
		response.setContent(htmlDirectories);
		return;
	}

	response.setStatus(HTTP_Status::OK);
	response.setContent("This should NEVER happen!?");
	return;
}

void HTTP_ResponseBuilder::build_response_for_POST(
	HTTP_Response &response,
	HTTP_Request &request,
	const ServerConfig &sc)
{
	(void)request;
	setResponse(response, HTTP_Status::NOT_IMPLEMENTED, sc);
}

void HTTP_ResponseBuilder::build_response_for_DELETE(
	HTTP_Response &response,
	HTTP_Request &request,
	const ServerConfig &sc)
{
	(void)request;
	setResponse(response, HTTP_Status::NOT_IMPLEMENTED, sc);
}

const LocationConfig &HTTP_ResponseBuilder::locationGetBestMatch(
	const ServerConfig &serverConfig,
	const HTTP_Request &hRequest)
{
	std::vector<LocationConfig>::const_iterator selectedLocation_it = serverConfig.locations.end();
	std::string reqURL = hRequest.getURL();

	// now, go through the locations and match the best one
	std::vector<LocationConfig>::const_iterator loc_it = serverConfig.locations.begin();
	for (; loc_it != serverConfig.locations.end(); ++loc_it)
	{
		std::vector<LocationConfig>::const_iterator location_it = loc_it;
		std::string locPath = location_it->path;

		if (reqURL.find(locPath) == 0)
		{
			std::string overlap = locPath;
			if (*overlap.rbegin() == '/' || overlap.length() == reqURL.length())
			{
				if (selectedLocation_it == serverConfig.locations.end())
					selectedLocation_it = loc_it;
				else if (location_it->path.length() > selectedLocation_it->path.length())
					selectedLocation_it = loc_it;
			}
		}
	}

	if (selectedLocation_it == serverConfig.locations.end())
		throw std::runtime_error("No suitable server/location found for " + reqURL);

	std::cout << "Best location match is: " << selectedLocation_it->path << std::endl;
	return (*selectedLocation_it);
}

// According to the PDF:
// if location URL /kapouet is mapped to root dir /tmp/www
// then the request URL /kapouet/pouic/toto/pouet
// will have to be searched in /tmp/www/pouic/toto/pouet
//
// the above in nginx is an alias but in webserv has to be
// the default way.
std::string HTTP_ResponseBuilder::translateUriToPath(
	const LocationConfig &location, const HTTP_Request &hRequest, bool asAlias)
{
	const std::string &basePath = location.root;
	std::string result = hRequest.getURL();

	if (asAlias)
	{
		if (!replace(result, location.path, ""))
		{
			std::string error = "Error: HTTP_ResponseBuilder::translateUriToPath invalid request url \"" + hRequest.getURL() + "\"\n";
			std::cout << error;
			throw(std::runtime_error(error));
		}
	}

	char basePathLastChar = *(basePath.rbegin());
	if (basePathLastChar == '/' && result[0] == '/')
	{
		result.erase(0, 1);
		result = basePath + result;
	}
	else if (basePathLastChar == '/' || result[0] == '/')
		result = basePath + result;
	else
		result = basePath + '/' + result;

	return (result);
}
