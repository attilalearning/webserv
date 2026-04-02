/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_ResponseBuilder.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:48:39 by aistok            #+#    #+#             */
/*   Updated: 2026/04/02 06:39:30 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP/HTTP_ResponseBuilder.hpp"

std::string HTTP_ResponseBuilder::serverBasePath = std::string("./");

HTTP_Response HTTP_ResponseBuilder::build(const ServerConfig &sc, HTTP_Request &hReq)
{
	(void)sc;
	if (hReq.getParseStatus() == HTTP_Request::BAD_REQUEST)
		return (HTTP_Response(HTTP_Status::BAD_REQUEST)); //(build(BAD_REQUEST, sc));

	if (hReq.getParseStatus() == HTTP_Request::INCOMPLETE)
	{
		// throw exception ?
	}

	if (hReq.getMethod() == HTTP_Method::GET)
	{
		return (build_response_for_GET(sc, hReq));
	}
	else if (hReq.getMethod() == HTTP_Method::POST)
	{
		return (HTTP_Response()); //(build_response_for_POST(sc, hReq));
	}
	else if (hReq.getMethod() == HTTP_Method::DELETE)
	{
		return (HTTP_Response()); //(build_response_for_DELETE(sc, hReq));
	}
	return (HTTP_Response(HTTP_Status::FORBIDDEN));
}

HTTP_Response HTTP_ResponseBuilder::build_response_for_GET(
	const ServerConfig &serverConfig,
	HTTP_Request &hRequest)
{
	HTTP_Response hResponse;
	LocationConfig location;

	try
	{
		location = locationGetBestMatch(serverConfig, hRequest);
	}
	catch (std::exception &e)
	{
		// need to somehow load a default error page from server
		// or from serverConfig if there is any for the server or for the location
		std::cout << "HTTP_ResponseBuilder::build_response_for_GET - location"
				  << std::endl
				  << e.what() << std::endl;
		return (HTTP_Response(HTTP_Status::NOT_FOUND,
							  ErrorPages::generate(HTTP_Status::NOT_FOUND)));
	}

	// TO-DO ???
	// need to consider the redirects from the config file, if there are any
	// like:
	// location / {
	//      [method = GET]
	// 		return 301 [POST] /some/url/here
	// }

	// SUGGESTION: Location should transform into a proper class, having it's own
	// methods? Here, a bool .hasMethod(std::string method) would be useful, so
	// that these iterator loops are ommitted
	// NOTE: there are iterator loops in other parts of the code, which would be
	// nice if could be incorporated into a function of a specific class?
	// NOTE2: the .find() for vector and map could easily be replaced by
	// .count() == 0 to ease the code even more?
	std::vector<std::string>::iterator method_it = location.allowed_methods.begin();
	for (; method_it != location.allowed_methods.end(); ++method_it)
	{
		if (*method_it == HTTP_Method::GET)
			break;
	}
	if (method_it == location.allowed_methods.end())
	{
		// the GET method was not found for the location
		return (HTTP_Response(HTTP_Status::FORBIDDEN,
							  ErrorPages::generate(HTTP_Status::FORBIDDEN)));
	}

	std::string pathOnServer;
	try
	{
		pathOnServer = translateUriToPath(location, hRequest, true);
		std::cout << "pathOnServer: " << pathOnServer << "\n";
	}
	catch (std::exception &e)
	{
		return (HTTP_Response(HTTP_Status::BAD_REQUEST,
							  ErrorPages::generate(HTTP_Status::BAD_REQUEST)));
	}
	PathType pathType = getPathType(pathOnServer);

	if (pathType == PATH_NONE)
		return (HTTP_Response(HTTP_Status::NOT_FOUND,
							  ErrorPages::generate(HTTP_Status::NOT_FOUND)));
	else if (pathType == PATH_FILE)
	{
		// ...
		// the file exists, open it and add content into the response body
		hResponse.setStatus(HTTP_Status::OK);
		hResponse.setContent(Utils::getFileContent(pathOnServer));
		// hResponse.setContent("File exists, and will be included in here");
		return (hResponse);
	}
	else if (pathType == PATH_DIRECTORY)
	{
		std::string directoryURL = hRequest.getURL();
		char lastChar = *(directoryURL.rbegin());
		if (lastChar != '/')
		{
			// URL normalization or path canonicalization
			// redirect; the directory exists, but the client did not request
			// it properly, there was a missing '/' at the end
			hResponse.setStatus(HTTP_Status::MOVED_PERMANENTLY);
			hResponse.getHeaders()[HTTP_FieldName::LOCATION] = directoryURL + "/";
			hResponse.getHeaders()[HTTP_FieldName::CONTENT_LENGTH] = "0";
			// std::cout << "URL normalization: " << directoryURL << " -> "
			// 	<< (directoryURL + "/") << "\n";
			// std::cout << "Seding response:" << std::endl;
			return (hResponse);
		}

		if (location.index != "")
		{
			std::string indexOnServer = pathOnServer + location.index;
			PathType indexType = getPathType(indexOnServer);

			if (indexType == PATH_FILE)
			{
				hResponse.setStatus(HTTP_Status::OK);
				try
				{
					hResponse.setContent(Utils::getFileContent(indexOnServer));
				}
				catch (std::exception &e)
				{
					hResponse.setContent(ErrorPages::generate(HTTP_Status::FORBIDDEN));
				}
				return (hResponse);
			}
		}

		if (!location.autoindex)
		{
			return (HTTP_Response(HTTP_Status::FORBIDDEN,
								  ErrorPages::generate(HTTP_Status::FORBIDDEN)));
			// hResponse.setStatus(HTTP_Status::FORBIDDEN);
			// hResponse.headers[HTTP_FieldName::CONTENT_LENGTH] = ::toString(0);
			// return (hResponse);
		}

		// go ahead and list directory content,
		// and add it to response body
		hResponse.setStatus(HTTP_Status::OK);
		std::string htmlDirectories = DirectoriesToHTML::generate(
			Utils::getDirectoryList(pathOnServer),
			hRequest.getURL());
		hResponse.setContent(htmlDirectories);
		return (hResponse);
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

	hResponse.setStatus(HTTP_Status::OK); // ?
	hResponse.setContent("This should NEVER happen!?");
	return (hResponse);
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
