/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Request.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 16:34:38 by aistok            #+#    #+#             */
/*   Updated: 2026/03/10 16:36:18 by aistok           ###   ########.fr       */
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

class HTTP_Request
{
public:
	HTTP_Request();
	HTTP_Request(const char *raw, size_t len);

	// ~HTTP_Request();

	enum ParseStatus
	{
		BAD_REQUEST = -400,
		INCOMPLETE = 0,
		COMPLETE = 1
	};

	int parse(const char *raw, size_t len);

	int getParseStatus() const;
	const std::string &getMethod() const;
	const std::string &getURL() const;

	bool ready();
	void reset();

protected:
	// ...

private:
	// Rule of three
	// HTTP_Request(const HTTP_Request &other);			// TO-DO:
	// HTTP_Request &operator=(const HTTP_Request &other);	// TO-DO:

	std::string _method;
	std::string _url;
	std::string _version;
	bool _requestLine_completed;

	std::map<std::string, std::string> _headers;
	bool _headers_completed;
	int _headersRequiredCount;

	size_t _bodyLen;
	std::string _body;
	bool _body_completed;

	ParseStatus _parseStatus;

	int _parseRequestLine(std::string line);
	int _parseMethod(std::string method);
	int _parseURL(std::string url);
	int _parseVersion(std::string version);
	int _URLIsValid(std::string url);

	int _parseHeaderLine(std::string line);
	int _countHeaderIfRequired(std::string fieldName);
	int _fieldNameIsValid(std::string fieldName);
	int _headerValueIsValid(std::string value);
	int _fieldNameAlreadyProcessed(std::string eKey);
	int _fieldNameIsSecurityRisk(std::string eKey);

	// only if need access to private or protected elements
	friend class HTTP;
	friend std::ostream &operator<<(std::ostream &os, const HTTP_Request &hr);
};

std::ostream &operator<<(std::ostream &os, const HTTP_Request &hr);

#endif // HTTP_REQUEST_HPP
