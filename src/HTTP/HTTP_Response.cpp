/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Response.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 16:34:38 by aistok            #+#    #+#             */
/*   Updated: 2026/05/10 23:31:17 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP/HTTP_Response.hpp"

HTTP_Response::HTTP_Response() : _status(HTTP_Status::UNSET),
								 _version(HTTP_Version::v1_1),
								 _isHEADresponse(false),
								 _isCGIGenerated(false),
								 _body(""),
								 _cgiPath(""),
								 _scriptPath("")
{
	_addServerNameHeader();
	_addDegubHeaders();
}

HTTP_Response::HTTP_Response(const HTTP_StatusPair &status) : _status(status),
															  _version(HTTP_Version::v1_1),
															  _isHEADresponse(false),
															  _isCGIGenerated(false),
															  _body(""),
															  _cgiPath(""),
								 							  _scriptPath("")
{
	_addServerNameHeader();
	_addDegubHeaders();
}

HTTP_Response::HTTP_Response(
	const HTTP_StatusPair &status, std::string textContent) : _status(status),
															  _version(HTTP_Version::v1_1),
															  _isHEADresponse(false),
															  _isCGIGenerated(false),
															  _body(""),
															  _cgiPath(""),
								 							  _scriptPath("") 
{
	_addServerNameHeader();
	_addDegubHeaders();
	setContent(textContent);
}

HTTP_Response &HTTP_Response::operator=(const HTTP_Response &other)
{
	if (this != &other)
	{
		_status = other._status;
		_version = other._version;

		_headers = other._headers;

		_isHEADresponse = other._isHEADresponse;
		_isCGIGenerated = other._isCGIGenerated;
		_body = other._body;

		_cgiPath = other._cgiPath;
		_scriptPath = other._scriptPath;
	}
	return (*this);
}

HTTP_Response::~HTTP_Response() {}

std::map<std::string, std::string> &HTTP_Response::getHeaders()
{
	return (_headers);
}

void HTTP_Response::setStatus(const HTTP_StatusPair &status)
{
	_status = status;
}

std::string HTTP_Response::serialize()
{

	std::ostringstream oss(std::ios::binary);
	oss << *this;
	return (oss.str());
}

void HTTP_Response::setContent(const std::string &text)
{
	if (!_isHEADresponse)
	{
		_body = text;
		_headers[HTTP_FieldName::CONTENT_LENGTH] = ::toString(text.length());
	}
}

void HTTP_Response::setHeadersOnly(const bool value)
{
	_isHEADresponse = value;
}

bool HTTP_Response::isHeadersOnly()
{
	return (_isHEADresponse);
}

void HTTP_Response::setCGIGenerated(const bool value)
{
	_isCGIGenerated = value;
}

bool HTTP_Response::isCGIGenerated()
{
	return (_isCGIGenerated);
}

void HTTP_Response::reset()
{
	_status = HTTP_Status::UNSET;
	_version = HTTP_Version::v1_1;

	_headers.clear();

	_isHEADresponse = false;
	_isCGIGenerated = false;
	_body = "";
	_cgiPath = "";
	_scriptPath = "";
}

void HTTP_Response::_addServerNameHeader()
{
	_headers[HTTP_FieldName::SERVER_NAME] = WEBSERV_NAME;
}

void HTTP_Response::_addDegubHeaders()
{
	return; // enable/disable the below - debugging!
	_headers["Cache-Control"] = "no-store, no-cache, must-revalidate, max-age=0";
	_headers["Pragma"] = "no-cache";
	_headers["Expires"] = "0";
}

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

void HTTP_Response::setCgiPath(const std::string &path) {
    _cgiPath = path;
}

std::string HTTP_Response::getCgiPath() const {
    return _cgiPath;
}

void HTTP_Response::setScriptPath(const std::string &path) {
    _scriptPath = path;
}

std::string HTTP_Response::getScriptPath() const {
    return _scriptPath;
}