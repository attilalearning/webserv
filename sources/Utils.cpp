#include "Utils.hpp"
#include <algorithm>
#include <cctype>
#include <ctime>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

/*private constructors to prevent instantiation*/
Utils::Utils() {}
Utils::~Utils() {}

std::string Utils::trim(const std::string& str)
{
    size_t start = 0;
    size_t end = str.length();

    while (start < end && std::isspace(str[start]))
        ++start;
    while (end > start && std::isspace(str[end - 1]))
        --end;
    return (str.substr(start, end - start));
}

std::string Utils::toLowerCase(const std::string& str)
{
    std::string res = str;
    
    for (size_t i = 0; i < str.length(); ++i)
        res[i] = std::tolower(res[i]);

    return (res);
}

std::string Utils::toUpperCase(const std::string& str)
{
    std::string res = str;

    for (size_t i = 0; i < str.length(); ++i)
        res[i] = std::toupper(res[i]);

    return (res);
}

std::vector<std::string> Utils::split(const std::string& str, char delimiter)
{
    std::vector<std::string> res;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, delimiter))
    {
        if (!item.empty())
            res.push_back(item);
    }
    
    return (res);
}

std::vector<std::string> Utils::split(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> res;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos)
    {
        res.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    res.push_back(str.substr(start));
    
    return (res);
}

bool Utils::startsWith(const std::string& str, const std::string& prefix)
{
    if (prefix.length() > str.length())
        return (false);
    return (str.substr(0, prefix.length()) == prefix);
}

bool Utils::endsWith(const std::string& str, const std::string& suffix)
{
    if (suffix.length() > str.length())
        return (false);
    return (str.substr(str.length() - suffix.length()) == suffix);
}

bool Utils::fileExists(const std::string& path)
{
    struct stat st;
    return (stat(path.c_str(), &st) == 0);
}

/*Number conversions*/
int Utils::toInt(const std::string& str) {
    std::stringstream ss(str);
    int res;
    ss >> res;
    return (res);
}

size_t Utils::toSizeT(const std::string& str) {
    std::stringstream ss(str);
    int res;
    ss >> res;
    return (res);
}