/*
 * File: easysock.c
 * Author: Brent Hendricks
 * Project: libeasysock
 * Date: 4/16/98
 *
 * A library of useful routines to make life easier while using 
 * sockets
 *
 * Copyright (C) 1998-2001 Brent Hendricks.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#ifdef DEBUG_MEM
# include <dmalloc.h>
#endif

#include <config.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/param.h>

#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>

#include "ggz.h"

#define SA struct sockaddr  

static ggzIOError _err_func = NULL;
static ggzIOExit _exit_func = exit;
static unsigned int ggz_alloc_limit = 32768;

static void _debug(const char *fmt, ...)
{
#ifdef DEBUG_SOCKET
	char buf[4096];
	va_list ap;

	va_start(ap, fmt);
	sprintf(buf, "[%d]: ", getpid());
	vsprintf(buf + strlen(buf), fmt, ap);
	
	fputs(buf, stderr);
	va_end(ap);
#endif
}


int ggz_set_io_error_func(ggzIOError func)
{
	_err_func = func;
	return 0;
}


ggzIOError ggz_remove_io_error_func(void)
{
	ggzIOError old = _err_func;
	_err_func = NULL;
	return old;
}


int ggz_set_io_exit_func(ggzIOExit func)
{
	_exit_func = func;
	return 0;
}


ggzIOExit ggz_remove_exit_func(void)
{
	ggzIOExit old = _exit_func;
	_exit_func = exit;
	return old;
}


unsigned int ggz_get_io_alloc_limit(void)
{
	return ggz_alloc_limit;
}


unsigned int ggz_set_io_alloc_limit(const unsigned int limit)
{
	unsigned int old = ggz_alloc_limit;
	ggz_alloc_limit = limit;
	return old;
}


int ggz_make_socket(const GGZSockType type, const unsigned short port, 
		   const char *server)
{
	int sock;
	const int on = 1;
	struct sockaddr_in name;
	struct hostent *hp;

	if ( (sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		if (_err_func)
			(*_err_func) (strerror(errno), GGZ_IO_CREATE, GGZ_DATA_NONE);
		return -1;
	}

	name.sin_family = AF_INET;
	name.sin_port = htons(port);

	switch (type) {

	case GGZ_SOCK_SERVER:
		name.sin_addr.s_addr = htonl(INADDR_ANY);
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*)&on, 
			       sizeof(on)) < 0
		    || bind(sock, (SA *)&name, sizeof(name)) < 0) {
			if (_err_func)
				(*_err_func) (strerror(errno), GGZ_IO_CREATE, 
					      GGZ_DATA_NONE);
			return -1;
		}
		break;

	case GGZ_SOCK_CLIENT:
		if ( (hp = gethostbyname(server)) == NULL) {
			if (_err_func)
				(*_err_func) ("Lookup failure", GGZ_IO_CREATE, 
					      GGZ_DATA_NONE);
			return -1;
			break;
		}
		memcpy(&name.sin_addr, hp->h_addr, hp->h_length);
		if (connect(sock, (SA *)&name, sizeof(name)) < 0) {
			if (_err_func)
				(*_err_func) (strerror(errno), GGZ_IO_CREATE, 
					      GGZ_DATA_NONE);
			return -1;
		}
		break;
	}

	return sock;
}


int ggz_make_socket_or_die(const GGZSockType type, const unsigned short port, 
			  const char *server)
{
	int sock;
	
	if ( (sock = ggz_make_socket(type, port, server)) < 0)
		(*_exit_func) (-1);
	
	return sock;
}


int ggz_make_unix_socket(const GGZSockType type, const char* name) 
{
	int sock;
	struct sockaddr_un addr;
	
	if ( (sock = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0) {
		if (_err_func)
			(*_err_func) (strerror(errno), GGZ_IO_CREATE, GGZ_DATA_NONE);
		return -1;
	}

	
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	/* Copy in filename up to the limit, leaving room for \0 term. */
	strncpy(addr.sun_path, name, (sizeof(addr.sun_path) - 1));

	switch(type) {

	case GGZ_SOCK_SERVER:
		unlink(name);
		if (bind(sock, (SA *)&addr, SUN_LEN(&addr)) < 0 ) {
			if (_err_func)
				(*_err_func) (strerror(errno), GGZ_IO_CREATE, 
					      GGZ_DATA_NONE);
			return -1;
		}
		break;
	case GGZ_SOCK_CLIENT:
		if (connect(sock, (SA *)&addr, sizeof(addr)) < 0) {
			if (_err_func)
				(*_err_func) (strerror(errno), GGZ_IO_CREATE, 
					      GGZ_DATA_NONE);
			return -1;
		}
		break;
	}
	return sock;
}


int ggz_make_unix_socket_or_die(const GGZSockType type, const char* name) 
{
	int sock;
	
	if ( (sock = ggz_make_unix_socket(type, name)) < 0)
		(*_exit_func) (-1);
	
	return sock;
}


int ggz_write_char(const int sock, const char message)
{
	if (ggz_writen(sock, &message, sizeof(char)) < 0) {
		_debug("Error sending char\n");
		if (_err_func)
			(*_err_func) (strerror(errno), GGZ_IO_WRITE, GGZ_DATA_CHAR);
		return -1;
	}

	_debug("Sent \"%d\" : char\n", message);
	return 0;
}


void ggz_write_char_or_die(const int sock, const char data)
{
	if (ggz_write_char(sock, data) < 0)
		(*_exit_func) (-1);
}


int ggz_read_char(const int sock, char *message)
{
	int status;

	if ( (status = ggz_readn(sock, message, sizeof(char))) < 0) {
		_debug("Error receiving char\n");
		if (_err_func)
			(*_err_func) (strerror(errno), GGZ_IO_READ, GGZ_DATA_CHAR);
		return -1;
	} 

	if (status < sizeof(char)) {
		_debug("Warning: fd is closed\n");
		if (_err_func)
			(*_err_func) ("fd closed", GGZ_IO_READ, GGZ_DATA_CHAR);
		return -1;
	} 
	
	_debug("Received \"%d\" : char\n", *message);
	return 0;
}


void ggz_read_char_or_die(const int sock, char *data)
{
	if (ggz_read_char(sock, data) < 0)
		(*_exit_func) (-1);
}


/*
 * Take a host-byte order int and write on fd using
 * network-byte order.
 */
int ggz_write_int(const int sock, const int message)
{
	int data = htonl(message);

	if (ggz_writen(sock, &data, sizeof(int)) < 0) {
		_debug("Error sending int\n");
		if (_err_func)
			(*_err_func) (strerror(errno), GGZ_IO_WRITE, GGZ_DATA_INT);
		return -1;
	}

	_debug("Sent \"%d\" : int\n", message);
	return 0;
}


void ggz_write_int_or_die(const int sock, const int data)
{
	if (ggz_write_int(sock, data) < 0)
		(*_exit_func) (-1);
}


/*
 * Read a network byte-order integer from the fd and
 * store it in host-byte order.
 */
int ggz_read_int(const int sock, int *message)
{
	int data, status;
	
	if ( (status = ggz_readn(sock, &data, sizeof(int))) < 0) {
		_debug("Error receiving int\n");
		if (_err_func)
			(*_err_func) (strerror(errno), GGZ_IO_READ, GGZ_DATA_INT);
		return -1;
	}
	
	if (status < sizeof(int)) {
		_debug("Warning: fd is closed\n");
		if (_err_func)
			(*_err_func) ("fd closed", GGZ_IO_READ, GGZ_DATA_INT);
		return -1;
	}
	
	*message = ntohl(data);
	_debug("Received \"%d\" : int\n", *message);
	return 0;
}


void ggz_read_int_or_die(const int sock, int *data)
{
	if (ggz_read_int(sock, data) < 0)
		(*_exit_func) (-1);
}


/*
 * Write a char string to the given fd preceeded by its size
 */
int ggz_write_string(const int sock, const char *message)
{
	unsigned int size = strlen(message) * sizeof(char) + 1;
	
	if (ggz_write_int(sock, size) < 0)
		return -1;
	
	if (ggz_writen(sock, message, size) < 0) {
		_debug("Error sending string\n");
		if (_err_func)
			(*_err_func) (strerror(errno), GGZ_IO_WRITE, GGZ_DATA_STRING);
		return -1;
	}
	
	_debug("Sent \"%s\" : string \n", message);
	return 0;
}


int ggz_va_write_string(const int sock, const char *fmt, ...)
{
	char buf[4096];
	va_list ap;

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);

	return ggz_write_string(sock, buf);
}


void ggz_write_string_or_die(const int sock, const char *data)
{
	if (ggz_write_string(sock, data) < 0)
		(*_exit_func) (-1);
}


void ggz_va_write_string_or_die(const int sock, const char *fmt, ...)
{
	char buf[4096];
	va_list ap;

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);

	ggz_write_string_or_die(sock, buf);
}


/*
 * Read a char string from the given fd
 */
int ggz_read_string(const int sock, char *message, const unsigned int len)
{
	unsigned int size;
	int status;

	if (ggz_read_int(sock, &size) < 0)
		return -1;
	
	if (size > len) {
		_debug("String too long for buffer\n");
		if (_err_func)
			(*_err_func) ("String too long", GGZ_IO_READ, GGZ_DATA_STRING);
		return -1;
	}
	
       	if ( (status = ggz_readn(sock, message, size)) < 0) {
		_debug("Error receiving string\n");
		if (_err_func)
			(*_err_func) (strerror(errno), GGZ_IO_READ, GGZ_DATA_STRING);
		return -1;
	} 

	if (status < size) {
		_debug("Warning: fd is closed\n");
		if (_err_func)
			(*_err_func) ("fd closed", GGZ_IO_READ, GGZ_DATA_STRING);
		return -1;
	} 
	
	/* Guarantee NULL-termination */
	message[len-1] = '\0';
	
	_debug("Received \"%s\" : string\n", message);
	return 0;
}


void ggz_read_string_or_die(const int sock, char *data, const unsigned int len)
{
	if (ggz_read_string(sock, data, len) < 0)
		(*_exit_func) (-1);
}


/*
 * Read a char string from the given fd and allocate space for it.
 */
int ggz_read_string_alloc(const int sock, char **message)
{
	unsigned int size;
	int status;

	if (ggz_read_int(sock, &size) < 0)
		return -1;

	if (size > ggz_alloc_limit) {
		_debug("Error: Easysock allocation limit exceeded\n");
		if (_err_func)
			(*_err_func) ("Allocation limit exceeded", GGZ_IO_ALLOCATE,
				      GGZ_DATA_STRING);
		return -1;
	}
	
	if ( (*message = calloc((size+1), sizeof(char))) == NULL) {
		_debug("Error: Not enough memory\n");
		if (_err_func)
			(*_err_func) (strerror(errno), GGZ_IO_ALLOCATE, GGZ_DATA_STRING);
		return -1;
	}

	if ( (status = ggz_readn(sock, *message, size)) < 0) {
		_debug("Error receiving string\n");
		if (_err_func)
			(*_err_func) (strerror(errno), GGZ_IO_READ, GGZ_DATA_STRING);
		return -1;
	}

	if (status < size) {
		_debug("Warning: fd is closed\n");
		if (_err_func)
			(*_err_func) ("fd closed", GGZ_IO_READ, GGZ_DATA_STRING);
		return -1;
	} 

	_debug("Received \"%s\" : string\n", *message);
	return 0;
}


void ggz_read_string_alloc_or_die(const int sock, char **data)
{
	if (ggz_read_string_alloc(sock, data) < 0)
		(*_exit_func) (-1);
}


/* Write "n" bytes to a descriptor. */
int ggz_writen(int fd, const void *vptr, size_t n)
{

	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nwritten = write(fd, ptr, nleft)) <= 0) {
			if (errno == EINTR)
				nwritten = 0;	/* and call write() again */
			else
				return (-1);	/* error */
		}
		
		nleft -= nwritten;
		ptr += nwritten;
	}
	_debug("Wrote %d bytes\n", n);
	return (n);
}


/* Read "n" bytes from a descriptor. */
int ggz_readn(int fd, void *vptr, size_t n)
{

	size_t nleft;
	ssize_t nread;
	char *ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;	/* and call read() again */
			else
				return (-1);
		} else if (nread == 0)
			break;	/* EOF */

		nleft -= nread;
		ptr += nread;
	}
	_debug("Read %d bytes\n", (n - nleft));
	return (n - nleft);	/* return >= 0 */
}


int ggz_write_fd(int fd, int sendfd)
{
        struct msghdr msg;
	struct iovec iov[1];

#ifdef	HAVE_MSGHDR_MSG_CONTROL
	union {
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr *cmptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);

	cmptr = CMSG_FIRSTHDR(&msg);
	cmptr->cmsg_len = CMSG_LEN(sizeof(int));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;
	*((int *) CMSG_DATA(cmptr)) = sendfd;
#else
	msg.msg_accrights = (caddr_t) &sendfd;
	msg.msg_accrightslen = sizeof(int);
#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

        /* We're just sending a fd, so it's a dummy byte */
        iov[0].iov_base = "";
	iov[0].iov_len = 1;

	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	if (sendmsg(fd, &msg, 0) < 0) {
		_debug("Error sending fd\n");
		if (_err_func)
			(*_err_func) (strerror(errno), GGZ_IO_WRITE, GGZ_DATA_FD);
		return -1;
	}

	_debug("Sent \"%d\" : fd\n", sendfd);
	return 0;
}


int ggz_read_fd(int fd, int *recvfd)
{
	struct msghdr msg;
	struct iovec iov[1];
	ssize_t	n;
        char dummy;
#ifndef HAVE_MSGHDR_MSG_CONTROL
	int newfd;
#endif

  
#ifdef	HAVE_MSGHDR_MSG_CONTROL
	union {
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr *cmptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);
#else
	msg.msg_accrights = (caddr_t) &newfd;
	msg.msg_accrightslen = sizeof(int);
#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

        /* We're just sending a fd, so it's a dummy byte */
	iov[0].iov_base = &dummy;
	iov[0].iov_len = 1;

	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	if ( (n = recvmsg(fd, &msg, 0)) < 0) {
		_debug("Error reading fd msg\n");
		if (_err_func)
			(*_err_func) (strerror(errno), GGZ_IO_READ, GGZ_DATA_FD);
		return -1;
	}

        if (n == 0) {
		_debug("Warning: fd is closed\n");
		if (_err_func)
			(*_err_func) ("fd closed", GGZ_IO_READ, GGZ_DATA_FD);
	        return -1; 
	}
  
#ifdef	HAVE_MSGHDR_MSG_CONTROL
        if ( (cmptr = CMSG_FIRSTHDR(&msg)) == NULL 
	     || cmptr->cmsg_len != CMSG_LEN(sizeof(int))) {
		_debug("Bad cmsg\n");
		if (_err_func)
			(*_err_func) ("Bad cmsg", GGZ_IO_READ, GGZ_DATA_FD);
		return -1;
	}

	if (cmptr->cmsg_level != SOL_SOCKET) {
		_debug("Bad cmsg\n");
		if (_err_func)
			(*_err_func) ("level != SOL_SOCKET", GGZ_IO_READ, GGZ_DATA_FD);
		return -1;
	}

	if (cmptr->cmsg_type != SCM_RIGHTS) {
		_debug("Bad cmsg\n");
		if (_err_func)
			(*_err_func) ("type != SCM_RIGHTS", GGZ_IO_READ, GGZ_DATA_FD);
		return -1;
	}

	/* Everything is good */
	*recvfd = *((int *) CMSG_DATA(cmptr));
#else
	if (msg.msg_accrightslen != sizeof(int)) {
		_debug("Bad msg\n");
		if (_err_func)
			(*_err_func) ("Bad msg", GGZ_IO_READ, GGZ_DATA_FD);
		return -1;
	}		

	/* Everything is good */
	*recvfd = newfd;
#endif
	
	_debug("Received \"%d\" : fd\n", *recvfd);
        return 0;
}

