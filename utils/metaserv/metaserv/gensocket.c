/**************************************************************************
GenSecure - a generic client/server example of TLS support for a connection
(C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
Published under GNU GPL conditions
***************************************************************************/
 
/* gensocket: generic socket and connection functions */

#include "gensocket.h"
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/* The socket used by server and client */
int _socket;
/* Connection file descriptor */
int _accept;
/* Saved port */
int _port;

void socket_error(int quit, const char *file, int line)
{
	printf("*** ERROR! ***\n");
	printf("(%s)\n", strerror(errno));
	printf("(in %s, line %i)\n", file, line);
	printf("**************\n");
	if(quit) exit(-1);
}

int socket_create_arg(const char *protocol)
{
	struct protoent *proto;
	int sock;

	proto = getprotobyname(protocol);
	endprotoent();
	if(proto == NULL)
	{
		printf("tcp not supported!\n");
		GENSOCKERROR(1);
	}
	printf("OK: The tcp number is: %i\n", proto->p_proto);
	sock = socket(AF_INET, SOCK_STREAM, proto->p_proto);
	if(sock == -1)
	{
		printf("Couldn't create socket...\n");
		GENSOCKERROR(1);
	}
	printf("OK: Socket is %i\n", sock);

	_socket = sock;
	return sock;
}

int socket_accept(int port)
{
	int binder;
	int newport;
	struct sockaddr_in sa;

	if(port != _port)
	{
		socket_create_arg("tcp");

		_port = port;
		sa.sin_family = AF_INET;
		sa.sin_addr.s_addr = htonl(INADDR_ANY);
		sa.sin_port = htons(port);
		binder = bind(_socket, (const struct sockaddr*)&sa, sizeof(sa));
		if(binder != 0)
		{
			printf("Couldn't bind...\n");
			GENSOCKERROR(1);
		}
		newport = ntohs(sa.sin_port);
		printf("OK: Right now we are connected to port %i!\n", newport);
	}

	_accept = socket_read_arg(_socket);
	return _accept;
}

int socket_connect(const char *host, int port)
{
	struct sockaddr_in sa;
	struct hostent *name;

	socket_create_arg("tcp");

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);

	name = gethostbyname(host);
	if(!name)
	{
		printf("Host not found!\n");
		GENSOCKERROR(1);
	}
	memcpy(&sa.sin_addr, name->h_addr, name->h_length);
	_accept = connect(_socket, (const struct sockaddr*)&sa, sizeof(sa));
	if(_accept != 0)
	{
		printf("Connect invoked error!\n");
		GENSOCKERROR(1);
	}
	printf("OK: Right now we are connected to host %s, port %i!\n", host, port);

	_accept = _socket;
	return _accept;
}

int socket_read_arg(int sock)
{
	struct sockaddr_in newsa;
	int *size_accept;
	int in, in_accept;

	in = listen(sock, 4);
	if(in == -1)
	{
		printf("Error while listening!\n");
		GENSOCKERROR(1);
	}
	if(in == 0)
	{
		newsa.sin_family = AF_INET;
		newsa.sin_addr.s_addr = htonl(INADDR_ANY);
		newsa.sin_port = 7025;
		size_accept = (int*)sizeof(newsa);
		in_accept = accept(sock, (struct sockaddr*)&newsa, size_accept);
		if(in_accept == -1)
		{
			printf("Accept invoked error!\n");
			GENSOCKERROR(1);
		}
		return in_accept;
	}
	return -1;
}

void socket_close()
{
	socket_close_arg(_accept);
}

void socket_close_arg(int in_accept)
{
	close(in_accept);
}

int output(const char *s)
{
	printf("(debug: write %s)\n", s);
	return write(_accept, s, strlen(s) + 1);
}

int input(char *buffer, int size)
{
	ssize_t ret;

	ret = read(_accept, buffer, size);
	if(ret == -1)
	{
		if(errno != EAGAIN)
		{
			printf("An error occured while reading fd %i!\n", _accept);
			GENSOCKERROR(1);
		}
	}
	if(ret > 0) buffer[strlen(buffer)] = 0;
	return ret;
	/*printf("(debug: read %s)\n", buffer);*/
}

int socket_fd()
{
	return _accept;
}
