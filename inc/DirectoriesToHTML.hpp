/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoriesToHTML.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 08:52:13 by aistok            #+#    #+#             */
/*   Updated: 2026/03/25 09:13:16 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRECTORIES_TO_HTML_HPP
#define DIRECTORIES_TO_HTML_HPP

#include <string>
#include <vector>
#include "Utils.hpp"

class DirectoriesToHTML
{
public:
	DirectoriesToHTML();
	~DirectoriesToHTML();

	static std::string generate(const std::vector<std::string> &dirList,
								const std::string &requestURL);

private:
	static std::string _templateHTMLPage;
	static std::string _templateHTMLDirectoryListItem;
};

#endif // DIRECTORIES_TO_HTML_HPP
