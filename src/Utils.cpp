/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 11:32:29 by mosokina          #+#    #+#             */
/*   Updated: 2026/04/23 20:59:42 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

bool setNonBlocking(int fd)
{
	if (fd < 0)
		return false;
	// 1. Handle File Status Flags (O_NONBLOCK)
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return false;
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		return false;
	// 2. Handle File Descriptor Flags (FD_CLOEXEC)
	// Highly recommended for the CGI part of the project!
	// int fd_flags = fcntl(fd, F_GETFD, 0);
	// if (fd_flags != -1)
	// 	fcntl(fd, F_SETFD, fd_flags | FD_CLOEXEC);
	return true;
}

std::string toUpperCase(std::string &original)
{
	std::string upperCaseCopy = original;
	for (size_t i = 0; i < upperCaseCopy.length(); ++i)
	{
		upperCaseCopy[i] = static_cast<char>(std::toupper(
			static_cast<unsigned char>(upperCaseCopy[i])));
	}
	return (upperCaseCopy);
}

std::string &capitaliseFirstLetter(std::string &str)
{
	unsigned char previousChar;
	unsigned char firstChar;
	unsigned char currentChar;

	if (!str.empty())
	{
		for (std::string::size_type i = 0; i < str.size(); ++i)
		{
			if (i > 0)
				previousChar = static_cast<unsigned char>(str[i - 1]);

			if (i == 0 || (i > 0 && !std::isalpha(previousChar)))
			{
				firstChar = static_cast<unsigned char>(str[i]);
				str[i] = static_cast<char>(std::toupper(firstChar));
			}
			else
			{
				currentChar = static_cast<unsigned char>(str[i]);
				str[i] = static_cast<char>(std::tolower(currentChar));
			}
		}
	}
	return (str);
}

std::string &trimString(std::string &str, std::string stripChars)
{
	std::string::size_type start = 0;
	std::string::size_type end = str.size();

	if (end > 0)
	{
		while (start < end &&
			   (stripChars.find(str[start]) != std::string::npos))
			++start;

		while (end > start &&
			   (stripChars.find(str[end - 1]) != std::string::npos))
			--end;

		if (start != 0 || end != str.size())
			str = str.substr(start, end - start);
	}
	return (str);
}

bool replace(std::string &str, const std::string &what, const std::string &with)
{
	size_t start_pos = str.find(what);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, what.length(), with);
	return true;
}

bool removePortion(std::string &line, std::string portion)
{
	if (line.find(portion) == std::string::npos)
		return (false);
	line.erase(line.size() - portion.size(), portion.size());
	return (true);
}

bool numberIsPositive(std::string value)
{
	if (value.empty())
		return (false);

	if (value.find('-') != std::string::npos)
		return (false);

	return (true);
}

PathType getPathType(const std::string &pathStr)
{
	struct stat st;

	if (stat(pathStr.c_str(), &st) != 0)
		return PATH_NONE;

	if (S_ISREG(st.st_mode))
		return PATH_FILE;

	if (S_ISDIR(st.st_mode))
		return PATH_DIRECTORY;

	return PATH_NONE;
}

/*private constructors to prevent instantiation*/
Utils::Utils() {}
Utils::~Utils() {}

std::string Utils::trim(const std::string &str)
{
	size_t start = 0;
	size_t end = str.length();

	while (start < end && std::isspace(str[start]))
		++start;
	while (end > start && std::isspace(str[end - 1]))
		--end;
	return (str.substr(start, end - start));
}

std::string Utils::toLowerCase(const std::string &str)
{
	std::string res = str;

	for (size_t i = 0; i < str.length(); ++i)
		res[i] = std::tolower(res[i]);

	return (res);
}

std::string Utils::toUpperCase(const std::string &str)
{
	std::string res = str;

	for (size_t i = 0; i < str.length(); ++i)
		res[i] = std::toupper(res[i]);

	return (res);
}

std::vector<std::string> Utils::split(const std::string &str, char delimiter)
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

std::vector<std::string> Utils::split(const std::string &str, const std::string &delimiter)
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

bool Utils::startsWith(const std::string &str, const std::string &prefix)
{
	if (prefix.length() > str.length())
		return (false);
	return (str.substr(0, prefix.length()) == prefix);
}

bool Utils::endsWith(const std::string &str, const std::string &suffix)
{
	if (suffix.length() > str.length())
		return (false);
	return (str.substr(str.length() - suffix.length()) == suffix);
}

std::string Utils::replaceAll(const std::string &src,
							  const std::string &what,
							  const std::string &with)
{
	if (what.empty())
		return src;

	std::string result = src;
	std::string::size_type pos = 0;

	while ((pos = result.find(what, pos)) != std::string::npos)
	{
		result.replace(pos, what.length(), with);
		pos += with.length();
	}
	return result;
}

bool Utils::fileExists(const std::string &path)
{
	struct stat st;
	return (stat(path.c_str(), &st) == 0);
}

/*Number conversions*/
int Utils::toInt(const std::string &str)
{
	std::stringstream ss(str);
	int res;
	ss >> res;
	return (res);
}

size_t Utils::toSizeT(const std::string &str)
{
	std::stringstream ss(str);
	int res;
	ss >> res;
	return (res);
}

std::string Utils::getFileContent(const std::string &filename)
{
	std::ifstream file(filename.c_str(), std::ios_base::binary);

	if (!file.is_open())
	{
		throw std::runtime_error(filename + " can not be opened!");
	}

	std::string content((std::istreambuf_iterator<char>(file)),
						std::istreambuf_iterator<char>());

	return content;
}

std::vector<fsItem> Utils::getDirectoryList(const std::string &path)
{
	std::vector<fsItem> result;

	DIR *dir = opendir(path.c_str());
	if (!dir)
		throw std::runtime_error(path + " is not a directory!");

	struct dirent *entry;
	struct stat st;

	while ((entry = readdir(dir)) != NULL)
	{
		fsItem item;

		item.name = entry->d_name;
		if (item.name != "." && item.name != "..")
		{
			std::string itemPath = path + "/" + item.name;
			if (stat(itemPath.c_str(), &st) == 0)
			{
				item.isReadable = Utils::isReadable(itemPath);
				item.isDir = (st.st_mode & S_IFDIR) != 0;
				if (!item.isDir)
					item.size = st.st_size;
			}
			else
			{
				// ... this should not happen - the way this function is called
			}
			result.push_back(item);
		}
	}

	closedir(dir);
	// std::sort(result.begin(), result.end());

	return (result);
}

bool Utils::isReadable(const std::string &pathOnServer)
{
	struct stat st;

	if (stat(pathOnServer.c_str(), &st) != 0)
		return false; // error accessing file

	uid_t uid = getuid();
	gid_t gid = getgid();

	if (st.st_uid == uid) // checing owner permissions
		return (st.st_mode & S_IRUSR) != 0;

	else if (st.st_gid == gid) // checking group permissions
		return (st.st_mode & S_IRGRP) != 0;

	else // checking others permissions
		return (st.st_mode & S_IROTH) != 0;
}
