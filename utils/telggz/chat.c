#include "net.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define BUFSIZE 100

char **chat_list(char *buffer)
{
	static char **list = NULL;
	int i;
	char *token;

	if(list)
	{
		i = 0;
		while(list[i])
		{
			free(list[i]);
			i++;
		}
		free(list);
		list = NULL;
	}

	i = 1;
	token = strtok(buffer, " ,:");
	while(token)
	{
		i++;
		list = (char**)realloc(list, i * sizeof(char*));
		list[i - 2] = (char*)malloc(strlen(token) + 1);
		strcpy(list[i - 2], token);
		list[i - 1] = NULL;
		token = strtok(NULL, " ,:");
	}

	return list;
}

void chat_loop()
{
	char buffer[BUFSIZE];
	int ret;
	char **list;

	fcntl(0, F_SETFL, O_NONBLOCK);

	net_init();

	while(1)
	{
		net_process();

		errno = 0;
		ret = read(0, buffer, sizeof(buffer));
		if(ret > 0)
		{
			if((ret > 0) && (buffer[ret - 1] == '\n')) buffer[ret - 1] = 0;
			if((ret > 1) && (buffer[ret - 2] == '\r')) buffer[ret - 2] = 0;
			net_allow(1);
			if(!strcmp(buffer, "")) net_allow(0);
			else if(buffer[0] == '/')
			{
				list = chat_list(buffer);
				if((list) && (list[0]))
				{
					if(!strcmp(list[0], "/login"))
					{
						if(list[1]) net_login(list[1], list[2]);
					}
					if((!strcmp(list[0], "/quit"))
					|| (!strcmp(list[0], "/exit")))
					{
						exit(0);
					}
					if((!strcmp(list[0], "/help"))
					|| (!strcmp(list[0], "/?")))
					{
						printf("TelGGZ - The GGZ Gaming Zone Telnet Wrapper.\n");
						printf("Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net\n");
						printf("Published under GNU GPL conditions\n");
						printf("\n");
						printf("Available commands:\n");
						printf("/host <hostname> - Connect to the given host (default: localhost)\n");
						printf("/login <nick> [<password>] - Log into the selected server\n");
						printf("/join <roomid> - Join the room with the given id\n");
						printf("/help - This dialog (also /?)\n");
						printf("/quit - Quit the chat (also /exit)\n");
						printf("\n");
						printf("Before starting to enter a line, press Enter once to prevent your line\n");
						printf("from being overwritten.\n");
						fflush(NULL);
					}
					if(!strcmp(list[0], "/join"))
					{
						if(list[1]) net_join(atoi(list[1]));
					}
					if(!strcmp(list[0], "/host"))
					{
						if(list[1]) net_host(list[1]);
					}
				}
			}
			else net_send(buffer);
		}
		else if(ret < 0)
		{
			if(errno != EAGAIN)
			{
				printf("TelGGZ: ERROR! Input error.\n");
				fflush(NULL);
				exit(-1);
			}
		}
		else if(ret == 0)
		{
			printf("TelGGZ: ERROR! Input error noop.\n");
			fflush(NULL);
			exit(-1);
		}
	}
}

