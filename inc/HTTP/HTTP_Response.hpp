/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Response.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 16:34:38 by aistok            #+#    #+#             */
/*   Updated: 2026/03/10 21:12:02 by aistok           ###   ########.fr       */
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
	HTTP_Response();
	HTTP_Response(const HTTP_StatusPair &status);
	HTTP_Response(const HTTP_StatusPair &status, std::string textContent);

	std::map<std::string, std::string> getHeaders();

	void setStatus(const HTTP_StatusPair &status); // will set status message too
												   // add headers

	std::string toString();
	void setContent(std::string text);

	// figure out, what functions are needed to be able to add
	// a body into the response, encode it if needed and
	// add the appropriate headers for it
	// (ex content length, transfer encoding, range? mime type?)

	// friend is needed for the operator<< to be able to access
	// the status and version private variables
	friend std::ostream &operator<<(std::ostream &os, const HTTP_Response &hResp);

protected:
	// ...

private:
	HTTP_StatusPair _status;
	std::string _version;

	std::map<std::string, std::string> _headers;

	size_t _bodyLen;
	std::string _body;
};

#endif // HTTP_RESPONSE_HPP
