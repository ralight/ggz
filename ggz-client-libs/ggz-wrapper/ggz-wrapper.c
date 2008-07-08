/*
 * File: ggz-wrapper.c
 * Author: Josef Spillner
 * Project: GGZ Client libs
 * Date: 2004
 * $Id: ggz-wrapper.c 10209 2008-07-08 16:03:03Z jdorje $
 *
 * Code for a wrapper for GGZ games
 *
 * Copyright (C) 2004-2005 GGZ Development Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "server.h"
#include "loop.h"
#include "game.h"

#define DEFAULT_SERVER   "live.ggzgamingzone.org"
#define DEFAULT_PORT     5688
#define DEFAULT_GAMENAME "TicTacToe"
#define DEFAULT_NICKNAME NULL
#define DEFAULT_PASSWORD NULL
#define DEFAULT_DESTNICK NULL

#define _(x) x

char *game_name;
char *dst_nick;
char *frontend;

static void usage(void) {
	printf(_("ggz-wrapper: Dummy core client for the GGZ Gaming Zone\n"));
	printf(_("\n"));
	printf(_("Options:\n"));
	printf(_("[-g <gametype>] Type of game to play\n"));
	printf(_("[-u <user>    ] Your nickname\n"));
	printf(_("[-p <password>] Password (empty for guest logins)\n"));
	printf(_("[-s <server>  ] Server hostname\n"));
	printf(_("[-P <port>    ] Server port\n"));
	printf(_("[-d <destnick>] Nickname of opponent\n"));
	printf(_("[-f <frontend>] Preferred game client frontend\n"));
	exit(0);
}

int main(int argc, char **argv) {
	struct option options[] =
	{
		{"help", no_argument, 0, 'h'},
		{"gametype", required_argument, 0, 'g'},
		{"user", required_argument, 0, 'u'},
		{"password", required_argument, 0, 'p'},
		{"server", required_argument, 0, 's'},
		{"port", required_argument, 0, 'P'},
		{"destnick", required_argument, 0, 'd'},
		{"frontend", required_argument, 0, 'f'},
		{0, 0, 0, 0}
	};
	char optstring[] = "g:u:p:s:d:P:f:h";
	char randomuser[64];

	int optch;
	int optindex;

	char *host = DEFAULT_SERVER;
	char *password = DEFAULT_PASSWORD;
	char *nick = DEFAULT_NICKNAME;
	int port = DEFAULT_PORT;

	game_name = DEFAULT_GAMENAME;
	dst_nick = DEFAULT_DESTNICK;
	frontend = NULL;

	while((optch = getopt_long(argc, argv, optstring, options, &optindex)) != -1) {
		switch(optch) {
			case 'g':
				game_name = optarg;
				break;
			case 'u':
				nick = optarg;
				break;
			case 'p':
				password = optarg;
				break;
			case 's':
				host = optarg;
				break;
			case 'd':
				dst_nick = optarg;
				break;
			case 'P':
				port = atoi(optarg);
				break;
			case 'f':
				frontend = optarg;
				break;
			case 'h':
				usage();
				break;
			default:
				printf(_("Option %c unknown, try --help.\n"), optch);
				usage();
				break;
		}
	}

	if(!nick) {
		snprintf(randomuser, sizeof(randomuser), _("guest%i"), rand() % 10000);
		nick = strdup(randomuser);
	}

	if(password)
		server_init(host, port, GGZ_LOGIN, nick, password);
	else
		server_init(host, port, GGZ_LOGIN_GUEST, nick, password);
	loop();
	return 0;
}
