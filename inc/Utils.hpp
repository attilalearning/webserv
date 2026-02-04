/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 23:56:58 by mosokina          #+#    #+#             */
/*   Updated: 2026/02/04 11:04:24 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <sstream>
#include <string>

template <typename T>
std::string toString(const T &value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

#endif
