#include "net.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	net_connect("localhost", 5688);
	while(1)
	{
		switch(net_status())
		{
			case NET_ERROR:
				printf("ERROR: Couldn't connect\n");
				exit(-1);
				break;
			case NET_LOGIN:
				printf("Logged in.\n");
				net_join(0);
				break;
			case NET_GOTREADY:
				printf("Ready.\n");
				break;
		}
	}
}

