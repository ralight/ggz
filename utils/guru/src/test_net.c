#include "net.h"
#include <stdio.h>
#include <dlfcn.h>

typedef void (*netconnectfunc)(const char *host, int port);
typedef void (*netjoinfunc)(int room);
typedef int (*netstatusfunc)();
typedef Guru* (*netinputfunc)();
typedef void (*netoutputfunc)(Guru *output);

int main(int argc, char *argv[])
{
	Guru *guru;
	void *handle;
	netconnectfunc net_connect;
	netjoinfunc net_join;
	netstatusfunc net_status;
	netinputfunc net_input;
	netoutputfunc net_output;

	if((handle = dlopen("/usr/local/lib/libguru_netggz.so", RTLD_NOW)) == NULL)
	{
		printf("ERROR: Not a shared library\n");
		exit(-1);
	}
	if(((net_connect = dlsym(handle, "net_connect")) == NULL)
	|| ((net_join = dlsym(handle, "net_join")) == NULL)
	|| ((net_status = dlsym(handle, "net_status")) == NULL)
	|| ((net_input = dlsym(handle, "net_input")) == NULL)
	|| ((net_output = dlsym(handle, "net_output")) == NULL))
	{
		printf("ERROR: Couldn't find net functions\n");
		exit(-1);
	}

	(net_connect)("localhost", 5688);
	while(1)
	{
		switch((net_status)())
		{
			case NET_ERROR:
				printf("ERROR: Couldn't connect\n");
				exit(-1);
				break;
			case NET_LOGIN:
				printf("Logged in.\n");
				(net_join)(0);
				break;
			case NET_GOTREADY:
				printf("Ready.\n");
				break;
			case NET_INPUT:
				guru = (net_input)();
				printf("Received: %s\n", guru->message);
				(net_output)(guru);
				break;
		}
	}
}

