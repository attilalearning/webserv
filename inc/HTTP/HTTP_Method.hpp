/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_Method.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 21:56:26 by aistok            #+#    #+#             */
/*   Updated: 2026/04/08 17:05:12 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_METHOD_HPP
#define HTTP_METHOD_HPP

#include <string>

class HTTP_Method
{
public:
	static const std::string GET;
	static const std::string HEAD;
	static const std::string POST;
	static const std::string DELETE;
};

#endif // HTTP_METHOD_HPP
