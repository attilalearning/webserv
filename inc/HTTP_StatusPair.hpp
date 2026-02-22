/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP_StatusPair.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 00:07:41 by aistok            #+#    #+#             */
/*   Updated: 2026/02/21 01:43:11 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_STATUSPAIR_HPP
#define HTTP_STATUSPAIR_HPP

class HTTP_StatusPair
{
public:
	// int value() const;
	// const char *message() const;

// private:
	//HTTP_StatusPair(int c, const char *t);

	int code;
	const char *text;

	/* allow access for HTTP_Status class */
	//friend class HTTP_Status;
};

#endif // HTTP_STATUSPAIR_HPP
