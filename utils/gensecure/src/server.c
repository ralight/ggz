/**************************************************************************
GenSecure - a generic client/server example of TLS support for a connection
(C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
Published under GNU GPL conditions
***************************************************************************/

/* server: an example server for gensecure. */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "gensocket.h"
#include "tls.h"
#include "configuration.h"

/* Macros to be used as BIO emulation */
#define INPUT(x, y) (tls_active() ? tls_input(x, y) : input(x, y))
#define OUTPUT(x) (tls_active() ? tls_output(x) : output(x))

int passwordcallback(char *buf, int size, int rwflag, void *userdata)
{
	strncpy(buf, GENSECURE_PASSWORD, size);
	return strlen(GENSECURE_PASSWORD);
}

int main(int argc, char *argv[])
{
	char tmpbuf[4096];
	int bytes;
	int tls_req;

	tls_req = 1;
	if((argc == 2) && (!strcmp(argv[1], "-notls"))) tls_req = 0;

	printf("> Starting generic server...\n");

	while(1)
	{
		socket_accept(GENSECURE_PORT);
		printf("Generic server: waiting for input\n");

		if(tls_req)
		{
			printf("> Securing connection...\n");
			tls_prepare(GENSECURE_CERTIFICATE, GENSECURE_KEY, passwordcallback);
			tls_start(socket_fd(), TLS_SERVER, TLS_NOVERIFY);
		}

		printf("> Server in working state.\n");
		if(tls_active()) printf("> TLS IN USE.\n");

		fcntl(socket_fd(), F_SETFL, O_NONBLOCK);

		for(;;)
		{
			bytes = INPUT(tmpbuf, sizeof(tmpbuf));
			if(bytes > 0)
			{
				if(!strcmp(tmpbuf, "quit"))
				{
					OUTPUT("Bye.");
					tls_finish();
					/*socket_close();*/
					break;
				}
				printf("Received: %s\n", tmpbuf);
			}
		}
	}

	printf("> Server quit.\n");
	
	return 0;
}

