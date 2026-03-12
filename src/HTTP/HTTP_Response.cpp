/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Response.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 16:34:38 by aistok            #+#    #+#             */
/*   Updated: 2026/03/12 16:38:02 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP/HTTP_Response.hpp"
#include <sstream>

HTTP_Response::HTTP_Response()
	: _status(HTTP_Status::UNSET), _version(HTTP_Version::v1_1), _bodyLen(0), _body("")
{
	// done
}

HTTP_Response::HTTP_Response(const HTTP_StatusPair &status)
	: _status(status), _version(HTTP_Version::v1_1), _bodyLen(0), _body("")
{
	// done
}

HTTP_Response::HTTP_Response(const HTTP_StatusPair &status, std::string textContent)
	: _status(status), _version(HTTP_Version::v1_1), _bodyLen(0)/*, _body("")*/
{
	setContent(textContent);
}

std::map<std::string, std::string> HTTP_Response::getHeaders()
{
	return (_headers);
}

// will set status message too add headers
void HTTP_Response::setStatus(const HTTP_StatusPair &status)
{
	_status = status;
}

std::string HTTP_Response::toString()
{
	std::ostringstream oss;
	oss << *this;
	return (oss.str());
}

void HTTP_Response::setContent(std::string text)
{
	_body = text;
	_headers[HTTP_FieldName::CONTENT_LENGTH] = ::toString(text.length());
}

size_t HTTP_Response::getBodyLen() const // TO-DO: temporary only, to compile the project
{
	return (_bodyLen);
}

// figure out, what functions are needed to be able to add
// a body into the response, encode it if needed and
// add the appropriate headers for it
// (ex content length, transfer encoding, range? mime type?)

std::ostream &operator<<(std::ostream &os, const HTTP_Response &hResp)
{
	os << hResp._version << " " << hResp._status.code << " " << hResp._status.text << CRLF;

	std::map<std::string, std::string>::const_iterator it;
	for (it = hResp._headers.begin(); it != hResp._headers.end(); ++it)
	{
		std::string fieldName = it->first;
		std::string value = it->second;

		os << fieldName << ": " << value << CRLF;
	}

	os << CRLF;

	if (hResp._body.size() > 0)
		os.write(hResp._body.c_str(), hResp._body.size());

	return (os);
}
