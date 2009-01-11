/*
 * File: ggz-wrapper.c
 * Author: Josef Spillner
 * Project: GGZ Client libs
 * Date: 2004
 * $Id: ggz-wrapper.c 10849 2009-01-11 09:25:29Z josef $
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

#include <ggz.h>

#include "server.h"
#include "loop.h"
#include "game.h"

#define _(x) x

static void usage(int retval) {
	printf(_("ggz-wrapper: Dummy core client for the GGZ Gaming Zone\n"));
	printf(_("Syntax: ggz-wrapper [options] [<GGZ URI>]\n"));
	printf(_("\n"));
	printf(_("Options:\n"));
	printf(_("[-g | --gametype <gametype>] Type of game to play, in any room\n"));
//	printf(_("[-p <password>] Password (empty for guest logins)\n"));
	printf(_("[-d | --destnick <destnick>] Nickname of human opponent, bot otherwise\n"));
	printf(_("[-f | --frontend <frontend>] Preferred game client frontend\n"));
	printf(_("Options can also be set in ~/.ggz/ggz-wrapper.rc\n"));
	exit(retval);
}

int main(int argc, char **argv) {
	struct option options[] =
	{
		{"help", no_argument, 0, 'h'},
		{"gametype", required_argument, 0, 'g'},
//		{"password", required_argument, 0, 'p'},
		{"destnick", required_argument, 0, 'd'},
		{"frontend", required_argument, 0, 'f'},
		{0, 0, 0, 0}
	};
//	char optstring[] = "g:p:d:f:h";
	char optstring[] = "g:d:f:h";
	char randomuser[64];

	int optch;
	int optindex;

//	char *password = NULL;
	char *uristr = NULL;

	char *game_name = NULL;
	char *dst_nick = NULL;
	char *frontend = NULL;

	while((optch = getopt_long(argc, argv, optstring, options, &optindex)) != -1) {
		switch(optch) {
			case 'g':
				game_name = optarg;
				break;
//			case 'p':
//				password = optarg;
//				break;
			case 'd':
				dst_nick = optarg;
				break;
			case 'f':
				frontend = optarg;
				break;
			case 'h':
				usage(0);
				break;
			default:
				printf(_("Option %c unknown, try --help.\n"), optch);
				usage(1);
				break;
		}
	}

	if(optind != argc - 1) {
		printf(_("No GGZ URI given.\n"));
		usage(1);
	}
	uristr = argv[optind];

	ggz_uri_t uri = ggz_uri_from_string(uristr);
	if(!uri.user) {
		// FIXME: this could go into ggzcore_mainloop
		snprintf(randomuser, sizeof(randomuser), _("%s-%i"), getenv("USER"), rand() % 10000);
		uri.user = strdup(randomuser);
	}
	uristr = ggz_uri_to_string(uri);

	server_init(uristr, dst_nick, frontend, game_name);

	ggz_free(uristr);
	ggz_uri_free(uri);

	return 0;
}
