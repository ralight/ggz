/* 
 * File: main.c
 * Author: Brent Hendricks, Rich Gade, Jason Short
 * Project: GGZCards Server
 * Date: 06/29/2000
 * Desc: Main loop
 * $Id: main.c 3483 2002-02-27 05:00:13Z jdorje $
 *
 * This file was originally taken from La Pocha by Rich Gade.  It just
 * contains the startup, command-line option handling, and main loop
 * for GGZCards.
 *
 * Copyright (C) 2000-2002 Brent Hendricks.
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
#include <ggz.h>

/* Needed for "select" loop only */
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "ai.h"
#include "common.h"
#include "options.h"

/* Both of these functions are taken from FreeCiv. */
/* static int is_option(const char *option_name,char *option); */
static char *get_option(const char *option_name, char **argv, int *i,
			int argc);
			
			
static void main_loop(GGZdMod *ggz);

/***************************************************************
 ...
***************************************************************/
/* 
   static int is_option(const char *option_name,char *option) { if
   (!strcmp(option_name,option) || !strncmp(option_name+1,option,2)) return
   1; return 0; } */

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
					ggzdmod_log(game.ggz,
						    "Empty argument for \"%s\".\n",
						    option_name);
					exit(1);
				}
			} else {
				ggzdmod_log(game.ggz,
					    "Missing argument for \"%s\".\n",
					    option_name);
				exit(1);
			}
		}

		return opt;
	}

	return NULL;
}

/* these should be part of libggzdmod */
/* note that for both es_error and es_exit, easysock must be called again to
   report the error! */
static void es_error(const char *msg, const GGZIOType op,
		     const GGZDataType data)
{
	fprintf(stderr, "ERROR: " "Bad easysock operation: %s.\n", msg);
}

static void es_exit(int result)
{
	fprintf(stderr, "ERROR: " "exiting because of easysock error.\n");
	exit(result);
}

int main(int argc, char **argv)
{
	int i;
	char* which_game = NULL;

	/* Initialize GGZ structures. */
	GGZdMod *ggz = ggzdmod_new(GGZDMOD_GAME);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_STATE, &handle_state_event);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_JOIN, &handle_join_event);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_LEAVE, &handle_leave_event);
	/* ggzdmod_set_handler(ggz, GGZDMOD_EVENT_PLAYER_DATA,
			    &handle_player_event); */
			
	init_path(argv[0]);
	
	/* See bid union in types.h */
	assert(sizeof(int) == 4 && sizeof(char) == 1);

	/* set up easysock functions to be called on error/exit */
	ggz_set_io_error_func(es_error);
	ggz_set_io_exit_func(es_exit);

	/* read options */
	for (i = 1; i < argc; i++) {
		char *option;
		ggzdmod_log(game.ggz, "Reading option %s.", argv[i]);
		if ((option = get_option("--game", argv, &i, argc)) != NULL) {
			which_game = games_get_gametype(option);
			ggzdmod_log(game.ggz, "which_game set to %s.",
				    which_game);
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
			ggzdmod_log(game.ggz, "Set option '%s' to %d.",
				    option, val);
			set_option(option, val);
		} else {
			/* bad option */
			ggzdmod_log(game.ggz, "ERROR: bad options '%s'.",
				    argv[i]);
		}
	}

	init_ggzcards(ggz, which_game);

	/* Connect to GGZ server; main loop */
	if (ggzdmod_connect(ggz) < 0)
		return -1;
	(void) ggzdmod_log(ggz, "Starting table.");
	
	main_loop(ggz);
	
	(void) ggzdmod_log(ggz, "Halting table.");
	(void) ggzdmod_disconnect(ggz);
	ggzdmod_free(ggz);

	return 0;
}

static void main_loop(GGZdMod *ggz)
{
	/* FIXME: I'd like to just use ggzdmod_loop, but it won't montitor
	   the bot channels. */
	/* (void) ggzdmod_loop(ggz); */
	do {
		/* FIXME: this is a whole lot of unnecessary code... */
		fd_set fd_set;
		int max_fd = ggzdmod_get_fd(ggz), p, status;
		
		FD_ZERO(&fd_set);
		FD_SET(max_fd, &fd_set);
		
		for (p = 0; p < game.num_players; p++) {
			int fd = get_player_socket(p);
			if (fd < 0)
				continue;
			if (fd > max_fd)
				max_fd = fd;
			FD_SET(fd, &fd_set);			
		}
		
		status = select(max_fd + 1, &fd_set, NULL, NULL, NULL);
		
		if (status <= 0) {
			if (errno != EINTR)
				break;
			continue;
		}
		
		if (FD_ISSET(ggzdmod_get_fd(ggz), &fd_set))
			ggzdmod_dispatch(ggz);
			
		for (p = 0; p < game.num_players; p++) {
			int fd = get_player_socket(p);
			
			if (fd >= 0 && FD_ISSET(fd, &fd_set))
				handle_player_event(ggz, GGZDMOD_EVENT_PLAYER_DATA, &p);		
		}	
	} while (ggzdmod_get_state(ggz) < GGZDMOD_STATE_DONE);
}
