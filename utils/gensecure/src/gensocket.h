/**************************************************************************
GenSecure - a generic client/server example of TLS support for a connection
(C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
Published under GNU GPL conditions
***************************************************************************/

/* gensocket: generic socket and connection functions */

#ifndef GENSOCKET_H
#define GENSOCKET_H

#include <sys/types.h>

/* Server only: open a connection on the specified port */
int socket_accept(int port);
/* Client only: connect to the given location */
int socket_connect(const char *host, int port);
/* Finish a connection */
void socket_close();
/* Write some bytes */
int output(const char *s);
/* Read some bytes */
int input(char *buffer, int size);

/* Internal: create a socket */
int socket_create_arg(const char *protocol);
/* Internal: wait for socket to come ready */
int socket_read_arg(int sock);
/* Internal: close socket */
void socket_close_arg(int in_accept);
/* Internal: display an error and quit if requested */
void socket_error(int quit, const char *file, int line);

/* Return the active file descriptor */
int socket_fd();

#define GENSOCKERROR(x) socket_error(x, __FILE__, __LINE__)

#endif

