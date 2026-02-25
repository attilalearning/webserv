/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPages.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 08:52:13 by aistok            #+#    #+#             */
/*   Updated: 2026/02/25 10:42:00 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_PAGES_HPP
#define ERROR_PAGES_HPP

#include <string>
#include "HTTP/HTTP_Status.hpp"

class ErrorPages
{
public:
    ErrorPages();
    ~ErrorPages();

    static std::string generate(const HTTP_StatusPair &status);

private:
    static std::string _template;

    static std::string replaceAll(const std::string &src,
                                  const std::string &from,
                                  const std::string &to);
};

#endif // ERROR_PAGES_HPP