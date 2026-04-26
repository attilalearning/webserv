/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cat_request.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aistok <aistok@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 06:40:19 by aistok            #+#    #+#             */
/*   Updated: 2026/04/26 06:40:22 by aistok           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <string>

void	printFileContent(const std::string& filename, const bool showEscapes)
{
	std::ifstream file(filename.c_str());
	if (!file)
	{
		std::cerr << "Error opening file\n";
		return;
	}

	std::string content((std::istreambuf_iterator<char>(file)),
						 std::istreambuf_iterator<char>());

	for (std::size_t i = 0; i < content.size(); ++i)
	{
		if ((i == 0 && content[i] == '#')
			|| (i > 0 && content[i - 1] == '\n' && content[i] == '#'))
		{
			size_t j = i;
			while (j < content.size() && content[++j] != '\n')
				;
			i = j;
			continue;
		}

		if (!showEscapes && i + 1 < content.size() && content[i] == '\\')
		{
			++i;
			switch (content[i])
			{
				case 'n':
					if ((i + 1 < content.size() && content[i + 1] != '\n')
							|| i + 1 == content.size())
						std::cout << '\n';
					break;
				case 'r': std::cout << '\r'; break;
				case 't': std::cout << '\t'; break;
				case '\\': std::cout << '\\'; break;
				default:
					std::cout << '\\' << content[i];
			}
		}
		else
		{
			std::cout << content[i];
		}
	}
}

void showUsage(const char *programName)
{
	std::cout << "Usage: " << programName << " [-v|--visual] filename" << std::endl;
	std::cout << "Display content of text files to standard output, converting" << std::endl;
	std::cout << "\\n \\r \\t into actual new line, carriage return and tab characters." << std::endl;
	std::cout << std::endl;
	std::cout << "NOTE: if the \\n happens to be followed by an unescaped new line," << std::endl;
	std::cout << "the \\n will be ignored, like it never existed. This is to ease the" << std::endl;
	std::cout << "readability of a HTTP request stored in plain a text file." << std::endl;
	std::cout << std::endl;
	std::cout << "Options:" << std::endl
			<< "  -v, --visual               Will display the exact content of the text file," << std::endl
			<< "                             with zero replacements." << std::endl;
	std::cout << std::endl;

}

int main(int argc, char **argv)
{
	if (argc <= 1)
	{
		showUsage(argv[0]);
		return (1);
	}

	std::string arg1(argv[1]);
	std::string arg2;

	if (arg1 == "-v" || arg1 == "--visual")
	{
		if (argc < 3)
		{
			showUsage(argv[0]);
			return (1);
		}
		arg2 = argv[2];
	}

	if (argc == 2)
	{
		printFileContent(argv[1], false);
		return (0);
	}
	else if (argc == 3 && (arg1 == "-v" || arg1 == "--visual"))
	{
		printFileContent(argv[2], true);
		return (0);
	}

	showUsage(argv[0]);
	return (1);
}
