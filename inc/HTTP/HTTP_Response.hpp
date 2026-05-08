/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Response.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 16:34:38 by aistok            #+#    #+#             */
/*   Updated: 2026/05/07 14:05:47 by mosokina         ###   ########.fr       */
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
#include "CGI.hpp"

// TO-DO: orthodox canonical form!
class HTTP_Response
{
public:
	HTTP_Response();
	HTTP_Response(const HTTP_StatusPair &status);
	HTTP_Response(const HTTP_StatusPair &status, std::string textContent);

	std::map<std::string, std::string> &getHeaders();

	void setStatus(const HTTP_StatusPair &status); // will set status message too
												   // add headers

	std::string serialize();
	void setContent(const std::string &text);
	size_t getBodyLen() const; // TO-DO: temporary only, to compile the project
	
	void setHeadersOnly(const bool value);
	bool isHeadersOnly();
	
	void setCGIGenerated(const bool value);
	bool isCGIGenerated();
	void reset();

	void setCgiPath(const std::string &path);
    std::string getCgiPath() const;

    void setScriptPath(const std::string &path);
    std::string getScriptPath() const;

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

	bool _isHEADresponse;
	bool _isCGIGenerated;
	size_t _bodyLen;
	std::string _body;

	void _addServerNameHeader();
	void _addDegubHeaders();

	std::string _cgiPath;
    std::string _scriptPath;
};

#endif // HTTP_RESPONSE_HPP
