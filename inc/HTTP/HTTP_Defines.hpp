/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Defines.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 00:32:21 by aistok            #+#    #+#             */
/*   Updated: 2026/04/09 13:27:22 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_DEFINES_HPP
#define HTTP_DEFINES_HPP

#define WEBSERV_NAME "miniMAA"
#define CR "\r"
#define LF "\n"
#define CRLF CR LF
#define DBL_CRLF "\r\n\r\n"
#define DISALLOWED_CHARS_IN_FIELD_VALUE " \t"
#define ALLOWED_CHARS_IN_FIELD_NAME "!#$%&'*+-.^_`|~"
#define SUCCESS 1
#define FAILURE 0

#endif // HTTP_DEFINES_HPP
