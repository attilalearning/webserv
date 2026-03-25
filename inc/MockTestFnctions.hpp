/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MockTestFnctions.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 12:49:26 by mosokina          #+#    #+#             */
/*   Updated: 2026/03/25 13:59:33 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MOCKTESTFUNCTIONS_HPP
#define MOCKTESTFUNCTIONS_HPP

#include <iostream>
#include <csignal>

#include "WebServ.hpp"
#include "ConfigStructs.hpp"
#include "Listener.hpp"

std::vector<ServerConfig> getMockConfig();
void runTemporaryTest(WebServ &ws);

#endif