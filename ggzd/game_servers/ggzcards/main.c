/*
 * File: main.c
 * Author: Brent Hendricks, Jason Short
 * Project: GGZCards game module
 * Date: 06/29/2000
 * Desc: Main loop
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

	/* Connect to GGZ server */
	if ((ggz_sock = ggzdmod_connect()) < 0) {
		ggzdmod_debug("Failed ggz_sock test.");
		return -1;
	}

	/* Seed the random number generator */
	srandom((unsigned) time(NULL));

	FD_ZERO(&active_fd_set);
	FD_SET(ggz_sock, &active_fd_set);

	init_ggzcards(which_game);
	while (!game_over) {

		read_fd_set = active_fd_set;
		fd_max = ggzdmod_fd_max();

		status = select((fd_max + 1), &read_fd_set, NULL, NULL, NULL);

		if (status <= 0) {
			if (errno == EINTR)
				continue;
			else
				return -1;
		}

		/* Check for message from GGZ server */
		if (FD_ISSET(ggz_sock, &read_fd_set)) {
			status = handle_ggz(ggz_sock, &fd);
			switch (status) {

			case -1:	/* Big error!! */
				ggzdmod_debug("handle_ggz gives status == -1.");
				return -1;

			case 0:	/* All ok, how boring! */
				break;

			case 1:	/* A player joined */
				FD_SET(fd, &active_fd_set);
				break;

			case 2:	/* A player left */
				FD_CLR(fd, &active_fd_set);
				break;

			case 3:	/*Safe to exit */
				game_over = 1;
				break;
			}
		}

		/* Check for message from player */
		for (i = 0; i < ggzdmod_seats_num(); i++) {
			fd = ggz_seats[i].fd;
			if (fd != -1 && FD_ISSET(fd, &read_fd_set)) {
				status = handle_player(i);
				if (status == -1)
					FD_CLR(fd, &active_fd_set);
			}
		}
	}

	if (ggzdmod_disconnect() < 0)
		/* what else can we do??? */
		fprintf(stderr, "GGZCards: ggzdmod: failed disconnect\n");

	return 0;
}
