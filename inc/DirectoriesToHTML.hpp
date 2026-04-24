/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoriesToHTML.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 08:52:13 by aistok            #+#    #+#             */
/*   Updated: 2026/04/23 09:07:34 by aistok           ###   ########.fr       */
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

	static std::string generate(const std::vector<fsItem> &itemsList,
								const std::string &requestURL);

private:
	static std::string _templateHTMLPage;
	static std::string _templateHTMLDirectoryListItem;
};

#endif // DIRECTORIES_TO_HTML_HPP
