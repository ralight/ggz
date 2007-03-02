/* 
 * File: main.c
 * Author: Brent Hendricks, Rich Gade, Jason Short
 * Project: GGZCards Server
 * Date: 06/29/2000
 * Desc: Main loop
 * $Id: main.c 8994 2007-03-02 20:19:40Z jdorje $
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
#  include <config.h>		/* Site-specific config */
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

static char *get_option(const char *option_name, char **argv, int *i,
			int argc);
static void main_loop(GGZdMod * ggz);

static void es_error(const char *msg, const GGZIOType op,
                     int fd, const GGZDataType data);
static void es_exit(int result);
static void handle_debug_message(int priority, const char *msg);
static void initialize_debugging(void);

/**************************************************************************
  This function was taken from FreeCiv.  See http://www.freeciv.org/ for
  attributions.

  return a char * to the parameter of the option or NULL.
  *i can be increased to get next string in the array argv[].
  It is an error for the option to exist but be an empty string.
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
					ggz_debug(DBG_MISC,
						    "Empty argument for \"%s\".\n",
						    option_name);
					exit(1);
				}
			} else {
				ggz_debug(DBG_MISC,
					    "Missing argument for \"%s\".\n",
					    option_name);
				exit(1);
			}
		}

		return opt;
	}

	return NULL;
}

/* perhaps these should be part of libggzdmod? */
/* note that for both es_error and es_exit, easysock must be called again to
   report the error! Therefore we instead just send it to stderr, which isn't
   very useful. */
static void es_error(const char *msg, const GGZIOType op,
                     int fd, const GGZDataType data)
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
	game_data_t *game_data = NULL;
	GGZdMod *ggz;

	if (!ggzdmod_is_ggz_mode()) {
		printf("This program should only be run from within GGZ.");
		printf("\n");
		exit(1);
	}

	initialize_debugging();

	/* Initialize GGZ structures. */
	ggz = ggzdmod_new(GGZDMOD_GAME);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_STATE, &handle_ggz_state_event);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_JOIN, &handle_ggz_seat_event);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_LEAVE, &handle_ggz_seat_event);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_SEAT, &handle_ggz_seat_event);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_SPECTATOR_JOIN,
			    &handle_ggz_spectator_seat_event);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_SPECTATOR_LEAVE,
			    &handle_ggz_spectator_seat_event);
#if 0
	/* This is excluded because it's taken care of separately by the
	   specialized main loop that ggzcards uses.  Normally we'd register
	   this function so that ggzdmod could monitor the player sockets and
	   call it if necessary, but instead we do that manually. */
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_PLAYER_DATA,
			    &handle_ggz_player_data_event);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_SPECTATOR_DATA,
			    &handle_ggz_spectator_data_event);
#endif

	/* set up easysock functions to be called on error/exit */
	ggz_set_io_error_func(es_error);
	ggz_set_io_exit_func(es_exit);

	/* read options.  see options.[ch] to find out how game options work.
	 */
	for (i = 1; i < argc; i++) {
		char *option;
		ggz_debug(DBG_MISC, "Reading option %s.", argv[i]);
		if ((option = get_option("--game", argv, &i, argc)) != NULL) {
			game_data = games_get_gametype(option);
			ggz_debug(DBG_MISC, "which_game set to %s.",
				    game_data->name);
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
			ggz_debug(DBG_MISC, "Set option '%s' to %d.",
				    option, val);
			set_option(option, val);
		} else {
			/* bad option */
			ggz_error_msg("bad options '%s'.",  argv[i]);
		}
	}

	/* pre-initialization of the program.  Although we may know what game
	   we're playing at this point, we don't yet do any specific
	   initializations. */
	init_ggzcards(ggz, game_data);

	/* Connect to GGZ server; main loop */
	if (ggzdmod_connect(ggz) < 0)
		return -1;
	ggz_debug(DBG_MISC, "Starting table.");

	/* Scan in the available AI modules. */
	read_ai_modules(argv[0]);

#if 0
	(void) ggzdmod_loop(ggz);
#else
	/* Instead of using ggzdmod's main loop function, we use our own.
	   This is necessary because we have to monitor extra file
	   descriptors. */
	main_loop(ggz);
#endif

 	/* Unfortunately this must come here, since we can do no logging later. */
	ggz_debug(DBG_MISC, "Halting table.");
	ggz_debug_cleanup(GGZ_CHECK_MEM);

	(void) ggzdmod_disconnect(ggz);
	ggzdmod_free(ggz);

	return 0;
}

static void main_loop(GGZdMod * ggz)
{
	/* FIXME: I'd like to just use ggzdmod_loop, but it won't montitor
	   the bot channels. */
	do {
		/* this is a whole lot of unnecessary code... */
		fd_set read_fd_set, write_fd_set;
		int max_fd = ggzdmod_get_fd(ggz), status;

		FD_ZERO(&read_fd_set);
		FD_SET(max_fd, &read_fd_set);

		FD_ZERO(&write_fd_set);

		/* Assemble a list of file descriptors to monitor.  This list
		   includes the main GGZ connection, a connection for each
		   player (including bots), plus a stderr connection for bots.
		 */
		allplayers_iterate(p) {
			GGZDataIO *dio = get_player_dio(p);
			int fd = dio ? ggz_dio_get_socket(dio) : -1;

			if (fd >= 0) {
				max_fd = MAX(max_fd, fd);

#if 0 /* With nonatomic seat ops this is possible now. */
				assert(!FD_ISSET(fd, &read_fd_set));
				assert(!FD_ISSET(fd, &write_fd_set));
#endif

				FD_SET(fd, &read_fd_set);
				if (ggz_dio_is_write_pending(dio)) {
					FD_SET(fd, &write_fd_set);
				}
			}

#ifdef DEBUG
			if (get_player_status(p) == GGZ_SEAT_BOT) {
				fd = game.players[p].err_fd;
				if (fd >= 0) {
					max_fd = MAX(max_fd, fd);
					assert(!FD_ISSET(fd, &read_fd_set));
					FD_SET(fd, &read_fd_set);
				}
			}
#endif /* DEBUG */
		} allplayers_iterate_end;

		status = select(max_fd + 1, &read_fd_set, &write_fd_set,
				NULL, NULL);

		if (status <= 0) {
			if (errno != EINTR)
				break;
			continue;
		}

		if (FD_ISSET(ggzdmod_get_fd(ggz), &read_fd_set))
			ggzdmod_dispatch(ggz);

		/* Check each FD for activity */
		allplayers_iterate(p) {
			GGZDataIO *dio = get_player_dio(p);
			int fd = dio ? ggz_dio_get_socket(dio) : -1;

			/* This is the player's communication socket.  Note
			   that AI players will have such a socket too, since
			   they are run as client-like programs. */
			if (fd >= 0 && FD_ISSET(fd, &read_fd_set)) {
				/* Note - this handles spectator data too,
				   but the spectator is given a player
				   number. */
				handle_player_data_event(p);
				FD_CLR(fd, &read_fd_set);
			}

			if (fd >= 0 && FD_ISSET(fd, &write_fd_set)) {
				ggz_dio_write_data(dio);
				FD_CLR(fd, &write_fd_set);
			}

#ifdef DEBUG
			/* The AI can send output to stderr; this is read by
			   us and translated as debugging output. */
			if (get_player_status(p) == GGZ_SEAT_BOT) {
				fd = game.players[p].err_fd;
				if (fd >= 0 && FD_ISSET(fd, &read_fd_set))
					handle_ai_stderr(p);
			}
#endif /* DEBUG */
		} allplayers_iterate_end;
	} while (ggzdmod_get_state(ggz) < GGZDMOD_STATE_DONE);
}

static void handle_debug_message(int priority, const char *msg)
{
	if (!game.ggz
#ifdef DEBUG
	    || 1
#endif
	    || ggzdmod_log(game.ggz, "%s", msg) < 0) {
		fflush(stdout);
		fputs(msg, stderr);
		fputs("\n", stderr);
		fflush(NULL);
	}
}

static void initialize_debugging(void)
{
#ifdef DEBUG
	const char *debug_types[] = {"GGZDMOD",
				     DBG_BID, DBG_PLAY, DBG_AI,
	                             DBG_GAME, DBG_NET, DBG_CLIENT,
	                             DBG_MISC, NULL};
#else
	const char *debug_types[] = {NULL};
#endif
	ggz_debug_init(debug_types, NULL);
	ggz_debug_set_func(handle_debug_message);
}
