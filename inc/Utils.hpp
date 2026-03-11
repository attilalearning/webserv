/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 23:56:58 by mosokina          #+#    #+#             */
/*   Updated: 2026/03/11 18:53:31 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <sstream>
#include <string>
#include <vector>

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

bool removePortion(std::string &line, std::string portion);
bool numberIsPositive(std::string value);

PathType getPathType(const std::string &pathStr);

class Utils
{
    public:
        /*String Manipulations*/
        static std::string trim(const std::string& str);
        static std::string toLowerCase(const std::string& str);
        static std::string toUpperCase(const std::string& str);
        static std::vector<std::string> split(const std::string& str, char delimiter);
        static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
        static bool startsWith(const std::string& str, const std::string& prefix);
        static bool endsWith(const std::string& str, const std::string& suffix);

        /*File operations*/
        static bool fileExists(const std::string& path);
        static bool isDirectory(const std::string& path);
        static bool isFile(const std::string& path);
        static std::string readFile(const std::string& path);
        static bool writeFile(const std::string& path, const std::string& content);
        static bool deleteFile(const std::string& path);
        static std::vector<std::string> listDirectory(const std::string& path);

        /*Path operations*/

        /*MIME types*/

        /*URL operations*/

        /*Number conversions*/
        static int toInt(const std::string& str);
        static size_t toSizeT(const std::string& str);

        /*Time*/

    private:
        Utils();
        ~Utils();
        Utils(const Utils&);
        Utils& operator=(const Utils&);
};

#endif
