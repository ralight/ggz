/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "guru.h"
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int admin(Guru *guru, Gurucore *core)
{
	int i;
	int room;

	if(!guru) return 0;
	if(!guru->message) return 0;
	if(!core->owner) return 0;
	if(strcmp(guru->player, core->owner)) return 0;

	i = 0;
	while((guru->list) && (guru->list[i])) i++;

	printf("*** Admin: got request: %i words\n", i);
	switch(i)
	{
		case 3:
			if(!strcmp(guru->list[1], "goto"))
			{
				room = atoi(guru->list[2]);
				printf("*** Admin: Goto room %i\n", room);
				guru->message = strdup("Yes, I follow, my master.");
				(core->net_output)(guru);
				sleep(2);
				(core->net_join)(room);
				return 1;
			}
			if(!strcmp(guru->list[1], "logging"))
			{
				printf("*** Admin: Logging %s\n", guru->list[2]);
				if(!strcmp(guru->list[2], "off")) (core->net_log)(NULL);
				else (core->net_log)(core->logfile);
				guru->message = strdup("Toggled logging.");
				(core->net_output)(guru);
				return 1;
			}
			break;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	Gurucore *core;
	Guru *guru;

	printf("Grubby: initializing...\n");
	core = guru_init();
	if(!core)
	{
		printf("Grubby initialization failed!\n");
		exit(-1);
	}
	printf("Grubby: connect...\n");

	(core->net_log)(core->logfile);
	(core->net_connect)(core->host, 5688, core->name, core->guestname);
	if(core->i18n_init) (core->i18n_init)();
	while(1)
	{
		switch((core->net_status)())
		{
			case NET_ERROR:
				printf("ERROR: Couldn't connect\n");
				exit(-1);
				break;
			case NET_LOGIN:
				printf("Grubby: Logged in.\n");
				(core->net_join)(core->autojoin);
				break;
			case NET_GOTREADY:
				printf("Grubby: Ready.\n");
				break;
			case NET_INPUT:
				guru = (core->net_input)();
				/*printf("Received: %s\n", guru->message);*/
				if(!admin(guru, core))
				{
					guru = guru_work(guru);
					if(guru)
					{
						printf("Answer is: %s\n", guru->message);
						if(core->i18n_translate) guru->message = (core->i18n_translate)(guru->message);
						printf("Translated-Answer is: %s\n", guru->message);
						(core->net_output)(guru);
					}
				}
				break;
		}
	}

	guru_close();
}

