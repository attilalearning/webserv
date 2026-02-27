#include <fcntl.h>
#include <unistd.h>

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

bool replace(std::string &str, const std::string &from, const std::string &to)
{
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
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
