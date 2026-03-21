/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Defines.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 00:32:21 by aistok            #+#    #+#             */
/*   Updated: 2026/03/20 15:17:55 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_DEFINES_HPP
#define HTTP_DEFINES_HPP

#define CR "\r"
#define LF "\n"
#define CRLF CR LF
#define DBL_CRLF "\r\n\r\n"
#define DISALLOWED_CHARS_IN_FIELD_VALUE " \t"
#define ALLOWED_CHARS_IN_FIELD_NAME "!#$%&'*+-.^_`|~"
#define SUCCESS 1
#define FAILURE 0

#endif // HTTP_DEFINES_HPP
