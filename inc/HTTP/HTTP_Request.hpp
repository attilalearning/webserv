/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Request.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 16:34:38 by aistok            #+#    #+#             */
/*   Updated: 2026/02/27 17:58:25 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <iostream>
#include <map>

#include "HTTP_Status.hpp"
#include "HTTP_Method.hpp"
#include "HTTP_FieldName.hpp"
#include "Utils.hpp"

// TO-DO: orthodox canonical form!
class HTTP_Request
{
public:

	std::string method;
	std::string url;
	std::string version;
	bool requestLine_completed;

	std::map<std::string, std::string> headers;
	bool headers_completed;
	int headersRequiredCount;

	size_t bodyLen;
	std::string body;
	bool body_completed;

	enum ParseStatus
	{
		BAD_REQUEST = -400,
		INCOMPLETE = 0,
		COMPLETE = 1
	};

	ParseStatus parseStatus;

	HTTP_Request();
	HTTP_Request(const char *raw, size_t len);

	int parse(const char *raw, size_t len);
	int removePortion(std::string &line, std::string portion);

	int parseRequestLine(std::string line);
	int parseMethod(std::string method);
	int parseURL(std::string url);
	int parseVersion(std::string version);
	int URLIsValid(std::string url);

	int parseHeaderLine(std::string line);
	int countHeaderIfRequired(std::string fieldName);
	int fieldNameIsValid(std::string fieldName);
	int headerValueIsValid(std::string value);
	int fieldNameAlreadyProcessed(std::string eKey);
	int fieldNameIsSecurityRisk(std::string eKey);
	int validNumber(std::string value);

	bool ready();

	/* only if need access to private or protected elements */
	// friend class HTTP;
	// friend std::ostream &operator<<(std::ostream &os, HTTPRequest &hr);

protected:

	/* ... */

private:

	/* ... */

};

std::ostream &operator<<(std::ostream &os, HTTP_Request &hr);

#endif // HTTP_REQUEST_HPP
