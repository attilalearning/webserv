/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 12:49:10 by mosokina          #+#    #+#             */
/*   Updated: 2026/03/12 01:42:42 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(int fd, const sockaddr_in &clientAddr, Server *server)
	: _connectFd(fd), _clientAddr(clientAddr), _server(server), _rawRequest("")
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


void Connection::appendRawRequest(const char *buffer, ssize_t bytesRead)
{
    if (bytesRead <= 0) return;

	// // Safety Check: Total buffer protection
    // size_t absoluteLimit = 10 * 1024 * 1024; // 10MB absolute safety cap
    // if (_rawRequest.size() + bytesRead > absoluteLimit) {
    //     throw std::runtime_error("Request exceeds absolute safety limit");
    // }
    _rawRequest.append(buffer, bytesRead);
}

bool Connection::isHeadersComplete()
{
    // Search for the CRLF CRLF delimiter
    if (this->_rawRequest.find(CRLF) != std::string::npos)
    {
        return true;
    }
    return false;
}

void Connection::handleRead(const char *buffer, ssize_t bytesRead)
{
    // 1. Always append first
    _rawRequest.append(buffer, bytesRead);

    // 2. Decide what to do based on current state
    if (_state == READING_HEADERS) {
        _tryParseHeaders();
    }
    
    if (_state == READING_BODY) {
        _checkBodyCompletion();
    }
}

void Connection::_tryParseHeaders()
{
    size_t pos = _rawRequest.find("\r\n\r\n");
    if (pos == std::string::npos) return; // Not enough data yet

    // 1. Extract the header block (including the delimiter)
    std::string headerBlock = _rawRequest.substr(0, pos + 4);
    
    // 2. Pass it to your HTTP::Request object to parse
    _request.parseHeaders(headerBlock);

    // 3. Determine the "Body Start" point
    _headerSize = pos + 4; 

    // 4. Check if we expect a body
    if (_request.hasHeader("Content-Length")) {
        _contentLength = std::stoul(_request.getHeader("Content-Length"));
        
        if (_contentLength > _server->getConfig().max_body_size)
		{
            throw std::runtime_error("413 Payload Too Large");
        }
        
        _state = READING_BODY;
        _checkBodyCompletion(); // Check if the body was already in the buffer
    }
	else {
        _state = REQUEST_READY; // No body (like a GET request)
    }
}


// void Connection::_checkBodyCompletion() {
//     // Total bytes we need = header length + body length
//     if (_rawRequest.size() >= (_headerSize + _contentLength)) {
        
//         // Extract the body from the raw buffer
//         std::string body = _rawRequest.substr(_headerSize, _contentLength);
//         _request.setBody(body);
        
//         _state = REQUEST_READY;
        
//         // OPTIONAL: Clear _rawRequest to save memory if you're done with it
//         // _rawRequest.clear(); 
//     }
// }

void Connection::_determineNextState() {
    // 1. Check for Chunked Encoding (Priority 1)
    if (_request.getHeader("Transfer-Encoding") == "chunked") {
        _state = READING_BODY_CHUNKED;
        return;
    }

    // 2. Check for Content-Length (Priority 2)
    if (_request.hasHeader("Content-Length")) {
        _contentLength = std::stoul(_request.getHeader("Content-Length"));
        
        if (_contentLength > 0) {
            if (_contentLength > _server->getConfig().max_body_size) {
                // Return 413 Payload Too Large
                return;
            }
            _state = READING_BODY;
        } else {
            _state = REQUEST_READY; // Content-Length is 0
        }
        return;
    }

    // 3. No headers found - assume no body for standard methods
    // (POST/PUT without Content-Length usually results in a 400 or 411 error)
    _state = REQUEST_READY;
}