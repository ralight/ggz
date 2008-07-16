/*
 * File: main.c
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers Client
 * Date: 01/01/2001
 * Desc: Main loop and supporting logic
 * $Id: main.c 9952 2008-04-12 22:50:04Z oojah $
 *
 * Copyright (C) 2001-2002 Richard Gade.
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
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <ggz.h>
#include <ggz_common.h>
#include <ggzmod.h>

#include "dlg_about.h"
#include "ggzintl.h"

#include "main.h"
#include "game.h"
#include "protocol.h"
#include "display.h"

static void initialize_debugging(void);
static void cleanup_debugging(void);
static void initialize_about_dialog(void);

static int get_seat(void);
static int get_players(void);
static int get_opponent_move(void);
static int get_move_response(void);
static int get_gameover_msg(void);
static int get_sync_data(void);

int main(int argc, char *argv[])
{
	initialize_debugging();
	ggz_intl_init("chinese-checkers");

	gtk_init(&argc, &argv);
	initialize_about_dialog();

	game_init();

	gtk_main();

	if (ggzmod_disconnect(game_mod()) < 0)
		return -2;
	ggzmod_free(game_mod());

	cleanup_debugging();

	return 0;
}


static void initialize_debugging(void)
{
	/* Our debugging code uses libggz's ggz_debug() function, so we
	   just initialize the _types_ of debugging we want. */
#ifdef DEBUG
	const char *debugging_types[] = { "main", NULL };
#else
	const char *debugging_types[] = { NULL };
#endif
	/* Debugging goes to ~/.ggz/ccheckers-gtk.debug */
#ifdef WIN32
	char *file_name =
	    g_strdup_printf("%s/.ggz/ccheckers-gtk.debug", getenv("APPDATA"));
#else
	char *file_name =
	    g_strdup_printf("%s/.ggz/ccheckers-gtk.debug", getenv("HOME"));
#endif
	ggz_debug_init(debugging_types, file_name);
	g_free(file_name);

	ggz_debug("main", "Starting chinese checkers client.");
}


/* This function should be called at the end of the program to clean up
 * debugging, as necessary. */
static void cleanup_debugging(void)
{
	/* ggz_cleanup_debug writes the data out to the file and does a
	   memory check at the same time. */
	ggz_debug("main", "Shutting down chinese checkers client.");
#ifdef DEBUG
	ggz_debug_cleanup(GGZ_CHECK_MEM);
#else
	ggz_debug_cleanup(GGZ_CHECK_NONE);
#endif
}


static void initialize_about_dialog(void)
{
	const char *content =
	    _
	    ("Visit us at http://www.ggzgamingzone.org/gameclients/chinese-checkers/\n"
	     "\n"
	     "Copyright (C) 2000, 2001 Richard Gade and Brent Hendricks \n");
	char *header;

	header = g_strdup_printf(_("GGZ Gaming Zone\n"
				   "Chinese Checkers Version %s"),
				 VERSION);
	init_dlg_about(_("About Chinese Checkers"), header, content);
	g_free(header);
}


gboolean main_io_handler(GGZMod * mod)
{
	int op, status;

	game.fd = ggzmod_get_server_fd(mod);

	if (ggz_read_int(game.fd, &op) < 0) {
		return FALSE;
	}

	status = 0;
	switch (op) {
	case CC_MSG_SEAT:
		status = get_seat();
		break;
	case CC_MSG_PLAYERS:
		status = get_players();
		break;
	case CC_MSG_MOVE:
		status = get_opponent_move();
		break;
	case CC_MSG_GAMEOVER:
		status = get_gameover_msg();
		break;
	case CC_MSG_SYNC:
		status = get_sync_data();
		break;
	case CC_REQ_MOVE:
		game_notify_our_turn();
		break;
	case CC_RSP_MOVE:
		status = get_move_response();
		break;
	default:
		ggz_error_msg("Unknown opcode received %d", op);
		status = -1;
		break;
	}

	if (status < 0) {
		ggz_error_msg("Ouch!");
		return FALSE;
	}

	return TRUE;
}


static int get_seat(void)
{
	if (ggz_read_int(game.fd, &game.players) < 0
	    || ggz_read_int(game.fd, &game.me) < 0)
		return -1;

	game_init_board();

	return 0;
}


static int get_players(void)
{
	int i, old;
	static int firsttime = 1;
	char *tmp;

	if (ggz_read_int(game.fd, &game.players) < 0)
		return -1;
	for (i = 0; i < game.players; i++) {
		old = game.seats[i];
		if (ggz_read_int(game.fd, &game.seats[i]) < 0)
			return -1;
		if (game.seats[i] != GGZ_SEAT_OPEN) {
			if (ggz_read_string
			    (game.fd, (char *)&game.names[i], 17) < 0)
				return -1;
			display_set_name(i, game.names[i]);
			if (old == GGZ_SEAT_OPEN && !firsttime) {
				tmp =
				    g_strdup_printf(_
						    ("%s joined the table"),
						    game.names[i]);
				display_statusbar(tmp);
				g_free(tmp);
			}
		} else if (old != GGZ_SEAT_OPEN && !firsttime) {
			tmp = g_strdup_printf(_("%s left the table"),
					      game.names[i]);
			display_statusbar(tmp);
			g_free(tmp);
			display_set_name(i, _("empty"));
		}
	}

	firsttime = 0;
	/* game.got_players++; */

	return 0;
}


static int get_opponent_move(void)
{
	int seat;
	char ro, co, rd, cd;

	if (ggz_read_int(game.fd, &seat) < 0
	    || ggz_read_char(game.fd, &ro) < 0
	    || ggz_read_char(game.fd, &co) < 0
	    || ggz_read_char(game.fd, &rd) < 0
	    || ggz_read_char(game.fd, &cd) < 0)
		return -1;

	game_opponent_move(seat, ro, co, rd, cd);

	return 0;
}


static int get_move_response(void)
{
	char status;

	if (ggz_read_char(game.fd, &status) < 0)
		return -1;

	switch (status) {
	case 0:
		display_statusbar(_
				  ("Move accepted. Waiting for opponents"));
		break;
	case CC_ERR_STATE:
		display_statusbar(_
				  ("Cannot accept move until table is full"));
		ggz_write_int(game.fd, CC_REQ_SYNC);
		game.my_turn = 0;
		break;
	default:
		ggz_error_msg("Err, CC_RSP_MOVE = %d", (int)status);
		return status;
		break;
	}

	return 0;
}


static int get_gameover_msg(void)
{
	char winner;
	char *msg;

	if (ggz_read_char(game.fd, &winner) < 0)
		return -1;

	if (winner == game.me)
		msg = _("Game over!  You won!");
	else
		msg = g_strdup_printf(_("Game over!  %s won!"),
				      game.names[(int)winner]);
	display_statusbar(msg);
	if (winner != game.me)
		g_free(msg);

	return 0;
}


static int get_sync_data(void)
{
	int i, j;

	for (i = 0; i < 17; i++)
		for (j = 0; j < 25; j++)
			if (ggz_read_char(game.fd, &game.board[i][j]) < 0)
				return -1;

	display_refresh_board();

	return 0;
}
