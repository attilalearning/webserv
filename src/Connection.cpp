/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 12:49:10 by mosokina          #+#    #+#             */
/*   Updated: 2026/03/25 02:15:47 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(int fd, Server *server): _state(READING_HEADERS),
												_connectFd(fd),
												_server(server),
												_expectedBodySize(0),
												_isChunked(false) 
{
	_lastActive = std::time(NULL);
}

Connection::~Connection()
{
	if (_connectFd != -1)
	{
		std::cout << "Closing fd " << _connectFd << std::endl;
		close(_connectFd);
		_connectFd = -1;
	}

	//kill() any child processes it started.- for CGI part
}

void Connection::resetForNextRequest()
{
	_request.reset();
	_state = READING_HEADERS;
	_chunkedAccumulator.clear();
	_isChunked = false;
	_expectedBodySize = 0;
	// Log for debugging:
	if (!_rawRequest.empty()) {
		std::cout << "[WebServ] Pipelined data remaining in buffer: " 
				  << _rawRequest.size() << " bytes." << std::endl;
	}
}

void Connection::resetTimeout()
{
	_lastActive = std::time(NULL);
}

bool Connection::isTimedOut(time_t now, int limit) const
{
	return (std::difftime(now, _lastActive) >= limit);
}
HTTP::Request &Connection::getRequest()
{
	return (_request);
}

HTTP::Response &Connection::getResponse()
{
	return (_response);
}

Server *Connection::getServer()
{
	return (_server);
}

int Connection::getState() const
{
	return _state;
}

void Connection::handleRead(const char *buffer, ssize_t bytesRead)
{
	// Safety check: Don't let _rawRequest grow indefinitely while looking for headers
	if (buffer != NULL && bytesRead > 0)
	{
		if (_state == READING_HEADERS && (_rawRequest.size() + bytesRead > MAX_HEADER_SIZE))
		{
			_state = ERROR;
			_request.setParseStatus(HTTP_Request::REQUEST_HEADER_FIELDS_TOO_LARGE);
			return;
		}
		_rawRequest.append(buffer, bytesRead);        
	}
	if (_state == READING_HEADERS)
	{
		size_t pos = _rawRequest.find(DBL_CRLF);
		if (pos != std::string::npos) {
			std::string headerString = _rawRequest.substr(0, pos + 4);
			
			if (_request.parseHeaders(headerString.c_str(), headerString.size()) == FAILURE) {
				_state = ERROR;
				return;
			}

			// Remove headers from buffer; only the start of the body remains
			_rawRequest.erase(0, pos + 4);
			_setupBodyReading();
		}
	}

	if (_state == READING_BODY) {
		if (_isChunked)
			_handleChunkedBody();
		else
			_handleStandardBody();
	}
}

bool Connection::handleWrite() //bool is finised
{
	if (_rawResponse.empty())
		return true; // //MO: check case, Nothing to send

	const char* dataPtr = _rawResponse.c_str() + _bytesSent;
	size_t remaining = _rawResponse.size() - _bytesSent;

	ssize_t sent = send(_connectFd, dataPtr, remaining, 0);

	if (sent == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return false;
		_state = ERROR; //MO: check case
		return true; 
	}
	
	_bytesSent += sent;
	this->resetTimeout(); //MO: find proper place for this line
	if (_bytesSent >= _rawResponse.size()) {
		return true;
	}
	return false;
}

bool Connection::shouldClose() const
{
	if (_state == ERROR) return true;
	// MO: HANDLE OTHER ERRORS
	if (_request.getParseStatus() == HTTP_Request::BAD_REQUEST)
		return true;

	const std::map<std::string, std::string>& headers = _request.getHeaders();
	std::string version = _request.getVersion();

	// Find the 'Connection' header
	std::string connHeader = "";
	std::map<std::string, std::string>::const_iterator it = headers.find("Connection");
		if (it != headers.end()) {
			connHeader = it->second; // C++98 safe!
			for (size_t i = 0; i < connHeader.length(); ++i)
				connHeader[i] = std::tolower(connHeader[i]);
		}
	// 1. HTTP/1.1 Logic: Persistent by default
	if (version == HTTP_Version::v1_1) {
		return (connHeader == "close");
	}

	// 2. HTTP/1.0 Logic: Close by default
	if (version == HTTP_Version::v1_0) {
		return (connHeader != "keep-alive");
	}

	return true; // Default to safety
}


void Connection::prepareResponse()
{
	HTTP::Response hResponse = HTTP::ResponseBuilder::build(_server->getConfig(), _request);
	_rawResponse = hResponse.toString(); //MO: rename to sterilise()?
	_bytesSent = 0;
}

void Connection::forceTimeoutError()
{
	_state = ERROR;
	_request.setParseStatus(HTTP_Request::REQUEST_TIMEOUT);
}

void Connection::_setupBodyReading()
{
	const std::map<std::string, std::string>& h = _request.getHeaders();
	std::map<std::string, std::string>::const_iterator itTE = h.find("Transfer-Encoding");
	std::map<std::string, std::string>::const_iterator itCL = h.find("Content-Length");
	
	// 1. RFC 9112: Priority & Smuggling Check
	// If both are present, we must reject it.
	if (itTE != h.end() && itCL != h.end())
	{
		std::cout << "[Security] Smuggling attempt: both CL and TE present." << std::endl;
		_request.setParseStatus(HTTP_Request::BAD_REQUEST);
		_state = ERROR;
		return;
	}

	// 2. Handle Transfer-Encoding
	if (itTE != h.end())
	{
		// We ONLY support 'chunked'.
		if (itTE->second == "chunked")
		{
			_isChunked = true;
			_state = READING_BODY;
		}
		else
		{
			std::cout << "[WebServ] Unsupported TE: " << itTE->second << std::endl;
			_request.setParseStatus(HTTP_Request::BAD_REQUEST); //or 501 (Not Implemented)
			_state = ERROR;
		}
		return;
	}
	// 3. Handle Content-Length
	if (itCL != h.end()) 
	{
		_expectedBodySize = std::strtoul(itCL->second.c_str(), NULL, 10);
		size_t maxBodySize = _server->getConfig().max_body_size;
		if (_expectedBodySize > maxBodySize) {
			std::cout << "[WebServ] Payload too large (Content-Length): " << _expectedBodySize << std::endl;
			_request.setParseStatus(HTTP_Request::CONTENT_TOO_LARGE);
			_state = ERROR;
			return;
		}

		_state = (_expectedBodySize > 0) ? READING_BODY : REQUEST_READY;
	}
	else 
		_state = REQUEST_READY; // No TE and no CL means no body.
}

bool Connection::hasBufferedData() const
{
	return !_rawRequest.empty();
}

std::string Connection::getRawRequest() const
{
	return _rawRequest;
}

std::string Connection::getRawResponse() const
{
	return _rawResponse;
}

void Connection::_handleStandardBody()
{
	if (_rawRequest.size() >= _expectedBodySize) {
		_request.setBody(_rawRequest.c_str(), _expectedBodySize);
		_rawRequest.erase(0, _expectedBodySize);
		_state = REQUEST_READY;
	}
}

void Connection::_handleChunkedBody() {
	while (true)
	{
		size_t pos = _rawRequest.find(CRLF);
		if (pos == std::string::npos) return; // Wait for more data

		// 1. Extract hex size, ignoring any chunk extensions (e.g., "5;ext=val")
		std::string hexLine = _rawRequest.substr(0, pos);
		size_t semiPos = hexLine.find(';');
		std::string hexStr = (semiPos != std::string::npos) ? hexLine.substr(0, semiPos) : hexLine;
		
		// Validation check
		if (!_isValidHex(hexStr)) {
			std::cout << "[WebServ] Invalid chunk size format" << std::endl;
			_request.setParseStatus(HTTP_Request::BAD_REQUEST);
			_state = ERROR;
			return;
		}

		size_t chunkSize = std::strtoul(hexStr.c_str(), NULL, 16);
		
		// 2. Terminal Chunk Handling (0\r\n\r\n)
		if (chunkSize == 0) {
			// Search for the end of the entire chunked message (handles trailing headers)
			size_t endPos = _rawRequest.find(DBL_CRLF, pos);
			if (endPos == std::string::npos) return; // Wait for the final CRLF

			_request.setBody(_chunkedAccumulator.c_str(), _chunkedAccumulator.size());
			_state = REQUEST_READY;
			_rawRequest.erase(0, endPos + 4); 
			return;
		}

		// 3. Overflow and Limit Check for Payload Size
		size_t maxBodySize = _server->getConfig().max_body_size;
		if (chunkSize > maxBodySize || _chunkedAccumulator.size() + chunkSize > maxBodySize)
		{
			std::cout << "[WebServ] Payload too large (Chunked stream exceeded limit)" << std::endl;
			_request.setParseStatus(HTTP_Request::CONTENT_TOO_LARGE);
			_state = ERROR;
			return;
		}

		// 4. Safe Bounds Check (Prevents integer overflow)
		if (_rawRequest.size() - (pos + 2) < chunkSize + 2) return;

		// 5. _handleChunkedBody
		if (_rawRequest.substr(pos + 2 + chunkSize, 2) != "\r\n") {
			std::cout << "[WebServ] Malformed chunk: missing trailing CRLF" << std::endl;
			_request.setParseStatus(HTTP_Request::BAD_REQUEST);
			_state = ERROR;
			return;
		}

		// 6. Append data and erase processed chunk
		_chunkedAccumulator.append(_rawRequest.substr(pos + 2, chunkSize));
		_rawRequest.erase(0, pos + 2 + chunkSize + 2); // Move to next chunk
	}
}

bool Connection::_isValidHex(const std::string& s) const
{
	if (s.empty()) return false;
	for (size_t i = 0; i < s.size(); ++i) {
		if (!std::isxdigit(static_cast<unsigned char>(s[i]))) {
			return false;
		}
	}
	return true;
}