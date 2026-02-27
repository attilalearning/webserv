/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mosokina <mosokina@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 11:32:29 by mosokina          #+#    #+#             */
/*   Updated: 2026/02/25 11:32:30 by mosokina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <unistd.h>

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
