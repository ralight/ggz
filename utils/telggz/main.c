/*
 * TelGGZ - The GGZ Gaming Zone Telnet Wrapper
 * Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net

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
#include "meta.h"

/* GGZ includes */
#include <ggzcore.h>

/* System includes */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/* TelGGZ version number */
#define VERSION "0.1"

/* Configuration */
#include "config.h"

/* Structure containing server information */
ServerGGZ **preflist;

/* Print out all servers to play wrapper for */
void pref_listservers()
{
	ServerGGZ *iterator;
	int i;

	/* Of course I meant, people will kill me for C++isms... */
	i = 0;
	while((preflist) && (preflist[i]))
	{
		iterator = preflist[i++];
		printf("%3i %22s %10s %20s %10ik\n", iterator->id, iterator->host,
			iterator->version, iterator->location, iterator->speed);
	}
}

/* The main function. How sweet. */
int main(int argc, char *argv[])
{
	GGZOptions opt;
	int ret;
	char hostname[128];
	char *username, *password;

	/* Meta server stuff */
	meta_init();
	meta_sync();
	preflist = meta_query(VERSION);

	/* This could be the default values. But we code in C here. */
	opt.flags = GGZ_OPT_MODULES | GGZ_OPT_PARSER;
	/*opt.debug_file = NULL;
	opt.debug_levels = 0;*/

	/* We just announce us the the world. */
	if(gethostname(hostname, sizeof(hostname))) strcpy(hostname, "[unknown]");
	printf("[TelGGZ: GGZ Gaming Zone Telnet Wrapper v%s]\n", VERSION);
	printf("You're connected to %s.\n", hostname);
	fflush(NULL);

	/* Make sure GGZ is actually available. */
	ret = ggzcore_init(opt);
	if(ret != 0)
	{
		printf("TelGGZ: Error! GGZ is currently unavailable. Please join back later.\n");
		fflush(NULL);
		exit(-1);
	}

	/* Let the chatter select a server. */
	printf("Please select a server from the list below.\n\n");
	pref_listservers();
	printf("\n");
	printf("Your choice: ");
	fflush(NULL);
	ret = chat_getserver() - 1;

	/* Connect to that server */
	if(ret >= 0)
	{
		printf("Login with username: ");
		fflush(NULL);
		username = chat_getusername();
		printf("Use password: ");
		fflush(NULL);
		password = chat_getpassword();
		printf("Connecting to %s:%i...\n", preflist[ret]->host, preflist[ret]->port);
		fflush(NULL);
		chat_connect(preflist[ret]->host, preflist[ret]->port, username, password);

		printf("Type '/help' to get the list of available commands.\n");
		fflush(NULL);

		/* Enter the chat loop */
		chat_loop();
	}

	/* Exit with humor */
	meta_free(preflist);
	return 0;
}

