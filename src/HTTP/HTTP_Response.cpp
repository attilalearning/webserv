/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Response.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 16:34:38 by aistok            #+#    #+#             */
/*   Updated: 2026/02/25 05:14:29 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP/HTTP_Response.hpp"
#include <sstream>

HTTP_Response::HTTP_Response()
	: bodyLen(0), body(""), _status(HTTP_Status::UNSET), _version(HTTP_Version::v1_1)
{
	// done
}

HTTP_Response::HTTP_Response(const HTTP_StatusPair &status)
	: bodyLen(0), body(""), _status(status), _version(HTTP_Version::v1_1)
{
	// done
}

HTTP_Response::HTTP_Response(const HTTP_StatusPair &status, std::string textContent)
	: bodyLen(0), /*body(""),*/ _status(status), _version(HTTP_Version::v1_1)
{
	setContent(textContent);
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
	body = text;
	headers[HTTP_FieldName::CONTENT_LENGTH] = ::toString(text.length());
}

// figure out, what functions are needed to be able to add
// a body into the response, encode it if needed and
// add the appropriate headers for it
// (ex content length, transfer encoding, range? mime type?)

std::ostream &operator<<(std::ostream &os, HTTP_Response &hResp)
{
	os << hResp._version << " " << hResp._status.code << " " << hResp._status.text << CRLF;

	std::map<std::string, std::string>::const_iterator it;
	for (it = hResp.headers.begin(); it != hResp.headers.end(); ++it)
	{
		std::string fieldName = it->first;
		std::string value = it->second;

		os << fieldName << ": " << value << CRLF;
	}

	os << CRLF;

	if (hResp.body.size() > 0)
		os.write(hResp.body.c_str(), hResp.body.size());

	return (os);
}
