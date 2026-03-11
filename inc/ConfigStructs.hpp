/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigStructs.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:16:52 by mosokina          #+#    #+#             */
/*   Updated: 2026/02/04 11:04:05 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_STRUCTS_HPP
#define CONFIG_STRUCTS_HPP

#include <string>
#include <vector>
#include <map>

/*Location-Level Elements
route: The URL prefix (e.g., /images).

allow_methods: A whitelist. If a user tries to DELETE on a GET-only route, return 405 Method Not Allowed.

root: The base directory. If the URL is /test.txt and root is /var/www,
the server looks for /var/www/test.txt.

autoindex: If the URL is a directory (like /images/) and there is no index.html,
do you show a list of files or return a 403 Forbidden?

cgi_ext: Which file extensions should trigger the CGI logic instead of just serving the file as text.*/

struct Location
{
	std::string path;
	std::vector<std::string> methods;
	std::string root;
	std::string index;
	bool autoindex;
	std::string upload_path;
	std::map<std::string, std::string> cgi_info; // ext -> executable_path

	Location() : autoindex(false) {} // Default is usually 'off'
};

/*Server-Level Elements
listen / host: Crucial for the socket(), bind(), and listen() functions.

server_name: Used if you want to host multiple websites on the same port (Virtual Hosting).

error_page: A mapping of status codes (e.g., 404, 500) to internal file paths.

client_max_body_size: A safety check. If a POST request comes in with a Content-Length larger than this,
the server should immediately return 413 Payload Too Large.
*/

struct ServerConfig
{
	int port;
	std::string host;
	std::vector<std::string> server_names;
	std::map<int, std::string> error_pages;
	size_t max_body_size;
	std::vector<Location> locations;

	// Constructor to set defaults
	ServerConfig() : port(8080), host("127.0.0.1"), max_body_size(1000000) {}
};

#endif
