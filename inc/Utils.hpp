/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 23:56:58 by mosokina          #+#    #+#             */
/*   Updated: 2026/02/26 22:48:45 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <sstream>
#include <string>

#include <sys/stat.h> // used for stat
#include <unistd.h> // used for stat

template <typename T>
std::string toString(const T &value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

template <typename T>
bool toNumber(const std::string &str, T &out)
{
	std::istringstream iss(str);
	iss >> out;

	return !iss.fail() && iss.eof();
}

bool setNonBlocking(int fd);

std::string toUpperCase(std::string &str);
std::string &capitaliseFirstLetter(std::string &str);
std::string &trimString(std::string &str, std::string stripChars);
bool replace(std::string &str, const std::string &from, const std::string &to);

enum PathType {
    PATH_NONE,
    PATH_FILE,
    PATH_DIRECTORY
};

PathType getPathType(const char* path);

#endif
