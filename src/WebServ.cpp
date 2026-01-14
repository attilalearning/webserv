/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 19:03:57 by aistok            #+#    #+#             */
/*   Updated: 2026/01/14 19:27:55 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "WebServ.hpp"

/* public section ----------------------------- */

WebServ::WebServ()
{
	std::cout << "This is gooooing to be the " << _name << "!" << std::endl;
}

WebServ::~WebServ()
{
	std::cout << _name << ": bye!" << std::endl;
}

WebServ::WebServ(const WebServ &other)
{
	(void) other;
	//..
}

WebServ &WebServ::operator=(const WebServ &other)
{
	(void) other;
	//..
	return (*this);
}

void WebServ::run(void)
{
	std::cout << _name << ": running (not quite just yer)" << std::endl;
}

/* protected section -------------------------- */

//...

/* private section ---------------------------- */

const std::string WebServ::_name = "WebServ";

