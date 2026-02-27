/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Request.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 16:46:32 by aistok            #+#    #+#             */
/*   Updated: 2026/02/27 18:01:15 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream> // stringbuf
#include <istream> // istream

#include "HTTP/HTTP_Request.hpp"

HTTP_Request::HTTP_Request() : method(""),
							   url(""),
							   version(""),
							   requestLine_completed(false),
							   headers_completed(false),
							   headersRequiredCount(0),
							   bodyLen(0),
							   body_completed(false),
							   parseStatus(INCOMPLETE)
{
	// done
}

HTTP_Request::HTTP_Request(const char *raw, size_t len) : method(""),
														  url(""),
														  version(""),
														  requestLine_completed(false),
														  headers_completed(false),
														  headersRequiredCount(0),
														  bodyLen(0),
														  body_completed(false),
														  parseStatus(INCOMPLETE)
{
	parse(raw, len);
}

/* getline removes the '\n' from each line it reads! */
int HTTP_Request::parse(const char *raw, size_t len)
{
	if (!len)
	{
		parseStatus = HTTP_Request::INCOMPLETE;
		return (FAILURE);
	}

	std::string rawStr(raw, len);
	std::stringbuf sb(rawStr);
	std::istream is(&sb);
	std::string line;

	if (!requestLine_completed)
	{
		if (!std::getline(is, line))
		{
			/* no data available
			 * QUESTION: is this even possible? for '\n' to be missing?
			 */
			this->parseStatus = HTTP_Request::INCOMPLETE;
			return (FAILURE);
		}
		if (!parseRequestLine(line))
			/* request line was malformed?
			 * parseStatus will be set!
			 */
			return (FAILURE);

		requestLine_completed = true;
	}

	while (!headers_completed && std::getline(is, line))
	{
		if (line == CR) /* LF was removed by getline! */
		{
			/* reached the end of headers;
			 * body may be present after this
			 */
			if (headersRequiredCount < 2)
			{
				parseStatus = HTTP_Request::BAD_REQUEST;
				return (FAILURE);
			}
			headers_completed = true;
			break;
		}
		else
		{
			if (!HTTP_Request::parseHeaderLine(line))
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
		if (headers.find(HTTP_FieldName::CONTENT_LENGTH) != headers.end() &&
			toNumber(headers[HTTP_FieldName::CONTENT_LENGTH], bodyLen))
		{
			/* bodyLen is now set */
		}
		else if (headers.find(HTTP_FieldName::TRANSFER_ENCODING) != headers.end())
			bodyLen = is.rdbuf()->in_avail();

		this->body = std::string(bodyLen, '\0');
		is.read(&this->body[0], bodyLen);

		if (is.gcount() != static_cast<std::streamsize>(bodyLen))
		{
			parseStatus = HTTP_Request::INCOMPLETE;
			return (FAILURE); /* TO-DO: should this be SUCCESS?
							   * ex: if it's "Transfer-Encoding: chunked
							   */
		}
	}

	body_completed = true;
	parseStatus = HTTP_Request::COMPLETE;
	return (SUCCESS);
}

/* TO-DO: if this function is used elsewhere too, place it in utils */
int HTTP_Request::removePortion(std::string &line, std::string portion)
{
	if (line.find(portion) == std::string::npos)
		return (FAILURE);
	line.erase(line.size() - portion.size(), portion.size());
	return (SUCCESS);
}

// TO-DO: should be protected
int HTTP_Request::parseRequestLine(std::string line)
{
	/*
	 *	getline removes '\n' (LF), so,
	 *	only check and remove '\r' (CR).
	 */
	if (!removePortion(line, CR))
	{
		parseStatus = HTTP_Request::BAD_REQUEST;
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
			if (!parseMethod(subString))
			{
				parseStatus = HTTP_Request::BAD_REQUEST;
				return (FAILURE);
			}
		}
		else if (part == 2)
		{
			if (!parseURL(subString))
			{
				parseStatus = HTTP_Request::BAD_REQUEST;
				return (FAILURE);
			}
		}
		part++;
	}

	if (part == 3)
	{
		subString = line.substr(start, line.size() - start);
		if (!parseVersion(subString))
		{
			parseStatus = HTTP_Request::BAD_REQUEST;
			return (FAILURE);
		}
	}
	else
	{
		parseStatus = HTTP_Request::BAD_REQUEST;
		return (FAILURE);
	}

	/*	request line parsed succesfully
	 *	this line is mandatory to be present in the request,
	 *	therefore, count it in the required headers
	 */
	headersRequiredCount++;
	return (SUCCESS);
}

int HTTP_Request::parseMethod(std::string method)
{
	if (method == HTTP_Method::GET ||
		method == HTTP_Method::POST ||
		method == HTTP_Method::DELETE)
	{
		this->method = method;
		return (SUCCESS);
	}
	return (FAILURE);
}

int HTTP_Request::parseURL(std::string url)
{
	if (!URLIsValid(url))
		return (FAILURE);
	this->url = url;
	return (SUCCESS);
}

int HTTP_Request::parseVersion(std::string version)
{
	if (version == "HTTP/1.1" ||
		version == "HTTP/1.0")
	{
		this->version = version;
		return (SUCCESS);
	}
	return (FAILURE);
}

int HTTP_Request::URLIsValid(std::string url)
{
	/* TO-DO: verify if url is valid */
	/* 	ex. contains white spaces, control characters,
	 *	etc. (RFC 9110, 9112, 3986) */
	(void)url;
	return (SUCCESS);
}

// TO-DO: should be protected
int HTTP_Request::parseHeaderLine(std::string line)
{
	/*
	 *	getline removes '\n' (LF), so,
	 *	only check and remove '\r' (CR).
	 */
	if (!removePortion(line, CR))
	{
		parseStatus = HTTP_Request::BAD_REQUEST;
		return (FAILURE);
	}

	std::string::size_type pos = line.find(":", 0);
	if (pos == std::string::npos)
	{
		parseStatus = HTTP_Request::BAD_REQUEST;
		return (FAILURE);
	}

	std::string fieldName = line.substr(0, pos);
	fieldName = capitaliseFirstLetter(fieldName);
	std::string value = line.substr(pos + 1, line.size());
	if (!fieldNameIsValid(fieldName) ||
		!headerValueIsValid(value) ||
		fieldNameAlreadyProcessed(fieldName) ||
		fieldNameIsSecurityRisk(fieldName))
	{
		parseStatus = HTTP_Request::BAD_REQUEST;
		return (FAILURE);
	}

	if (fieldName == HTTP_FieldName::CONTENT_LENGTH)
	{
		size_t value_size_t;

		if (validNumber(value) && toNumber(value, value_size_t))
			headers[HTTP_FieldName::CONTENT_LENGTH] = toString(value_size_t);
		else
		{
			/* not possible to parse value as a number */
			parseStatus = HTTP_Request::BAD_REQUEST;
			return (FAILURE);
		}
	}
	else
	{
		value = trimString(value, DISALLOWED_CHARS_IN_FIELD_VALUE);
		headers[fieldName] = value;
	}

	countHeaderIfRequired(fieldName);

	return (SUCCESS);
}

/* this function can include other headers
 * in the future if needed
 */
int HTTP_Request::countHeaderIfRequired(std::string fieldName)
{
	if (fieldName == HTTP_FieldName::HOST)
		headersRequiredCount++;
	return (SUCCESS);
}

/* Defined in RFC 9112, summarized in RFC 9110 */
int HTTP_Request::fieldNameIsValid(std::string fieldName)
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
int HTTP_Request::headerValueIsValid(std::string value)
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

int HTTP_Request::fieldNameAlreadyProcessed(std::string fieldName)
{
	if (headers.find(fieldName) != headers.end())
		return (SUCCESS);
	return (FAILURE);
}

/* Disallow CONTENT_LENGTH && TRANSFER_ENCODING headers,
 * both at the same time in the http request to avoid "request smuggling"
 */
int HTTP_Request::fieldNameIsSecurityRisk(std::string fieldName)
{
	if (fieldName == HTTP_FieldName::TRANSFER_ENCODING &&
		fieldNameAlreadyProcessed(HTTP_FieldName::CONTENT_LENGTH))
		return (SUCCESS);

	if (fieldName == HTTP_FieldName::CONTENT_LENGTH &&
		fieldNameAlreadyProcessed(HTTP_FieldName::TRANSFER_ENCODING))
		return (SUCCESS);

	return (FAILURE);
}

int HTTP_Request::validNumber(std::string value)
{
	if (value.empty())
		return (FAILURE);

	/* disallow negative values for content-length header */
	if (value.find('-') != std::string::npos)
		return (FAILURE);

	return (SUCCESS);
}

bool HTTP_Request::ready()
{
	return (this->parseStatus == HTTP_Request::COMPLETE ||
			this->parseStatus == HTTP_Request::BAD_REQUEST);
}

std::ostream &operator<<(std::ostream &os, HTTP_Request &hr)
{
	if (!hr.requestLine_completed) /* TO-DO: this is for debug only! */
	{
		os << "HTTP REQUEST [ DEBUG ]: incomplete request - incomplete header line";
		return (os);
	}

	os << hr.method << " " << hr.url << " " << hr.version;

	os << CRLF;

	if (!hr.headers_completed) /* TO-DO: this is for debug only! */
	{
		os << "HTTP REQUEST [ DEBUG ]: incomplete request - incomplete headers";
		return (os);
	}

	std::map<std::string, std::string>::const_iterator it;
	for (it = hr.headers.begin(); it != hr.headers.end(); ++it)
	{
		std::string fieldName = it->first;
		std::string value = it->second;

		os << fieldName << ": " << value << CRLF;
	}

	os << CRLF;

	if (!hr.body_completed) /* TO-DO: this is for debug only! */
	{
		os << "HTTP REQUEST [ DEBUG ]: incomplete request - incomplete body";
		return (os);
	}

	os.write(hr.body.c_str(), hr.body.size());
	return (os);
}
