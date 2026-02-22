/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockTestFnctions.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 12:49:26 by mosokina          #+#    #+#             */
/*   Updated: 2026/02/11 13:38:19 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MOCKTESTFUNCTIONS_HPP
#define MOCKTESTFUNCTIONS_HPP

#include <iostream>
#include <csignal>

#include "WebServ.hpp"
#include "ConfigStructs.hpp"
#include "Server.hpp"

std::vector<ServerConfig> getMockConfig();
void runTemporaryTest(WebServ &ws);

#endif