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
#include "meta.h"

/* GGZ includes */
#include <ggzcore.h>

/* System includes */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/* TelGGZ version number */
#define TELGGZ_VERSION "0.5"

/* Compatible GGZ protocol number */
/* FIXME: could be part of libggzmeta */
#define GGZ_PROTOCOL_VERSION "10"

/* Primary GGZ metaserver */
#define GGZ_METASERVER "ggzmeta://live.ggzgamingzone.org"

/* Configuration */
#include "config.h"

/* Safe atoi with default value for NULL strings */
static int safe_atoi(const char *s, int defval)
{
	if(s) return atoi(s);
	return defval;
}

/* Print out all servers to play wrapper for */
static void pref_listservers(ServerEntry **preflist)
{
	ServerEntry *iterator;
	int i;

	/* Of course I meant, people will kill me for C++isms... */
	i = 0;
	while((preflist) && (preflist[i]))
	{
		iterator = preflist[i++];
		printf("%3i %22s %10s %20s %10ik\n",
			i,
			meta_server_findattribute(iterator, "host"),
			meta_server_findattribute(iterator, "version"),
			meta_server_findattribute(iterator, "location"),
			safe_atoi(meta_server_findattribute(iterator, "speed"), 0));
	}
	if(!i)
	{
		printf("TelGGZ: Error! No servers are available.\n");
		printf("Please check ~/.ggz/metaserver.cache.\n");
		exit(-1);
	}
}

/* The main function. How sweet. */
int main(int argc, char *argv[])
{
	GGZOptions opt;
	int ret, i;
	char hostname[128];
	char *username, *password;
	ServerEntry **metaservers, **ggzservers;
	ServerEntry *seed;
	ggz_uri_t uri;
	const char *host;
	int port;
	char *uristr;

	/* No output buffering. */
	setbuf(stdout, NULL);

	/* If a GGZ URI was given, skip the metaserver */
	if(argc == 2)
	{
		uri = ggz_uri_from_string(argv[1]);
		seed = meta_server_new(uri);
		meta_server_attribute(seed, "host", uri.host),
		meta_server_attribute(seed, "version", "???"),
		meta_server_attribute(seed, "location", "???"),
		ggzservers = NULL;
		ggzservers = meta_list_server(ggzservers, seed);
	}
	else
	{
		/* Meta server query to find all GGZ servers */
		uri = ggz_uri_from_string(GGZ_METASERVER);
		seed = meta_server_new(uri);

		printf("* synchronizing metaservers...\n");
		metaservers = meta_network_load();
		metaservers = meta_list_server(metaservers, seed);
		metaservers = meta_network_sync(metaservers);
		meta_network_store(metaservers);

		for(i = 0; metaservers[i]; i++)
		{
			uristr = ggz_uri_to_string(metaservers[i]->uri);
			printf("* consulting metaserver %s\n", uristr);
			free(uristr);
		}

		ggzservers = meta_queryallggz(metaservers, GGZ_PROTOCOL_VERSION);
		meta_list_free(metaservers);
	}

	/* This could be the default values. But we code in C here. */
	opt.flags = GGZ_OPT_MODULES | GGZ_OPT_PARSER;

	/* We just announce us the the world. */
	if(gethostname(hostname, sizeof(hostname)))
		strcpy(hostname, "[unknown]");
	printf("[TelGGZ: GGZ Gaming Zone Telnet Wrapper v%s]\n",
	       TELGGZ_VERSION);
	printf("You're connected to %s.\n", hostname);

	/* Make sure GGZ is actually available. */
	ret = ggzcore_init(opt);
	if(ret != 0)
	{
		printf("TelGGZ: Error! GGZ is currently unavailable. Please join back later.\n");
		exit(-1);
	}

	/* Let the chatter select a server. */
	printf("Please select a server from the list below.\n\n");
	pref_listservers(ggzservers);
	printf("\n");
	printf("Your choice: ");
	ret = chat_getserver() - 1;

	/* Check argument */
	for(i = 0; i <= ret; i++)
		if(!ggzservers[i]) ret = -1;

	/* Connect to that server */
	if(ret >= 0)
	{
		host = meta_server_findattribute(ggzservers[ret], "host");
		port = safe_atoi(meta_server_findattribute(ggzservers[ret], "port"), 0);

		printf("Login with username: ");
		username = chat_getusername();
		printf("Use password: ");
		password = chat_getpassword();
		printf("Connecting to %s:%i...\n", host, port);
		chat_connect(host, port, username, password);
		free(username);
		free(password);

		printf("Type '/help' to get the list of available commands.\n");

		/* Enter the chat loop */
		chat_loop();
	}
	else
	{
		printf("Well, maybe next time...\n");
	}

	/* Exit with humor */
	meta_list_free(ggzservers);

	return 0;
}

