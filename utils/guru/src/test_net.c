#include "net.h"
#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char *argv[])
{
	Guru *guru;
	void *handle;
	Gurucore core;

	if((handle = dlopen("/usr/local/lib/libguru_netggz.so", RTLD_NOW)) == NULL)
	{
		printf("ERROR: Not a shared library\n");
		exit(-1);
	}
	if(((core.net_connect = dlsym(handle, "net_connect")) == NULL)
	|| ((core.net_join = dlsym(handle, "net_join")) == NULL)
	|| ((core.net_status = dlsym(handle, "net_status")) == NULL)
	|| ((core.net_input = dlsym(handle, "net_input")) == NULL)
	|| ((core.net_output = dlsym(handle, "net_output")) == NULL))
	{
		printf("ERROR: Couldn't find net functions\n");
		exit(-1);
	}

	(core.net_connect)("localhost", 5688);
	while(1)
	{
		switch((core.net_status)())
		{
			case NET_ERROR:
				printf("ERROR: Couldn't connect\n");
				exit(-1);
				break;
			case NET_LOGIN:
				printf("Logged in.\n");
				(core.net_join)(0);
				break;
			case NET_GOTREADY:
				printf("Ready.\n");
				break;
			case NET_INPUT:
				guru = (core.net_input)();
				printf("Received: %s\n", guru->message);
				(core.net_output)(guru);
				break;
		}
	}
}

