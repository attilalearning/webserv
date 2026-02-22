/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Response.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 16:34:38 by aistok            #+#    #+#             */
/*   Updated: 2026/02/22 09:27:47 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP_Response.hpp"

HTTP_Response::HTTP_Response()
	: bodyLen(0), body(""), status(HTTP_Status::UNSET), version(HTTP_Version::v1_1)
{
	// done
}

HTTP_Response::HTTP_Response(const HTTP_StatusPair &status)
	: bodyLen(0), body(""), status(status), version(HTTP_Version::v1_1)
{
	// done
}

// will set status message too add headers
void HTTP_Response::setStatus(const HTTP_StatusPair &status)
{
	this->status = status;
}

// figure out, what functions are needed to be able to add
// a body into the response, encode it if needed and
// add the appropriate headers for it
// (ex content length, transfer encoding, range? mime type?)

std::ostream &operator<<(std::ostream &os, HTTP_Response &hResp)
{
	os << hResp.status.code << " " << hResp.status.text << " " << hResp.version;

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
