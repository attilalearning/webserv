/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 00:00:40 by mosokina          #+#    #+#             */
/*   Updated: 2026/01/21 00:03:30 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <string>
#include <stdexcept>

int stringToInt(const std::string& str) {
    std::istringstream iss(str);
    int result;
    
    // Try to extract an integer
    // The '>>' operator returns false if it's not a number
    // 'iss >> std::ws' checks if there is any leftover junk after the number
    if (!(iss >> result) || !(iss >> std::ws).eof()) {
        throw std::runtime_error("Invalid number format: " + str);
    }
    return result;
}
