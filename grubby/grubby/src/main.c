/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001, 2002 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "guru.h"
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

/* Execute administrative commands */
/* FIXME: Log admin commands */
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

	switch(i)
	{
		case 0:
		case 1:
		case 2:
			break;
		case 3:
			if(!strcmp(guru->list[1], "goto"))
			{
				room = atoi(guru->list[2]);
				guru->message = strdup("Yes, I follow, my master.");
				(core->net_output)(guru);
				sleep(2);
				(core->net_join)(room);
				return 1;
			}
			if(!strcmp(guru->list[1], "logging"))
			{
				if(!strcmp(guru->list[2], "off")) (core->net_log)(NULL);
				else (core->net_log)(core->logfile);
				guru->message = strdup("Toggled logging.");
				(core->net_output)(guru);
				return 1;
			}
			break;
		default:
			if(!strcmp(guru->list[1], "announce"))
			{
				/*printf("DEBUG: announce!!!!!!!!\n");*/
				guru->message = strdup(guru->list[2]);
				guru->type = GURU_ADMIN;
				(core->net_output)(guru);
				return 1;
			}
	}

	return 0;
}

/* Main: startup until working state, loop */
int main(int argc, char *argv[])
{
	Gurucore *core;
	Guru *guru;
	char *opthost = NULL, *optname = NULL;

	/* Recognize command line arguments */
	struct option options[] =
	{
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"name", required_argument, 0, 'n'},
		{"host", required_argument, 0, 'H'},
		{0, 0, 0, 0}
	};
	int optindex;
	char opt;

	while(1)
	{
		opt = getopt_long(argc, argv, "hvH:n:", options, &optindex);
		if(opt == -1) break;
		switch(opt)
		{
			case 'h':
				printf("Grubby - the GGZ Gaming Zone Chat Bot\n");
				printf("Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net\n");
				printf("Published under GNU GPL conditions\n\n");
				printf("Recognized options:\n");
				printf("[-h | --help]:    Show this help screen\n");
				printf("[-H | --host]:    Connect to this host\n");
				printf("{-n | --name]:    Use this name\n");
				printf("[-v | --version]: Display version number\n");
				exit(0);
				break;
			case 'H':
				opthost = optarg;
				break;
			case 'n':
				optname = optarg;
				break;
			case 'v':
				printf("Grubby version 0.2\n");
				exit(0);
				break;
			default:
				printf("Unknown command line option, try --help.\n");
				exit(-1);
				break;
		}
	}

	/* Bring grubby to life */
	printf("Grubby: initializing...\n");
	core = guru_init();
	if(!core)
	{
		printf("Grubby initialization failed!\n");
		printf("Check if the bot is configured properly using grubby-config.\n");
		exit(-1);
	}

	/* Apply command line options */
	if(opthost) core->host = opthost;
	if(optname) core->name = optname;

	/* Start connection procedure */
	printf("Grubby: connect to %s...\n", core->host);
	(core->net_log)(core->logfile);
	(core->net_connect)(core->host, 5688, core->name, core->guestname);
	if(core->i18n_init) (core->i18n_init)();
	while(1)
	{
		/* Permanently check states */
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
				guru->guru = core->name;
				if(!admin(guru, core))
				{
					/* If message is valid, try to translate it first */
					if(core->i18n_check) (core->i18n_check)(guru->player, guru->message);
					guru = guru_work(guru);
					if(guru)
					{
						if(core->i18n_translate) guru->message = (core->i18n_translate)(guru->player, guru->message);
						(core->net_output)(guru);
					}
				}
				break;
		}
	}

	/* Shutdown the bot properly */
	return guru_close();
}

