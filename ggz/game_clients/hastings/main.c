/*
 * File: main.c
 * Author: Josef Spillner
 * Original Author: Brent Hendricks
 * Project: GGZ Hastings1066 game module
 * Date: 09/13/00
 * Desc: Main loop
 * $Id: main.c 5233 2002-11-06 22:11:22Z dr_maux $
 *
 * Copyright (C) 2000 - 2002 Josef Spillner
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

/* System includes */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>	/* for getenv */
#include <unistd.h>

/* GGZ includes */
#include <ggz.h>
#include <ggz_common.h>
#include <ggzmod.h>

/* GTK-games includes */
#include "dlg_about.h"
#include "dlg_players.h"
#include "selector.h"
#include "ggzintl.h"

/* Hastings includes */
#include "game.h"
#include "main_win.h"

/* main window widget */
extern GtkWidget *main_win;

/* Global game variables */
struct game_state_t game;

static void initialize_debugging(void);
static void cleanup_debugging(void);
static void initialize_about_dialog(void);

static GGZMod *mod;

static void handle_ggz(gpointer data, gint source, GdkInputCondition cond)
{
	ggzmod_dispatch(mod);
}

static void handle_ggzmod_server(GGZMod *mod, GGZModEvent e, void *data)
{
	int fd = *(int*)data;

	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);
	game.fd = fd;
	gdk_input_add(fd, GDK_INPUT_READ, game_handle_io, NULL);
}

/* Main function: connect and set up everything */
int main(int argc, char* argv[])
{
	initialize_debugging();
	ggz_intl_init("hastings");
		
	gtk_init (&argc, &argv);
	initialize_about_dialog();

	main_win = create_main_win();
	gtk_widget_show(main_win);

	mod = ggzmod_new(GGZMOD_GAME);
	ggzmod_set_handler(mod, GGZMOD_EVENT_SERVER, &handle_ggzmod_server);
	init_player_list(mod);

	ggzmod_connect(mod);

	gdk_input_add(ggzmod_get_fd(mod), GDK_INPUT_READ, handle_ggz, NULL);

	gtk_main();

	if (ggzmod_disconnect(mod) < 0)
		return -2;
	ggzmod_free(mod);

	cleanup_debugging();

	return 0;
}

static void initialize_debugging(void)
{
	/* Our debugging code uses libggz's ggz_debug() function, so we
	   just initialize the _types_ of debugging we want. */
#ifdef DEBUG
	const char *debugging_types[] = { "main", "debug", NULL };
#else
	const char *debugging_types[] = { NULL };
#endif
	/* Debugging goes to ~/.ggz/hastings-gtk.debug */
	char *file_name =
		g_strdup_printf("%s/.ggz/hastings-gtk.debug", getenv("HOME"));
	ggz_debug_init(debugging_types, file_name);
	g_free(file_name);

	ggz_debug("main", "Starting hastings client.");	
}

/* This function should be called at the end of the program to clean up
 * debugging, as necessary. */
static void cleanup_debugging(void)
{
	/* ggz_cleanup_debug writes the data out to the file and does a
	   memory check at the same time. */
	ggz_debug("main", "Shutting down hastings client.");
#ifdef DEBUG
	ggz_debug_cleanup(GGZ_CHECK_MEM);
#else
	ggz_debug_cleanup(GGZ_CHECK_NONE);
#endif
}

static void initialize_about_dialog(void)
{
	const char *content =
	  _("Authors:\n"
	    "        Gtk+ Client:\n"
	    "            Josef Spillner   <dr_maux@users.sourceforge.net>\n"
	    "\n"
	    "        Game Server:\n"
	    "            Josef Spillner   <dr_maux@users.sourceforge.net>\n"
	    "\n"
	    "Website:\n"
	    "        http://ggz.sourceforge.net/games/hastings/");
	char *header;

	header = g_strdup_printf(_("GGZ Gaming Zone\n"
				 "Hastings1066 Version %s"), VERSION);
	init_dlg_about(_("About Hastings1066"), header, content);
	g_free(header);
}

/* Handle input from Hastings game server */
void game_handle_io(gpointer data, gint source, GdkInputCondition cond)
{
	int op;

	if (ggz_read_int(game.fd, &op) < 0)
	{
		/* FIXME: do something here...*/
		return;
	}

	/* Distinguish between different server responses */
	switch(op)
	{
		case HASTINGS_MSG_MAPS:
			get_maps();
			break;
		case HASTINGS_MSG_SEAT:
			get_seat();
			break;
		case HASTINGS_MSG_PLAYERS:
			get_players();
			game_init();
			break;
		case HASTINGS_REQ_MOVE:
			game.state = STATE_SELECT;
			game_status(_("Your move: select a knight "
			              "(you are %s on the %s team)."),
			            _(player_colors[game.self / 2]),
			            _(team_colors[game.self % 2]));
			break;
		case HASTINGS_RSP_MOVE:
			get_move_status();
			display_board();
			break;
		case HASTINGS_MSG_MOVE:
			get_opponent_move();
			display_board();
			break;
		case HASTINGS_SND_SYNC:
			get_sync();
			if(game.state == STATE_PREINIT) game.state = STATE_INIT;
			display_board();
			break;
		case HASTINGS_MSG_GAMEOVER:
			get_gameover();
			game.state = STATE_DONE;
			break;
	}
}

int get_maps(void)
{
	GtkWidget *w;

	w = selector();
}

/* Read in own data: seat number */
int get_seat(void)
{
	game_status(_("Getting seat number"));

	if (ggz_read_int(game.fd, &game.num) < 0) return -1;

	game_status(_("Received seat number: %i"), game.num);

	/* Is this necessary? TODO!! */
	game.self = game.num;

	return 0;
}

/* Receive the player names */
int get_players(void)
{
	int i;

	game_status(_("Getting player names"));

	/* Number unknown; this will change now: */
	if (ggz_read_int(game.fd, &game.playernum) < 0) return -1;
	ggz_debug("main", "Detected %i players!!", game.playernum);

	/* Receive 8 players as a maximum, or less */
	for (i = 0; i < game.playernum; i++)
	{
		if (ggz_read_int(game.fd, &game.seats[i]) < 0) return -1;

		if(game.seats[i] != GGZ_SEAT_OPEN)
		{
			if (ggz_read_string(game.fd, (char*)&game.knightnames[i], 17) < 0) return -1;
			game_status(_("Player %d named: %s"), i, game.knightnames[i]);
		}
	}

	/* If not starting player, inform about */
	game_status(_("Your turn will be soon... have a look at the map in the meantime."));

	return 0;
}

/* Enemy has moved his guy */
int get_opponent_move(void)
{
	game_status(_("Getting opponent's move"));

	if ((ggz_read_int(game.fd, &game.num) < 0)
	|| (ggz_read_int(game.fd, &game.move_src_x) < 0)
	|| (ggz_read_int(game.fd, &game.move_src_y) < 0)
	|| (ggz_read_int(game.fd, &game.move_dst_x) < 0)
	|| (ggz_read_int(game.fd, &game.move_dst_y) < 0))
		return -1;

	/* Apply move: Clear old position, move to new one */
	game.board[game.move_src_x][game.move_src_y] = -1;
	game.board[game.move_dst_x][game.move_dst_y] = game.num;

	ggz_debug("debug", "Opponent %i: From %i/%i to %i/%i!", game.num,
		game.move_src_x, game.move_src_y, game.move_dst_x, game.move_dst_y);

	return 0;
}

/* Request synchronization from the server */
int request_sync(void)
{
	game_status(_("Requesting synchronization"));

	ggz_write_int(game.fd, HASTINGS_REQ_SYNC);

	return 0;
}

/* Ahhh, here it is. ;) */
int get_sync(void)
{
	int i, j;
	char space;

	game_status(_("Getting re-sync"));

	if (ggz_read_int(game.fd, &game.num) < 0) return -1;

	game_status(_("Player %d's turn"), game.num);

	for (i = 0; i < 6; i++)
		for (j = 0; j < 19; j++)
		{
			if (ggz_read_char(game.fd, &space) < 0) return -1;
			game.board[i][j] = space;
		}

	for (i = 0; i < 6; i++)
		for (j = 0; j < 19; j++)
		{
			if (ggz_read_char(game.fd, &space) < 0) return -1;
			game.boardmap[i][j] = space;
		}

	game_status(_("Sync completed"));

	return 0;
}

/* Handle game over situation */
int get_gameover(void)
{
	char winner;

	game_status(_("Game over"));

	if (ggz_read_char(game.fd, &winner) < 0) return -1;

	switch (winner)
	{
		case 0:
		case 1:
			game_status(_("Player %d won"), winner);
			break;
	}

	return 0;
}

/* Initialize game board. Actually: Get it from server. */
void game_init(void)
{
	game.state = STATE_PREINIT;

	game_status(_("Requesting game initialization"));

	ggz_write_int(game.fd, HASTINGS_REQ_INIT);
}

/* Forwarding my move to the Hastings server */
int send_my_move(void)
{
	game_status(_("Sending my move: %d/%d to %d/%d"),
		game.move_src_x, game.move_src_y, game.move_dst_x, game.move_dst_y);

	if ((ggz_write_int(game.fd, HASTINGS_SND_MOVE) < 0)
	    || (ggz_write_int(game.fd, game.move_src_x) < 0)
	    || (ggz_write_int(game.fd, game.move_src_y) < 0)
	    || (ggz_write_int(game.fd, game.move_dst_x) < 0)
	    || (ggz_write_int(game.fd, game.move_dst_y) < 0))
		return -1;

	game.state = STATE_WAIT;
	game_status(_("My status: I'm waiting (what for? AI players!)"));
	return 0;
}

/* TicTacToe had this. I am too lazy to delete. */
int send_options(void)
{
	game_status("Sending NULL options");
	return (ggz_write_int(game.fd, 0));
}

/* Important: Know how the server has responded to move request. */
/* TODO: HASTINGS_OK_* states */
int get_move_status(void)
{
	char status;

	if (ggz_read_char(game.fd, &status) < 0) return -1;

	switch(status)
	{
		case HASTINGS_ERR_STATE:
			game_status(_("Server not ready!!"));
			break;
		case HASTINGS_ERR_TURN:
			game_status(_("Not my turn !!"));
			break;
		case HASTINGS_ERR_BOUND:
			game_status(_("Move out of bounds"));
			break;
		case HASTINGS_ERR_EMPTY:
			game_status(_("Nothing to move (erm?)"));
			break;
		case HASTINGS_ERR_FULL:
			game_status(_("Space already occupied"));
			break;
		case HASTINGS_ERR_DIST:
			game_status(_("Hey, that is far too far!"));
			break;
		case HASTINGS_ERR_MAP:
			game_status(_("Argh! You should play 'sink the ship' for that purpose!"));
			break;
		case 0:
			game_status(_("Moved knight from %i/%i to %i/%i"),
				game.move_src_x, game.move_src_y, game.move_dst_x, game.move_dst_y);
			game.board[game.move_src_x][game.move_src_y] = -1;
			game.board[game.move_dst_x][game.move_dst_y] = game.self;
	}

	game.state = STATE_SELECT;

	return 0;
}

