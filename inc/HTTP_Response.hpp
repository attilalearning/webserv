/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Response.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 16:34:38 by aistok            #+#    #+#             */
/*   Updated: 2026/02/23 23:33:24 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <iostream>
#include <map>

#include "HTTP_Version.hpp"
#include "HTTP_Status.hpp"
#include "HTTP_Method.hpp"
#include "HTTP_FieldName.hpp"
#include "Utils.hpp"

// TO-DO: orthodox canonical form!
class HTTP_Response
{
public:
	std::map<std::string, std::string> headers;

	size_t bodyLen;
	std::string body;

	HTTP_Response();
	HTTP_Response(const HTTP_StatusPair &status);

	void setStatus(const HTTP_StatusPair &status); // will set status message too
												   // add headers

	std::string toString();

	// figure out, what functions are needed to be able to add
	// a body into the response, encode it if needed and
	// add the appropriate headers for it
	// (ex content length, transfer encoding, range? mime type?)

	// friend is needed for the operator<< to be able to access
	// the status and version private variables
	friend std::ostream &operator<<(std::ostream &os, HTTP_Response &hResp);

protected:
	// ...

private:
	HTTP_StatusPair _status;
	std::string _version;
};

#endif // HTTP_RESPONSE_HPP
