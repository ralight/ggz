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

	(core->net_connect)(core->host, 5688, core->name, core->guestname, core->logfile);
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
				guru = guru_work(guru);
				if(guru)
				{
					printf("Answer is: %s\n", guru->message);
					if(core->i18n_translate) guru->message = (core->i18n_translate)(guru->message);
					printf("Translated-Answer is: %s\n", guru->message);
					(core->net_output)(guru);
				}
				break;
		}
	}

	guru_close();
}

