/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 19:03:57 by aistok            #+#    #+#             */
/*   Updated: 2026/01/14 19:21:33 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef WEBSERV_HPP
# define WEBSERV_HPP
# include <iostream>

class WebServ {

	public:

		WebServ();
		~WebServ();
		WebServ(const WebServ &other);
		WebServ &operator=(const WebServ &other);
		void run();

	protected:

		//...

	private:

		static const std::string _name;

};

#endif // WEBSERV_HPP

