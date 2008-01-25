/*
 * TelGGZ - The GGZ Gaming Zone Telnet Wrapper
 * Copyright (C) 2001 - 2003 Josef Spillner, dr_maux@users.sourceforge.net

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* Header files */
#include "chat.h"
#include "net.h"

/* System includes */
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

/* Definitions */
#define BUFSIZE 100

/* Functions */

void chat_connect(const char *host, int port, const char *username, const char *password)
{
	net_init();
	net_host(host, port);
	net_login(username, password);
}

static char *chat_input()
{
	char *s;
	int ret;

	s = (char*)malloc(128);
	ret = read(0, s, 128);
	if(ret < 1) return NULL;
	if((ret > 0) && (s[ret - 1] == '\n')) s[ret - 1] = 0;
	if((ret > 1) && (s[ret - 2] == '\r')) s[ret - 2] = 0;
	return s;
}

int chat_getserver()
{
	char *s;
	int ret;
	s = chat_input();
	if(!s) return -1;
	ret = atoi(s);
	free(s);
	return ret;
}

char *chat_getusername()
{
	return chat_input();
}

char *chat_getpassword()
{
	return chat_input();
}

static char **chat_list(char *buffer)
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
	int unknown;

	fcntl(0, F_SETFL, O_NONBLOCK);

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
					unknown = 1;
					if((!strcmp(list[0], "/quit"))
					|| (!strcmp(list[0], "/exit")))
					{
						exit(0);
						unknown = 0;
					}
					if((!strcmp(list[0], "/help"))
					|| (!strcmp(list[0], "/?")))
					{
						printf("TelGGZ - The GGZ Gaming Zone Telnet Wrapper.\n");
						printf("Copyright (C) 2001 - 2003 Josef Spillner, josef@ggzgamingzone.org\n");
						printf("Published under GNU GPL conditions\n");
						printf("\n");
						printf("Available commands:\n");
						printf("/join <roomid> - Join the room with the given id\n");
						printf("/list - Show all rooms\n");
						printf("/who - Show all players\n");
						printf("/help - This dialog (also /?)\n");
						printf("/quit - Quit the chat (also /exit)\n");
						printf("\n");
						printf("Before starting to enter a line, press Enter once to prevent your line\n");
						printf("from being overwritten.\n");
						unknown = 0;
					}
					if(!strcmp(list[0], "/list"))
					{
						net_list();
						unknown = 0;
					}
					if(!strcmp(list[0], "/who"))
					{
						net_who();
						unknown = 0;
					}
					if(!strcmp(list[0], "/join"))
					{
						if(list[1]) net_join(atoi(list[1]));
						else
						{
							printf("Wrong arguments,\n");
						}
						unknown = 0;
					}
					if(unknown)
					{
						printf("Unknown command!\n");
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
				exit(-1);
			}
		}
		else if(ret == 0)
		{
			printf("TelGGZ: ERROR! Input error noop.\n");
			exit(-1);
		}
	}
}

