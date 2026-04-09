/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_ResponseBuilder.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:48:39 by aistok            #+#    #+#             */
/*   Updated: 2026/04/09 22:03:02 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP/HTTP_ResponseBuilder.hpp"

void HTTP_ResponseBuilder::build(
	HTTP_Response &response,
	HTTP_Request &request,
	const ServerConfig &serverConfig)
{
	int parseStatus = request.getParseStatus();

	if (parseStatus == HTTP_Request::INCOMPLETE)
	{
		return; // should we throw exception ?
	}

	const std::string method = request.getMethod();
	if (method == HTTP_Method::HEAD)
		response.setHeadersOnly(true);

	// this will handle all errors including 400, 408, 413, 431, etc
	if (parseStatus >= 400)
	{
		setResponse(response, HTTP_Status::fromCode(parseStatus), serverConfig);
		return;
	}

	if (method == HTTP_Method::GET
		|| method == HTTP_Method::HEAD)
		build_response_for_GET_or_HEAD(response, request, serverConfig);

	else if (method == HTTP_Method::POST)
		build_response_for_POST(response, request, serverConfig);

	else if (method == HTTP_Method::DELETE)
		build_response_for_DELETE(response, request, serverConfig);

	else
		setResponse(response, HTTP_Status::NOT_IMPLEMENTED, serverConfig);
}

void HTTP_ResponseBuilder::setResponse(
	HTTP_Response &response,
	const HTTP_StatusPair &status,
	const ServerConfig &sc)
{
	response.setStatus(status);
	if (!response.isHeadersOnly())
	{
		response.setContent(ErrorPages::getContent(sc, status));
	}
}

void HTTP_ResponseBuilder::setResponseRedirect(
	HTTP_Response &response,
	const int statusCode,
	const std::string &url)
{
	response.setStatus(HTTP_Status::fromCode(statusCode));
	response.getHeaders()[HTTP_FieldName::LOCATION] = url;
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

void HTTP_ResponseBuilder::build_response_for_GET_or_HEAD(
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

	if (location.redirect_code > 0)
	{
		setResponseRedirect(
			response,
			location.redirect_code,
			location.redirect_url);
		return;
	}

	// if a GET request is allowed for a location,
	// then a HEAD request is also allowed
	if (!locationHasMethod(location, HTTP_Method::GET))
	{
		setResponse(response, HTTP_Status::FORBIDDEN, sc);
		return;
	}

	std::string pathOnServer;
	try
	{
		pathOnServer = translateUriToPath(request, location, sc, true);
		std::cout << "pathOnServer: " << pathOnServer << "\n";
	}
	catch (std::exception &e)
	{
		std::cout << "HTTP_ResponseBuilder::translateUriToPath : " << e.what() << "\n";
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
		if (!response.isHeadersOnly())
			response.setContent(Utils::getFileContent(pathOnServer));
		return;
	}
	else if (pathType == PATH_DIRECTORY)
	{
		std::string directoryURL = request.getURL();
		char lastChar = *(directoryURL.rbegin());
		if (lastChar != '/')
		{
			// URL normalization or path canonicalization redirect
			// the directory exists, but the client did not request
			// it properly, there was a missing '/' at the end
			setResponseRedirect(
				response,
				HTTP_Status::MOVED_PERMANENTLY.code,
				directoryURL + "/");
			return;
		}

		std::string theIndexFile = "";
		if (!location.index.empty())
			theIndexFile = location.index;
		else if (!sc.index.empty())
			theIndexFile = sc.index;

		if (theIndexFile != "")
		{
			std::string indexOnServer = pathOnServer + theIndexFile;
			PathType indexType = getPathType(indexOnServer);

			if (indexType == PATH_FILE)
			{
				try
				{
					if (!response.isHeadersOnly())
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

		response.setStatus(HTTP_Status::OK);
		if (!response.isHeadersOnly())
		{
			// go ahead and list directory content
			std::string htmlDirectories =
				DirectoriesToHTML::generate(
					Utils::getDirectoryList(pathOnServer), request.getURL());

			response.setContent(htmlDirectories);
		}
		return;
	}

	response.setStatus(HTTP_Status::OK);
	if (!response.isHeadersOnly())
		response.setContent("This should NEVER happen!?");
	return;
}

void HTTP_ResponseBuilder::build_response_for_POST(
	HTTP_Response &response,
	HTTP_Request &request,
	const ServerConfig &sc)
{
	(void)request;
	// setResponse(response, HTTP_Status::NOT_IMPLEMENTED, sc);
	//MO comment: this block for TESTS
	(void)sc;
	response.setStatus(HTTP_Status::OK);
	response.setContent("POST test");
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
	const HTTP_Request &request,
	const LocationConfig &location,
	const ServerConfig &sc,
	bool asAlias)
{
	std::string basePath;

	if (!location.root.empty())
		basePath = location.root;
	else if (!sc.root.empty())
		basePath = sc.root;
	else
		throw(std::runtime_error("location.root and serverConfig.root are both empty!"));

	std::string result = request.getURL();

	if (asAlias)
	{
		if (!replace(result, location.path, ""))
		{
			std::string error = "Error: HTTP_ResponseBuilder::translateUriToPath invalid request url \"" + request.getURL() + "\"\n";
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
