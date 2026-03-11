/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Version.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 21:56:26 by aistok            #+#    #+#             */
/*   Updated: 2026/02/21 21:20:36 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_VERSION_HPP
#define HTTP_VERSION_HPP

#include <string>

class HTTP_Version
{
public:
	static const std::string v1_1;
	static const std::string v1_0;
};

#endif // HTTP_VERSION_HPP
