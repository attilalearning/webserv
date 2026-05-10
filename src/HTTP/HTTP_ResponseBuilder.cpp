/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_ResponseBuilder.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:48:39 by aistok            #+#    #+#             */
/*   Updated: 2026/05/10 23:19:48 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP/HTTP_ResponseBuilder.hpp"

HTTP_ResponseBuilder::HTTP_ResponseBuilder() {}

HTTP_ResponseBuilder::HTTP_ResponseBuilder(const ServerConfig &sc)
{
	_serverConfig = sc;
}

HTTP_ResponseBuilder::~HTTP_ResponseBuilder()
{
}

HTTP_ResponseBuilder::HTTP_ResponseBuilder(const HTTP_ResponseBuilder &other)
{
	if (this != &other)
	{
		_serverConfig = other._serverConfig;
		_location = other._location;
		_pathOnServer = other._pathOnServer;
		_pathType = other._pathType;
	}
}

HTTP_ResponseBuilder &HTTP_ResponseBuilder::operator=(const HTTP_ResponseBuilder &other)
{
	if (this != &other)
	{
		_serverConfig = other._serverConfig;
		_location = other._location;
		_pathOnServer = other._pathOnServer;
		_pathType = other._pathType;
	}
	return (*this);
}

void HTTP_ResponseBuilder::build(HTTP_Response &response, HTTP_Request &request)
{
	int parseStatus = request.getParseStatus();

	if (parseStatus == HTTP_Request::INCOMPLETE)
		return;

	const std::string method = request.getMethod();
	if (method == HTTP_Method::HEAD)
		response.setHeadersOnly(true);

	if (parseStatus >= 400)
	{
		setResponse(response, HTTP_Status::fromCode(parseStatus));
		return;
	}

	try
	{
		_location = locationGetBestMatch(request);
	}
	catch (std::exception &e)
	{
		std::cout << "[DEBUG] HTTP_ResponseBuilder::build - location"
				  << std::endl
				  << e.what() << std::endl;

		setResponse(response, HTTP_Status::NOT_FOUND);
		return;
	}

	if (_location.redirect_code > 0)
	{
		setResponseRedirect(response, _location.redirect_code, _location.redirect_url);
		return;
	}

	if (!locationHasMethod(method))
	{
		setResponse(response, HTTP_Status::FORBIDDEN);
		return;
	}
	try
	{
		_pathOnServer = translateUriToPath(request, false);
		std::cout << "[DEBUG] pathOnServer: " << _pathOnServer << "\n";
	}
	catch (std::exception &e)
	{
		std::cout << "[DEBUG] HTTP_ResponseBuilder::translateUriToPath : " << e.what() << "\n";
		setResponse(response, HTTP_Status::BAD_REQUEST);
		return;
	}

	_pathType = getPathType(_pathOnServer);

	// 3. MO: NEW CGI Logic Integration
	if (_pathType == PATH_FILE && (method == "GET" || method == "POST")) {
// --- MOVE DEBUG HERE (Outside the if block) ---
        std::string ext = Utils::getExtension(_pathOnServer);
        std::cout << "[DEBUG] Checking CGI for Path: " << _pathOnServer << std::endl;
        std::cout << "[DEBUG] Ext extracted: [" << ext << "]" << std::endl;
        std::cout << "[DEBUG] Is ext in map? " << (CGI::forCGIResponse(_pathOnServer, _location.cgi_extensions) ? "YES" : "NO") << std::endl;

		if (CGI::forCGIResponse(_pathOnServer, _location.cgi_extensions)) {
			
			// We get the executable path (e.g., /usr/bin/python3)
			std::string cgi_path = CGI::getCGIPath(_pathOnServer, _location.cgi_extensions);
			
			// Flag the response as CGI
			response.setCGIGenerated(true);
			
			// NOTE: add these two setters to  HTTP_Response class!
			response.setCgiPath(cgi_path);
			response.setScriptPath(_pathOnServer);
		}
	}
	if (_pathType == PATH_NONE && !response.isCGIGenerated())
	{
		setResponse(response, HTTP_Status::NOT_FOUND);
		return;
	}

	if (response.isCGIGenerated())
		return;
	
	else if (method == HTTP_Method::GET || method == HTTP_Method::HEAD)
		build_response_for_GET_or_HEAD(response, request);

	else if (method == HTTP_Method::POST)
		build_response_for_POST(response, request);

	else if (method == HTTP_Method::DELETE)
		build_response_for_DELETE(response, request);

	else
		setResponse(response, HTTP_Status::NOT_IMPLEMENTED);
}

void HTTP_ResponseBuilder::setResponse(HTTP_Response &response, const HTTP_StatusPair &status)
{
	response.setStatus(status);
	if (!response.isHeadersOnly())
	{
		response.setContent(ErrorPages::getContent(_serverConfig, status));
	}
}

void HTTP_ResponseBuilder::setResponseRedirect(HTTP_Response &response, const int statusCode, const std::string &url)
{
	response.setStatus(HTTP_Status::fromCode(statusCode));
	response.getHeaders()[HTTP_FieldName::LOCATION] = url;
	response.setContent("");
}

// If the location has the GET in allowed_methods, then this
// function will return true for the HEAD method too!
bool HTTP_ResponseBuilder::locationHasMethod(std::string method)
{
	std::vector<std::string>::const_iterator method_it = _location.allowed_methods.begin();
	for (; method_it != _location.allowed_methods.end(); ++method_it)
	{
		if (*method_it == method || (method == HTTP_Method::HEAD && *method_it == HTTP_Method::GET))
			return (true);
	}
	return (false);
}

void HTTP_ResponseBuilder::build_response_for_GET_or_HEAD(HTTP_Response &response, HTTP_Request &request)
{
	if (_pathType == PATH_FILE)
	{
		if (!Utils::isReadable(_pathOnServer))
			setResponse(response, HTTP_Status::FORBIDDEN);
		else
		{
			response.setStatus(HTTP_Status::OK);
			if (!response.isHeadersOnly())
				response.setContent(Utils::getFileContent(_pathOnServer));
		}
		return;
	}
	else if (_pathType == PATH_DIRECTORY)
	{
		std::string directoryURL = request.getURLWithoutParams();
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
			std::cout << "[DEBUG] +++ URL normalization" << std::endl;
			return;
		}

		std::string theIndexFile;

		if (!_location.index.empty())
			theIndexFile = _location.index;
		else if (!_serverConfig.index.empty())
			theIndexFile = _serverConfig.index;

		if (!theIndexFile.empty())
		{
			std::string indexOnServer = _pathOnServer + theIndexFile;
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
					std::cout << "[DEBUG] Unable to open file " << indexOnServer << std::endl;
					setResponse(response, HTTP_Status::FORBIDDEN);
				}
				return;
			}
		}

		if (!_location.autoindex
			|| !Utils::isReadable(_pathOnServer))
		{
			setResponse(response, HTTP_Status::FORBIDDEN);
			return;
		}

		// dir exists and is readable
		if (response.isHeadersOnly())
			setResponse(response, HTTP_Status::OK);
		else
		{
			// if not HEAD request, list directory content
			response.setStatus(HTTP_Status::OK);

			std::string htmlDirectories =
				DirectoriesToHTML::generate(
					Utils::getDirectoryList(_pathOnServer),
					request.getURLWithoutParams(),
					locationHasMethod(HTTP_Method::DELETE)
				);

			response.setContent(htmlDirectories);
		}
		return;
	}

	// the below should never happen!
	response.setStatus(HTTP_Status::OK);
	if (!response.isHeadersOnly())
		response.setContent("This should NEVER happen!?");
	return;
}

void HTTP_ResponseBuilder::build_response_for_POST(
	HTTP_Response &response,
	HTTP_Request &request)
{
	(void)request;
	// setResponse(response, HTTP_Status::NOT_IMPLEMENTED, sc);
	// MO comment: this block for TESTS
	response.setStatus(HTTP_Status::OK);
	response.setContent("POST test");
}

void HTTP_ResponseBuilder::build_response_for_DELETE(
	HTTP_Response &response,
	HTTP_Request &request)
{
	(void)request;
	if (std::remove(_pathOnServer.c_str()) == 0)
		setResponse(response, HTTP_Status::OK);

	else
		setResponse(response, HTTP_Status::INTERNAL_SERVER_ERROR);
}

const LocationConfig &HTTP_ResponseBuilder::locationGetBestMatch(const HTTP_Request &hRequest)
{
    std::vector<LocationConfig>::const_iterator selectedLocation_it = _serverConfig.locations.end();
    
    // 1. Strip the query string so we only match against the actual path
    std::string reqURL = hRequest.getURLWithoutParams(); 

    std::vector<LocationConfig>::const_iterator loc_it = _serverConfig.locations.begin();
    for (; loc_it != _serverConfig.locations.end(); ++loc_it)
    {
        std::string locPath = loc_it->path;

        // 2. Check if the URL starts with the location path
        if (reqURL.find(locPath) == 0)
        {
            bool isValidMatch = false;
            
            // 3. Prevent partial word matches (e.g., location "/app" matching URL "/apple")
            if (locPath[locPath.length() - 1] == '/') {
                isValidMatch = true; // Location ends in '/', so it's a directory match
            } 
            else if (reqURL.length() == locPath.length()) {
                isValidMatch = true; // Exact match
            } 
            else if (reqURL[locPath.length()] == '/') {
                isValidMatch = true; // Next character is '/', so it's a clean directory boundary
            }

            // 4. If it's a valid match, check if it's the longest one we've seen
            if (isValidMatch)
            {
                if (selectedLocation_it == _serverConfig.locations.end() || 
                    locPath.length() > selectedLocation_it->path.length())
                {
                    selectedLocation_it = loc_it;
                }
            }
        }
    }

    if (selectedLocation_it == _serverConfig.locations.end())
        throw std::runtime_error("No suitable server/location found for " + reqURL);

    std::cout << "[DEBUG] Best location match is: " << selectedLocation_it->path << std::endl;
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
	bool asAlias)
{
	std::string basePath;

	if (!_location.root.empty())
		basePath = _location.root;
	else if (!_serverConfig.root.empty())
		basePath = _serverConfig.root;
	else
		throw(std::runtime_error("location.root and serverConfig.root are both empty!"));
		// TO-DO: the above should "generate" a 500 Internal Server Error

	std::string result = request.getURLWithoutParams();

	if (asAlias)
	{
		if (!replace(result, _location.path, ""))
		{
			// TO-DO: the below should "generate" a 400 Bad Request error
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

void HTTP_ResponseBuilder::reset()
{
	_location.allowed_methods.clear();
	_location.autoindex = false;
	_location.cgi_extensions.clear();
	_location.client_max_body_size = 0;
	_location.index = "";
	_location.path = "";
	_location.redirect_code = 0;
	_location.redirect_url = "";
	_location.root = "";
	_location.upload_path = "";

	_pathOnServer = "";
	_pathType = PATH_NONE;
}
