/*
 * File: main.c
 * Author: Brent Hendricks, Jason Short
 * Project: GGZCards Server
 * Date: 06/29/2000
 * Desc: Main loop
 * $Id: main.c 2193 2001-08-23 08:57:10Z jdorje $
 *
 * This file was originally taken from La Pocha by Rich Gade.  It just
 * contains the startup, command-line option handling, and main loop
 * for GGZCards.
 *
 * Copyright (C) 2000 Brent Hendricks.
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

#include <easysock.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"

/* Both of these functions are taken from FreeCiv. */
/* static int is_option(const char *option_name,char *option); */
static char *get_option(const char *option_name, char **argv, int *i,
			int argc);

/***************************************************************
 ...
***************************************************************/
/*
static int is_option(const char *option_name,char *option)
{
  if (!strcmp(option_name,option) ||
      !strncmp(option_name+1,option,2)) return 1;
  return 0;
}
*/

/**************************************************************************
  return a char * to the parameter of the option or NULL.
  *i can be increased to get next string in the array argv[].
  It is an error for the option to exist but be an empty string.
  This doesn't use freelog() because it is used before logging is set up.
**************************************************************************/
static char *get_option(const char *option_name, char **argv, int *i,
			int argc)
{
	int len = strlen(option_name);

	if (!strcmp(option_name, argv[*i]) ||
	    (!strncmp(option_name, argv[*i], len) && argv[*i][len] == '=') ||
	    !strncmp(option_name + 1, argv[*i], 2)) {
		char *opt = argv[*i] + (argv[*i][1] != '-' ? 0 : len);

		if (*opt == '=') {
			opt++;
		} else {
			if (*i < argc - 1) {
				(*i)++;
				opt = argv[*i];
				if (strlen(opt) == 0) {
/*	  fprintf(stderr, _("Empty argument for \"%s\".\n", option_name); */
					exit(1);
				}
			} else {
/*	fprintf(stderr, _("Missing argument for \"%s\".\n"), option_name); */
				exit(1);
			}
		}

		return opt;
	}

	return NULL;
}

/* these should be part of libggzdmod */
/* note that for both es_error and es_exit, easysock must be called again
 * to report the error! */
static void es_error(const char *msg, const EsOpType op,
		     const EsDataType data)
{
	ggzdmod_debug("ERROR: " "Bad easysock operation: %s.", msg);
}

static void es_exit(int result)
{
	ggzdmod_debug("ERROR: " "exiting because of easysock error.");
	exit(result);
}

int main(int argc, char **argv)
{
	char game_over = 0;
	int i, fd, ggz_sock, fd_max, status;
	fd_set active_fd_set, read_fd_set;

	int which_game = GGZ_GAME_UNKNOWN;

	/* set up easysock functions to be called on error/exit */
	es_err_func_set(es_error);
	es_exit_func_set(es_exit);

	/* read options */
	for (i = 1; i < argc; i++) {
		char *option;
		printf("TEST: Reading argv '%s'.", argv[i]);
		if ((option = get_option("--game", argv, &i, argc)) != NULL) {
			which_game = games_get_gametype(option);
		} else if ((option =
			    get_option("--option", argv, &i, argc)) != NULL) {
			/* argument is of the form --option=<option>:<value> */
			char *colon = strchr(option, ':');
			int val;
			if (colon == NULL)
				continue;
			*colon = 0;
			colon++;
			if (!*colon)
				continue;
			val = atoi(colon);
			printf("TEST: Set option '%s' to %d.", option, val);
			set_option(option, val);
		} else {
			/* bad option */
			ggzdmod_debug("ERROR: bad options '%s'.", argv[i]);
		}
	}

	/* Seed the random number generator */
	srandom((unsigned) time(NULL));

	/* set up handlers */
	ggzdmod_set_handler(GGZ_EVENT_LAUNCH, &handle_launch_event);
	ggzdmod_set_handler(GGZ_EVENT_JOIN, &handle_join_event);
	ggzdmod_set_handler(GGZ_EVENT_LEAVE, &handle_leave_event);
	/* ggzdmod_set_handler(GGZ_EVENT_QUIT, &handle_gameover); */
	ggzdmod_set_handler(GGZ_EVENT_PLAYER, &handle_player_event);

	/* Connect to GGZ server; main loop */
	if (ggzdmod_main() < 0) {
		ggzdmod_debug("Failed ggz_sock test.");
		return -1;
	}

	return 0;
}
