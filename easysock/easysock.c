/*
 * File: easysock.c
 * Author: Brent Hendricks
 * Project: libeasysock
 * Date: 4/16/98
 *
 * A library of useful routines to make life easier while using 
 * sockets
 *
 * Copyright (C) 1998 Brent Hendricks.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */


#ifdef DEBUG_MEM
# include <dmalloc.h>
#endif

#include <config.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include <easysock.h>


static es_err_func _err_func = NULL;
static es_exit_func _exit_func = exit;

static void _debug(const char *fmt, ...)
{
#ifdef DEBUG_SOCKET
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
#endif
}


int es_err_func_set(es_err_func func)
{
	_err_func = func;
	return 0;
}


es_err_func es_err_func_rem(void)
{
	es_err_func old = _err_func;
	_err_func = NULL;
	return old;
}


int es_exit_func_set(es_exit_func func)
{
	_exit_func = func;
	return 0;
}


es_exit_func es_exit_func_rem(void)
{
	es_exit_func old = _exit_func;
	_exit_func = exit;
	return old;
}


int es_make_socket(const EsSockType type, const unsigned short port, 
		   const char *server)
{
	int sock;
	const int on = 1;
	struct sockaddr_in name;
	struct hostent *hp;

	if ( (sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		if (_err_func)
			(*_err_func) (strerror(errno), ES_CREATE, ES_NONE);
		return -1;
	}

	name.sin_family = AF_INET;
	name.sin_port = htons(port);

	switch (type) {

	case ES_SERVER:
		name.sin_addr.s_addr = htonl(INADDR_ANY);
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, 
			       sizeof(on)) < 0
		    || bind(sock, (struct sockaddr *)&name, 
			    sizeof(name)) < 0) {
			if (_err_func)
				(*_err_func) (strerror(errno), ES_CREATE, 
					      ES_NONE);
			return -1;
		}
		break;

	case ES_CLIENT:
		if ( (hp = gethostbyname(server)) == NULL) {
			if (_err_func)
				(*_err_func) (strerror(errno), ES_CREATE, 
					      ES_NONE);
			return -1;
			break;
		}
		memcpy(&name.sin_addr, hp->h_addr, hp->h_length);
		if (connect(sock, (struct sockaddr *)&name, sizeof(name))< 0) {
			if (_err_func)
				(*_err_func) (strerror(errno), ES_CREATE, 
					      ES_NONE);
			return -1;
		}
		break;
	}

	return sock;
}


int es_make_socket_or_die(const EsSockType type, const unsigned short port, 
			  const char *server)
{
	int sock;
	
	if ( (sock = es_make_socket(type, port, server)) < 0)
		(*_exit_func) (-1);
	
	return sock;
}


int es_write_char(const int sock, const char message)
{

	int status;

	status = write(sock, &message, sizeof(char));
	status = (status == 0 ? -1 : status);

	if (status < 0) {
		_debug("[%d]: Error sending char\n", getpid());
		if (_err_func != NULL)
			(*_err_func) (strerror(errno), ES_WRITE, ES_CHAR);
	} else
		_debug("[%d]: Sent \"%d\" : %d char\n", getpid(), message,
		       status);

	return status;
}


void es_write_char_or_die(const int sock, const char data)
{
	if (es_write_char(sock, data) < 0)
		(*_exit_func) (-1);
}


int es_read_char(const int sock, char *message)
{

	int status;

	status = read(sock, message, sizeof(char));

	if (status < 0) {
		_debug("[%d]: Error receiving char\n", getpid());
		if (_err_func != NULL)
			(*_err_func) (strerror(errno), ES_READ, ES_CHAR);
	} else if (status == 0) {
		status = -1;
		_debug("[%d]: Warning: fd is closed\n", getpid());
		if (_err_func != NULL)
			(*_err_func) ("Socket closed", ES_READ, ES_CHAR);
	} else
		_debug("[%d]: Received \"%d\" : %d char \n", getpid(),
		       *message, status);

	return status;
}


void es_read_char_or_die(const int sock, char *data)
{
	if (es_read_char(sock, data) <= 0)
		(*_exit_func) (-1);
}


/*
 * Take a host-byte order int and write on fd using
 * network-byte order.
 */
int es_write_int(const int sock, const int message)
{

	int status, data = htonl(message);

	status = write(sock, &data, sizeof(int));
	status = (status == 0 ? -1 : status);

	if (status < 0) {
		_debug("[%d]: Error sending int\n", getpid());
		if (_err_func != NULL)
			(*_err_func) (strerror(errno), ES_WRITE, ES_INT);
	} else
		_debug("[%d]: Sent \"%d\" : %d bytes\n", getpid(), message,
		       status);

	return status;
}


void es_write_int_or_die(const int sock, const int data)
{
	if (es_write_int(sock, data) < 0)
		(*_exit_func) (-1);
}


/*
 * Read a network byte-order integer from the fd and
 * store it in host-byte order.
 */
int es_read_int(const int sock, int *message)
{

	int data, status;

	status = read(sock, &data, sizeof(int));

	*message = ntohl(data);

	if (status < 0) {
		_debug("[%d]: Error receiving int\n", getpid());
		if (_err_func != NULL)
			(*_err_func) (strerror(errno), ES_READ, ES_INT);
	} else if (status == 0) {
		_debug("[%d]: Warning: fd is closed\n", getpid());
		status = -1;
		if (_err_func != NULL)
			(*_err_func) ("Socket closed", ES_READ, ES_INT);
	} else
		_debug("[%d]: Received \"%d\" : %d bytes \n", getpid(),
		       *message, status);

	return status;
}


void es_read_int_or_die(const int sock, int *data)
{
	if (es_read_int(sock, data) <= 0)
		(*_exit_func) (-1);
}


/*
 * Write a char string to the given fd preceeded by its size
 */
int es_write_string(const int sock, const char *message)
{

	unsigned int size = strlen(message) * sizeof(char) + 1;
	int status;

	status = es_write_int(sock, size);

	if (status > 0) {

		status = write(sock, message, size);
		status = (status == 0 ? -1 : status);

		if (status < 0) {
			_debug("[%d]: Error sending string\n");
			if (_err_func != NULL)
				(*_err_func) (strerror(errno), ES_WRITE, ES_STRING);
		} else
			_debug("[%d]: Sent \"%s\" : %d bytes \n", getpid(),
			       message, status);

	}

	return status;

}


int es_va_write_string(const int sock, const char *fmt, ...)
{

	char buf[4096];
	va_list ap;

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);

	return es_write_string(sock, buf);

}


void es_write_string_or_die(const int sock, const char *data)
{
	if (es_write_string(sock, data) < 0)
		(*_exit_func) (-1);
}


void es_va_write_string_or_die(const int sock, const char *fmt, ...)
{

	char buf[4096];
	va_list ap;

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);

	es_write_string_or_die(sock, buf);
}


/*
 * Read a char string from the given fd
 */
int es_read_string(const int sock, char *message)
{

	int size, status;

	status = es_read_int(sock, &size);

	if (status > 0) {

		status = read(sock, message, size);

		if (status < 0) {
			_debug("[%d]: Error receiving string\n", getpid());
			if (_err_func != NULL)
				(*_err_func) (strerror(errno), ES_READ, ES_STRING);
		} else if (status == 0) {
			status = -1;
			_debug("[%d]: Warning: fd is closed\n", getpid());

		} else
			_debug("[%d]: Received \"%s\" : %d bytes \n",
			       getpid(), *message, status);

	}

	return status;
}


void es_read_string_or_die(const int sock, char *data)
{
	if (es_read_string(sock, data) <= 0)
		(*_exit_func) (-1);
}


/*
 * Read a char string from the given fd and allocate space for it.
 */
int es_read_string_alloc(const int sock, char **message)
{

	unsigned int size;
	int status;

	status = es_read_int(sock, &size);

	if (status > 0) {

		*message = (char *) malloc(size * sizeof(char));

		if (*message == NULL) {
			status = -1;
			_debug("[%d]: Error: Not enough memory\n",
			       getpid());
			if (_err_func != NULL)
				(*_err_func) (strerror(errno), ES_ALLOCATE,
					      ES_STRING);
		} else {
			memset(*message, 0, size);
			status = read(sock, *message, size);

			if (status < 0) {
				_debug("[%d]: Error receiving string\n",
				       getpid());
				if (_err_func != NULL)
					(*_err_func) (strerror(errno),
						      ES_READ, ES_STRING);
			} else if (status == 0) {
				status = -1;
				_debug("[%d]: Warning: fd is closed\n",
				       getpid());
				if (_err_func != NULL)
					(*_err_func) ("Socket closed",
						      ES_READ, ES_STRING);
			} else
				_debug
				    ("[%d]: Received \"%s\" : %d bytes \n",
				     getpid(), *message, status);

		}		/* else not out of memory */
	}
	/* if status >0  */
	return status;
}


void es_read_string_alloc_or_die(const int sock, char **data)
{
	if (es_read_string_alloc(sock, data) <= 0)
		(*_exit_func) (-1);
}


/* Write "n" bytes to a descriptor. */
int es_writen(int fd, const void *vptr, size_t n)
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
	return (n);
}
/* end writen */


/* Read "n" bytes from a descriptor. */
int es_readn(int fd, void *vptr, size_t n)
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
	return (n - nleft);	/* return >= 0 */
}
/* end readn */
