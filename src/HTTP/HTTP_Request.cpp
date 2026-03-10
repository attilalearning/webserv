/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Request.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 16:46:32 by aistok            #+#    #+#             */
/*   Updated: 2026/03/10 07:34:52 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream> // stringbuf
#include <istream> // istream

#include "HTTP/HTTP_Request.hpp"

HTTP_Request::HTTP_Request() : _method(""),
							   _url(""),
							   _version(""),
							   _requestLine_completed(false),
							   _headers_completed(false),
							   _headersRequiredCount(0),
							   _bodyLen(0),
							   _body_completed(false),
							   _parseStatus(INCOMPLETE)
{
	// done
}

HTTP_Request::HTTP_Request(const char *raw, size_t len) : _method(""),
														  _url(""),
														  _version(""),
														  _requestLine_completed(false),
														  _headers_completed(false),
														  _headersRequiredCount(0),
														  _bodyLen(0),
														  _body_completed(false),
														  _parseStatus(INCOMPLETE)
{
	parse(raw, len);
}

// HTTP_Request::~HTTP_Request()
// {
// std::cout << "HTTP Request destructor called!" << std::endl;
// }

// HTTP_Request::HTTP_Request(const HTTP_Request &other)
// {
// TO-DO
// }

// HTTP_Request &HTTP_Request::operator=(const HTTP_Request &other)
// {
// TO-DO
// }

/* getline removes the '\n' from each line it reads! */
int HTTP_Request::parse(const char *raw, size_t len)
{
	if (!len)
	{
		_parseStatus = HTTP_Request::INCOMPLETE;
		return (FAILURE);
	}

	std::string rawStr(raw, len);
	std::stringbuf sb(rawStr);
	std::istream is(&sb);
	std::string line;

	if (!_requestLine_completed)
	{
		if (!std::getline(is, line))
		{
			/* no data available
			 * QUESTION: is this even possible? for '\n' to be missing?
			 */
			this->_parseStatus = HTTP_Request::INCOMPLETE;
			return (FAILURE);
		}
		if (!_parseRequestLine(line))
			/* request line was malformed?
			 * parseStatus will be set!
			 */
			return (FAILURE);

		_requestLine_completed = true;
	}

	while (!_headers_completed && std::getline(is, line))
	{
		if (line == CR) /* LF was removed by getline! */
		{
			/* reached the end of headers;
			 * body may be present after this
			 */
			if (_headersRequiredCount < 2)
			{
				_parseStatus = HTTP_Request::BAD_REQUEST;
				return (FAILURE);
			}
			_headers_completed = true;
			break;
		}
		else
		{
			if (!HTTP_Request::_parseHeaderLine(line))
				/* malformed header line
				 * TO-DO: or awaiting for more data?
				 * parseStatus is now set accordingly!
				 */
				return (FAILURE);
		}
	}

	/*	by now, we parsed all the headers and reached the header-body separator
	 *	(line == CRLF) is true!
	 *	if there is a body present in the request,
	 *	copy rest of string byte by byte into the body
	 */

	/*	TO-DO: this last bit may need more work, in case there is
	 *	more data to read (for ex, in chunks?)
	 */
	if (!is.eof())
	{
		if (_headers.find(HTTP_FieldName::CONTENT_LENGTH) != _headers.end() &&
			toNumber(_headers[HTTP_FieldName::CONTENT_LENGTH], _bodyLen))
		{
			/* bodyLen is now set */
		}
		else if (_headers.find(HTTP_FieldName::TRANSFER_ENCODING) != _headers.end())
			_bodyLen = is.rdbuf()->in_avail();

		this->_body = std::string(_bodyLen, '\0');
		is.read(&this->_body[0], _bodyLen);

		if (is.gcount() != static_cast<std::streamsize>(_bodyLen))
		{
			_parseStatus = HTTP_Request::INCOMPLETE;
			return (FAILURE); /* TO-DO: should this be SUCCESS?
							   * ex: if it's "Transfer-Encoding: chunked
							   */
		}
	}

	_body_completed = true;
	_parseStatus = HTTP_Request::COMPLETE;
	return (SUCCESS);
}

int HTTP_Request::getParseStatus() const
{
	return (_parseStatus);
}

const std::string &HTTP_Request::getMethod() const
{
	return (_method);
}

const std::string &HTTP_Request::getURL() const
{
	return (_url);
}

// TO-DO: should be protected
int HTTP_Request::_parseRequestLine(std::string line)
{
	/*
	 *	getline removes '\n' (LF), so,
	 *	only check and remove '\r' (CR).
	 */
	if (!removePortion(line, CR))
	{
		_parseStatus = HTTP_Request::BAD_REQUEST;
		return (FAILURE);
	}

	int part = 1;
	std::string::size_type start = 0;
	std::string::size_type end = 0;

	std::string subString;
	while ((end = line.find(" ", start)) != std::string::npos &&
		   part < 3)
	{
		subString = line.substr(start, end - start);
		start = end + 1;
		if (part == 1)
		{
			if (!_parseMethod(subString))
			{
				_parseStatus = HTTP_Request::BAD_REQUEST;
				return (FAILURE);
			}
		}
		else if (part == 2)
		{
			if (!_parseURL(subString))
			{
				_parseStatus = HTTP_Request::BAD_REQUEST;
				return (FAILURE);
			}
		}
		part++;
	}

	if (part == 3)
	{
		subString = line.substr(start, line.size() - start);
		if (!_parseVersion(subString))
		{
			_parseStatus = HTTP_Request::BAD_REQUEST;
			return (FAILURE);
		}
	}
	else
	{
		_parseStatus = HTTP_Request::BAD_REQUEST;
		return (FAILURE);
	}

	/*	request line parsed succesfully
	 *	this line is mandatory to be present in the request,
	 *	therefore, count it in the required headers
	 */
	_headersRequiredCount++;
	return (SUCCESS);
}

int HTTP_Request::_parseMethod(std::string method)
{
	if (method == HTTP_Method::GET ||
		method == HTTP_Method::POST ||
		method == HTTP_Method::DELETE)
	{
		this->_method = method;
		return (SUCCESS);
	}
	return (FAILURE);
}

int HTTP_Request::_parseURL(std::string url)
{
	if (!_URLIsValid(url))
		return (FAILURE);
	this->_url = url;
	return (SUCCESS);
}

int HTTP_Request::_parseVersion(std::string version)
{
	if (version == "HTTP/1.1" ||
		version == "HTTP/1.0")
	{
		this->_version = version;
		return (SUCCESS);
	}
	return (FAILURE);
}

int HTTP_Request::_URLIsValid(std::string url)
{
	/* TO-DO: verify if url is valid */
	/* 	ex. contains white spaces, control characters,
	 *	etc. (RFC 9110, 9112, 3986) */
	(void)url;
	return (SUCCESS);
}

// TO-DO: should be protected
int HTTP_Request::_parseHeaderLine(std::string line)
{
	/*
	 *	getline removes '\n' (LF), so,
	 *	only check and remove '\r' (CR).
	 */
	if (!removePortion(line, CR))
	{
		_parseStatus = HTTP_Request::BAD_REQUEST;
		return (FAILURE);
	}

	std::string::size_type pos = line.find(":", 0);
	if (pos == std::string::npos)
	{
		_parseStatus = HTTP_Request::BAD_REQUEST;
		return (FAILURE);
	}

	std::string fieldName = line.substr(0, pos);
	fieldName = capitaliseFirstLetter(fieldName);
	std::string value = line.substr(pos + 1, line.size());
	if (!_fieldNameIsValid(fieldName) ||
		!_headerValueIsValid(value) ||
		_fieldNameAlreadyProcessed(fieldName) ||
		_fieldNameIsSecurityRisk(fieldName))
	{
		_parseStatus = HTTP_Request::BAD_REQUEST;
		return (FAILURE);
	}

	if (fieldName == HTTP_FieldName::CONTENT_LENGTH)
	{
		size_t value_size_t;

		if (numberIsPositive(value) && toNumber(value, value_size_t))
			_headers[HTTP_FieldName::CONTENT_LENGTH] = toString(value_size_t);
		else
		{
			/* not possible to parse value as a number */
			_parseStatus = HTTP_Request::BAD_REQUEST;
			return (FAILURE);
		}
	}
	else
	{
		value = trimString(value, DISALLOWED_CHARS_IN_FIELD_VALUE);
		_headers[fieldName] = value;
	}

	_countHeaderIfRequired(fieldName);

	return (SUCCESS);
}

/* this function can include other headers
 * in the future if needed
 */
int HTTP_Request::_countHeaderIfRequired(std::string fieldName)
{
	if (fieldName == HTTP_FieldName::HOST)
		_headersRequiredCount++;
	return (SUCCESS);
}

/* Defined in RFC 9112, summarized in RFC 9110 */
int HTTP_Request::_fieldNameIsValid(std::string fieldName)
{
	const static std::string allowedChars(ALLOWED_CHARS_IN_FIELD_NAME);

	if (fieldName.empty())
		return (FAILURE);

	for (std::string::size_type i = 0; i < fieldName.size(); ++i)
	{
		unsigned char c = static_cast<unsigned char>(fieldName[i]);

		if (std::isalnum(c))
			continue;

		if (allowedChars.find(c, 0) == std::string::npos)
			return (FAILURE);
	}
	return (SUCCESS);
}

/* defined in RFC 9112, with semantics summarized in RFC 9110 */
int HTTP_Request::_headerValueIsValid(std::string value)
{
	for (std::string::size_type i = 0; i < value.size(); ++i)
	{
		unsigned char c = static_cast<unsigned char>(value[i]);

		/* IMPORTANT SECURITY CONCERN:
		 * the below has CR and LF and '\t' characters covered!
		 */
		if (c < 32 || c == 127)
			return (FAILURE);
	}
	return (SUCCESS);
}

int HTTP_Request::_fieldNameAlreadyProcessed(std::string fieldName)
{
	if (_headers.find(fieldName) != _headers.end())
		return (SUCCESS);
	return (FAILURE);
}

/* Disallow CONTENT_LENGTH && TRANSFER_ENCODING headers,
 * both at the same time in the http request to avoid "request smuggling"
 */
int HTTP_Request::_fieldNameIsSecurityRisk(std::string fieldName)
{
	if (fieldName == HTTP_FieldName::TRANSFER_ENCODING &&
		_fieldNameAlreadyProcessed(HTTP_FieldName::CONTENT_LENGTH))
		return (SUCCESS);

	if (fieldName == HTTP_FieldName::CONTENT_LENGTH &&
		_fieldNameAlreadyProcessed(HTTP_FieldName::TRANSFER_ENCODING))
		return (SUCCESS);

	return (FAILURE);
}

bool HTTP_Request::ready()
{
	return (this->_parseStatus == HTTP_Request::COMPLETE ||
			this->_parseStatus == HTTP_Request::BAD_REQUEST);
}

void HTTP_Request::reset()
{
	_method = "";
	_url = "";
	_version = "";
	_requestLine_completed = false;

	_headers.clear();

	_headers_completed = false;
	_headersRequiredCount = 0;

	_bodyLen = 0;
	_body = "";
	_body_completed = false;

	_parseStatus = INCOMPLETE;
}

std::ostream &operator<<(std::ostream &os, HTTP_Request &hr)
{
	if (!hr._requestLine_completed) /* TO-DO: this is for debug only! */
	{
		os << "HTTP REQUEST [ DEBUG ]: incomplete request - incomplete header line";
		return (os);
	}

	os << hr._method << " " << hr._url << " " << hr._version;

	os << CRLF;

	if (!hr._headers_completed) /* TO-DO: this is for debug only! */
	{
		os << "HTTP REQUEST [ DEBUG ]: incomplete request - incomplete headers";
		return (os);
	}

	std::map<std::string, std::string>::const_iterator it;
	for (it = hr._headers.begin(); it != hr._headers.end(); ++it)
	{
		std::string fieldName = it->first;
		std::string value = it->second;

		os << fieldName << ": " << value << CRLF;
	}

	os << CRLF;

	if (!hr._body_completed) /* TO-DO: this is for debug only! */
	{
		os << "HTTP REQUEST [ DEBUG ]: incomplete request - incomplete body";
		return (os);
	}

	os.write(hr._body.c_str(), hr._body.size());
	return (os);
}
