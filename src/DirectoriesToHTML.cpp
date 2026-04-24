/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoriesToHTML.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 08:59:06 by aistok            #+#    #+#             */
/*   Updated: 2026/04/23 21:05:37 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DirectoriesToHTML.hpp"

std::string DirectoriesToHTML::_templateHTMLPage =
	"<!DOCTYPE html>\n"
	"<html lang=\"en\">\n"
	"<head>\n"
	"    <meta charset=\"UTF-8\">\n"
	"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
	"    <title>Content of {{REQUEST_URL}}</title>\n"
	"    <style>\n"
	"        body { font-family: Arial, sans-serif; }\n"
	"        table { border-collapse: collapse; width: 100%; }\n"
	"        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n"
	"        th { background-color: #f0f0f0; }\n"
	"    </style>\n"
	"</head>\n"
	"<body>\n"
	"    <h1>Content of {{REQUEST_URL}}</h1>\n"
	"    <table>\n"
	"        <tr>\n"
	"            <th>Entry</th>\n"
	"            <th>Info</th>\n"
	"        </tr>\n"
	"{{DIRECTORY_LIST}}\n"
	"    </table>\n"
	"</body>\n"
	"</html>\n";
std::string DirectoriesToHTML::_templateHTMLDirectoryListItem =
	"        <tr>\n"
	"            <td><a href=\"{{RESOURCE_HYPERLINK}}\">{{ENTRY_NAME}}</a></td>\n"
	"            <td>{{ENTRY_INFO}}</td>\n"
	"        </tr>\n";

DirectoriesToHTML::DirectoriesToHTML() {}

DirectoriesToHTML::~DirectoriesToHTML() {}

std::string DirectoriesToHTML::generate(
	const std::vector<fsItem> &itemsList,
	const std::string &requestURL)
{
	std::string result = _templateHTMLPage;
	result = Utils::replaceAll(result, "{{REQUEST_URL}}", requestURL);

	std::string htmlDirectoryList;

	std::vector<fsItem>::const_iterator itemList_it = itemsList.begin();
	for (; itemList_it != itemsList.end(); ++itemList_it)
	{
		std::string directoryEntry;

		directoryEntry =
			Utils::replaceAll(_templateHTMLDirectoryListItem, "{{ENTRY_NAME}}", itemList_it->name);

		std::string link = requestURL + itemList_it->name;
		if (itemList_it->isDir)
		{
			link += "/";
			directoryEntry =
				Utils::replaceAll(directoryEntry, "{{ENTRY_INFO}}", "DIR");
		}
		else
		{
			directoryEntry =
				Utils::replaceAll(directoryEntry, "{{ENTRY_INFO}}", ::toString(itemList_it->size) + " bytes");
		}

		directoryEntry =
			Utils::replaceAll(directoryEntry, "{{RESOURCE_HYPERLINK}}", link);

		htmlDirectoryList += directoryEntry;
	}

	result = Utils::replaceAll(result, "{{DIRECTORY_LIST}}", htmlDirectoryList);
	return result;
}
