/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 - 2004 Josef Spillner, <josef@ggzgamingzone.org>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "guru.h"
#include "net.h"
#include "i18n.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define GRUBBY_VERSION "0.6"

/* Execute administrative commands */
/* FIXME: Log admin commands */
static int admin(Guru *guru, Gurucore *core)
{
	int i;
	char *room;
	int valid;

	if(!guru) return 0;
	if(!guru->message) return 0;
	if(!core->owner) return 0;
	if((guru->player) && (strcmp(guru->player, core->owner))) return 0;

	i = 0;
	while((guru->list) && (guru->list[i])) i++;

	valid = 0;
	if(guru->type == GURU_PRIVMSG) valid = 1;
	else if(guru->type == GURU_DIRECT) valid = 1;
	if(!valid) return 0;

	switch(i)
	{
		case 0:
		case 1:
		case 2:
			break;
		case 3:
			if((!strcmp(guru->list[1], "goto")) && (valid = 1))
			{
				room = guru->list[2];
				guru->message = strdup(__("Yes, I follow, my master."));
				(core->net_output)(guru);
				sleep(2);
				(core->net_join)(room);
				return 1;
			}
			if((!strcmp(guru->list[1], "logging")) && (valid = 1))
			{
				if(!strcmp(guru->list[2], "off")) (core->net_log)(NULL);
				else (core->net_log)(core->logfile);
				guru->message = strdup(__("Toggled logging."));
				(core->net_output)(guru);
				return 1;
			}
			break;
		default:
			if((!strcmp(guru->list[1], "announce")) && (valid = 1))
			{
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
	char *opthost = NULL, *optname = NULL, *optdatadir = NULL;
	char *input;
	int language;
	int ret;

	/* Recognize command line arguments */
	struct option options[] =
	{
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"name", required_argument, 0, 'n'},
		{"host", required_argument, 0, 'H'},
		{"datadir", required_argument, 0, 'd'},
		{0, 0, 0, 0}
	};
	int optindex;
	int opt;

	extern int _nl_msg_cat_cntr;

	bindtextdomain("grubby", PREFIX "/share/locale");
	textdomain("grubby");
	setlocale(LC_ALL, "");

	setenv("LANGUAGE", getenv("LANG"), 1);
	++_nl_msg_cat_cntr;

	while(1)
	{
		opt = getopt_long(argc, argv, "hvH:n:d:", options, &optindex);
		if(opt == -1) break;
		switch(opt)
		{
			case 'h':
				printf(_("Grubby - the GGZ Gaming Zone Chat Bot\n"));
				printf(_("Copyright (C) 2001 - 2004 Josef Spillner, josef@ggzgamingzone.org\n"));
				printf(_("Published under GNU GPL conditions\n\n"));
				printf(_("Recognized options:\n"));
				printf(_("[-h | --help]:    Show this help screen\n"));
				printf(_("[-H | --host]:    Connect to this host\n"));
				printf(_("[-n | --name]:    Use this name\n"));
				printf(_("[-d | --datadir]: Use this data directory (default: ~/.ggz)\n"));
				printf(_("[-v | --version]: Display version number\n"));
				exit(0);
				break;
			case 'H':
				opthost = optarg;
				break;
			case 'n':
				optname = optarg;
				break;
			case 'v':
				printf(_("Grubby version %s\n"), GRUBBY_VERSION);
				exit(0);
				break;
			case 'd':
				optdatadir = optarg;
				break;
			default:
				printf(_("Unknown command line option, try --help.\n"));
				exit(-1);
				break;
		}
	}

	/* Bring grubby to life */
	printf(_("Grubby: initializing...\n"));
	core = guru_init(optdatadir);
	if(!core)
	{
		printf(_("Grubby initialization failed!\n"));
		printf(_("Check if the bot is configured properly using grubby-config.\n"));
		exit(-1);
	}

	/* Apply command line options */
	if(opthost) core->host = opthost;
	if(optname) core->name = optname;
	if(optdatadir) core->datadir = optdatadir;

	/* Start connection procedure */
	printf(_("Grubby: connect to %s...\n"), core->host);
	(core->net_log)(core->logfile);
	(core->net_connect)(core->host, 5688, core->name, core->guestname);
	if(core->i18n_init) (core->i18n_init)(core->language, getenv("LANG"));

	/* Main loop */
	while(1)
	{
		/* Permanently check states */
		switch((core->net_status)())
		{
			case NET_ERROR:
				printf(_("ERROR: Couldn't connect\n"));
				exit(-1);
				break;
			case NET_LOGIN:
				printf(_("Grubby: Logged in.\n"));
				(core->net_join)(core->autojoin);
				break;
			case NET_GOTREADY:
				printf(_("Grubby: Ready.\n"));
				break;
			case NET_INPUT:
				guru = (core->net_input)();
				guru->guru = core->name;
				guru->datadir = core->datadir;

				if(core->i18n_catalog) (core->i18n_catalog)(1);
				if(core->i18n_check) (core->i18n_check)(guru->player, "", 1);
				ret = admin(guru, core);
				if(core->i18n_catalog) (core->i18n_catalog)(0);

				if(!ret)
				{
					/* If message is valid, try to translate it first */
					language = 0;
					if(guru->type == GURU_PRIVMSG) language = 1;
					else if(guru->type == GURU_DIRECT) language = 1;

					input = NULL;
					if(core->i18n_check)
					{
						(core->i18n_catalog)(1);
						input = (core->i18n_check)(guru->player, guru->message, language);
						(core->i18n_catalog)(0);

						if(input)
						{
							free(guru->message);
							guru->message = input;
							/*guru->message = (core->i18n_translate)(guru->player, guru->message);*/
							(core->net_output)(guru);
						}
					}
					if(!input)
					{
						if(core->i18n_catalog) (core->i18n_catalog)(1);
						if(core->i18n_check) (core->i18n_check)(guru->player, "", language);
						guru = guru_work(guru);
						if(core->i18n_catalog) (core->i18n_catalog)(0);

						if(guru)
						{
							/*if(core->i18n_translate)
								guru->message = (core->i18n_translate)(guru->player, guru->message);*/
							(core->net_output)(guru);
						}
					}
				}
				break;
		}
	}

	/* Shutdown the bot properly */
	return guru_close(core);
}

