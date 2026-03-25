/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoriesToHTML.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 08:59:06 by aistok            #+#    #+#             */
/*   Updated: 2026/03/25 09:37:20 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include "DirectoriesToHTML.hpp"

std::string DirectoriesToHTML::_templateHTMLPage =
	"<!DOCTYPE html>\n"
	"<html lang=\"en\">\n"
	"<head>\n"
	"  <meta charset=\"UTF-8\">\n"
	"  <title>Content of {{REQUEST_URL}}</title>\n"
	"  <style>\n"
	"    body { font-family: Arial, sans-serif; background: #f4f4f4; }\n"
	"    .container { margin: 20px auto; text-align: left; }\n"
	"    h1 { font-size: 72px; margin-bottom: 10px; }\n"
	"    p { font-size: 24px; color: #555; }\n"
	"  </style>\n"
	"</head>\n"
	"<body>\n"
	"  <div class=\"container\">\n"
	"    <h1>Content of {{REQUEST_URL}}</h1>\n"
	"{{DIRECTORY_LIST}}\n"
	"  </div>\n"
	"</body>\n"
	"</html>\n";
std::string DirectoriesToHTML::_templateHTMLDirectoryListItem =
	"    <p>{{DIRECTORY_NAME}}</p>\n";

DirectoriesToHTML::DirectoriesToHTML()
{
}

DirectoriesToHTML::~DirectoriesToHTML()
{
}

std::string DirectoriesToHTML::generate(
	const std::vector<std::string> &dirList,
	const std::string &requestURL)
{
	std::string result = _templateHTMLPage;
	result = Utils::replaceAll(result, "{{REQUEST_URL}}", requestURL);

	std::string htmlDirectoryList;

	std::vector<std::string>::const_iterator dirList_it = dirList.begin();
	for (; dirList_it != dirList.end(); ++dirList_it)
	{
		htmlDirectoryList += Utils::replaceAll(_templateHTMLDirectoryListItem,
											   "{{DIRECTORY_NAME}}", *dirList_it);
	}

	result = Utils::replaceAll(result, "{{DIRECTORY_LIST}}", htmlDirectoryList);
	return result;
}
