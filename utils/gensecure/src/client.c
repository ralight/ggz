/**************************************************************************
GenSecure - a generic client/server example of TLS support for a connection
(C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
Published under GNU GPL conditions
***************************************************************************/

/* client: an example client for gensecure. */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "gensocket.h"
#include "tls.h"
#include "configuration.h"

/* Macros to be used as BIO emulation */
#define INPUT(x, y) (tls_active(socket_fd()) ? tls_read(socket_fd(), x, y) : input(x, y))
#define OUTPUT(x) (tls_active(socket_fd()) ? tls_write(socket_fd(), x, strlen(x)) : output(x))

int main(int argc, char *argv[])
{
	char tmpbuf[4096];
	int bytes;
	int tls_req;

	tls_req = 1;
	if((argc == 2) && (!strcmp(argv[1], "-notls"))) tls_req = 0;

	printf("> Starting generic client...\n");

	socket_connect(GENSECURE_HOST, GENSECURE_PORT);
	printf("Generic client: connected\n");

	if(tls_req)
	{
		printf("> Securing connection...\n");
		tls_start(socket_fd(), TLS_CLIENT, /*TLS_NOVERIFY*/ TLS_NOVERIFY);
	}

	printf("> Client in working state.\n");
	if(tls_active(socket_fd())) printf("> TLS IN USE.\n");

	fcntl(0, F_SETFL, O_NONBLOCK);
	fcntl(socket_fd(), F_SETFL, O_NONBLOCK);

	for(;;)
	{
		bytes = INPUT(tmpbuf, sizeof(tmpbuf));
		if(bytes > 0)
		{
			if(!strcmp(tmpbuf, "Bye."))
			{
				OUTPUT("Quitting.");
				tls_finish(socket_fd());
				socket_close();
				break;
			}
			printf("Received: %s\n", tmpbuf);
		}
		
		bytes = read(0, tmpbuf, sizeof(tmpbuf));
		if(bytes > 0)
		{
			tmpbuf[bytes - 1] = 0;
			OUTPUT(tmpbuf);
		}
	}

	printf("> Client quit.\n");

	return 0;
}

