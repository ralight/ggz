/*
 * File: main.c
 * Author: Ismael Orenstein
 * Project: GGZ Reversi game module
 * Date: 09/17/2000
 * Desc: Reversi client main game loop
 * $Id: main.c 6333 2004-11-12 02:27:20Z jdorje $
 *
 * Copyright (C) 2000-2002 Ismael Orenstein.
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
#  include <config.h>	/* Site-specific config */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <gtk/gtk.h>

#include <ggz.h>
#include <ggzmod.h>
#include <ggz_common.h>

#include "ggz_gtk.h"

#include "game.h"
#include "support.h"

/* main window widget */
extern GtkWidget *main_win;

/* Global game variables */
struct game_state_t game;

static void initialize_debugging(void);
static void cleanup_debugging(void);
static void initialize_about_dialog(void);

static void load_data(void);
static void save_data(void);

static GGZMod *mod;

static gboolean game_handle_io(GGZMod * mod)
{
	int op = -1;

	game.fd = ggzmod_get_server_fd(mod);

	// Read the fd
	if (ggz_read_int(game.fd, &op) < 0) {
		ggz_error_msg("Couldn't read the game fd");
		return FALSE;
	}

	switch (op) {
	case RVR_MSG_SEAT:
		get_seat();
		break;
	case RVR_MSG_PLAYERS:
		get_players();
		game.state = RVR_STATE_WAIT;
		break;
	case RVR_MSG_SYNC:
		get_sync();
		display_board();
		break;
	case RVR_MSG_START:
		game.state = RVR_STATE_PLAYING;
		ggz_debug("main", "Game has started");
		display_board();
		break;
	case RVR_MSG_MOVE:
		get_move();
		display_board();
		break;
	case RVR_MSG_GAMEOVER:
		get_gameover();
		game.state = RVR_STATE_DONE;
		break;
	default:
		game_status("Ops! Wrong message: %d", op);
		break;
	}

	return TRUE;
}


int main(int argc, char *argv[])
{

	ggz_intl_init("reversi");
	initialize_debugging();
	load_data();	/* This must come before create_main_win() */

	gtk_init(&argc, &argv);
	initialize_about_dialog();
	add_pixmap_directory(".");

	main_win = create_main_win();
	gtk_widget_show(main_win);

	game_init();

	strcpy(game.names[0], "empty");
	strcpy(game.names[1], "empty");

	display_board();

	mod = init_ggz_gtk(game_handle_io);

	gtk_main();

	save_data();

	if (ggzmod_disconnect(mod) < 0)
		return -2;
	ggzmod_free(mod);

	cleanup_debugging();
	ggz_conf_cleanup();

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
	/* Debugging goes to ~/.ggz/reversi-gtk.debug */
	char *file_name =
	    g_strdup_printf("%s/.ggz/reversi-gtk.debug", getenv("HOME"));
	ggz_debug_init(debugging_types, file_name);
	g_free(file_name);

	ggz_debug("main", "Starting Reversi client.");
}

static int config_file = -1;

/* Function to load config data from a file.  It must be called before
   create_main_win(). */
static void load_data(void)
{
	char *user_conf_path;

	last_color = (GdkColor *) malloc(sizeof(GdkColor));
	back_color = (GdkColor *) malloc(sizeof(GdkColor));

	user_conf_path = g_strdup_printf("%s/.ggz/reversi-gtk.rc",
					 getenv("HOME"));
	config_file = ggz_conf_parse(user_conf_path,
				     GGZ_CONF_RDWR | GGZ_CONF_CREATE);

	/* You'd think ggz_conf_[read|write]_int could be used to load/save these,
	   but that doesn't appear to work. */

	back_color->red =
	    ggz_conf_read_int(config_file, "background color", "red",
			      29695);
	back_color->green =
	    ggz_conf_read_int(config_file, "background color", "green",
			      27391);
	back_color->blue =
	    ggz_conf_read_int(config_file, "background color", "blue",
			      44031);
	last_color->red =
	    ggz_conf_read_int(config_file, "last played color", "red",
			      36863);
	last_color->green =
	    ggz_conf_read_int(config_file, "last played color", "green",
			      34047);
	last_color->blue =
	    ggz_conf_read_int(config_file, "last played color", "blue",
			      54783);

	g_free(user_conf_path);
}

/* Function to save the config data back in a file.  It should be called
   immediately before exiting. */
static void save_data(void)
{
	GdkColor *back_color, *last_color;

	/* Write to config file */
	back_color = g_object_get_data(G_OBJECT(main_win), "back_color");
	last_color = g_object_get_data(G_OBJECT(main_win), "last_color");

	ggz_conf_write_int(config_file, "background color", "red",
			   back_color->red);
	ggz_conf_write_int(config_file, "background color", "green",
			   back_color->green);
	ggz_conf_write_int(config_file, "background color", "blue",
			   back_color->blue);
	ggz_conf_write_int(config_file, "last played color", "red",
			   last_color->red);
	ggz_conf_write_int(config_file, "last played color", "green",
			   last_color->green);
	ggz_conf_write_int(config_file, "last played color", "blue",
			   last_color->blue);

	ggz_conf_commit(config_file);
}

/* This function should be called at the end of the program to clean up
 * debugging, as necessary. */
static void cleanup_debugging(void)
{
	/* ggz_cleanup_debug writes the data out to the file and does a
	   memory check at the same time. */
	ggz_debug("main", "Shutting down Reversi client.");
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
	      "        Gtk+ Client:"
	      "\n"
	      "            Ismael Orenstein <perdig@linuxbr.com.br>\n"
	      "\n"
	      "        Game Server:\n"
	      "            Ismael Orenstein <perdig@linuxbr.com.br>\n"
	      "\n        Based on Dots code by:\n"
	      "            Rich Gade <rgade@users.sourceforge.net>\n"
	      "\n"
	      "Website:\n"
	      "        http://www.ggzgamingzone.org/games/reversi/");
	char *header;
	header = g_strdup_printf(_("GGZ Gaming Zone\n"
				   "Reversi Version %s"), VERSION);
	init_dlg_about(_("About Reversi"), header, content);
	g_free(header);
}

void game_init(void)
{
	int i;

	for (i = 0; i < 64; i++) {
		game.board[i] = EMPTY;
	}

	game.board[CART(4, 4)] = WHITE;
	game.board[CART(5, 5)] = WHITE;
	game.board[CART(4, 5)] = BLACK;
	game.board[CART(5, 4)] = BLACK;

	game.black = 2;
	game.white = 2;
	game.turn = BLACK;
	game.state = RVR_STATE_INIT;
	game.last_move = -1;


}

int get_seat(void)
{

	if (ggz_read_int(game.fd, &game.num) < 0)
		return -1;

	game.num = SEAT2PLAYER(game.num);

	game_status("Getting seat number: %d\n", game.num);

	return 0;
}

int get_players(void)
{
	int i;

	for (i = 0; i < 2; i++) {
		if (ggz_read_int(game.fd, &game.seats[i]) < 0)
			return -1;

		if (game.seats[i] != GGZ_SEAT_OPEN) {
			if (ggz_read_string
			    (game.fd, (char *)&game.names[i], 17) < 0)
				return -1;
			game_status("Player %d named: %s", i,
				    game.names[i]);
		}
	}

	return 0;

}

// Get the move from the server and makes it (don't check for validity!!)
int get_move(void)
{
	int move;

	if (ggz_read_int(game.fd, &move) < 0)
		return -1;

	if (move == RVR_ERROR_CANTMOVE) {
		// Skip move
		game.turn *= -1;
		return 1;
	}

	if (move < 0) {
		return -1;
	}

	game_make_move(game.turn, move);

	return 0;

}

// make the move
void game_make_move(int player, int move)
{
	int x = X(move), y = Y(move);

	// Make the move
	game.board[move] = player;

	// Now goes through all directions, marking the board

	game_mark_board(player, 0, -1, x, y);
	game_mark_board(player, 1, -1, x, y);
	game_mark_board(player, 1, 0, x, y);
	game_mark_board(player, 1, 1, x, y);
	game_mark_board(player, 0, 1, x, y);
	game_mark_board(player, -1, 1, x, y);
	game_mark_board(player, -1, 0, x, y);
	game_mark_board(player, -1, -1, x, y);

	// Change turn
	game.turn *= -1;

	// Update scores
	game_update_scores();

	// Update last move
	game.last_move = move;

	return;

}


void game_mark_board(int player, int vx, int vy, int x, int y)
{
	int i, j;
	if (!game_check_direction(player, vx, vy, x, y))
		return;
	for (i = x + vx, j = y + vy; GET(i, j) == -player;
	     i += vx, j += vy) {
		game.board[CART(i, j)] *= -1;
	}
	return;
}


int game_check_direction(int player, int vx, int vy, int x, int y)
{
	int i, j;
	for (i = x + vx, j = y + vy; GET(i, j) == -player;
	     i += vx, j += vy) {
		if (GET(i + vx, j + vy) == player)
			return 1;
	}
	return 0;
}

void send_my_move(int move)
{
	if (ggz_write_int(game.fd, RVR_REQ_MOVE) < 0
	    || ggz_write_int(game.fd, move) < 0) {
		game_status("Can't send move!");
		return;
	}
	ggz_debug("main", "Sent move: %d", move);
}

int request_sync(void)
{
	if (ggz_write_int(game.fd, RVR_REQ_SYNC) < 0) {
		// Not that someone would check this return value, but...
		return -1;
	} else {
		game_status("Requesting sync from the server");
	}
	return 0;
}

int get_sync(void)
{
	char fboard[64];
	char fturn;
	int i;
	if (ggz_read_char(game.fd, &fturn) < 0)
		return -1;
	for (i = 0; i < 64; i++) {
		if (ggz_read_char(game.fd, &fboard[i]) < 0)
			return -1;
	}
	// Ok, everything worked well!
	// Some sanity tests
	if (fturn > -2 && fturn < 2) {
		if (game.turn != fturn)
			game_status("You have been synchronized");
		game.turn = fturn;
	}
	for (i = 0; i < 64; i++) {
		if (fboard[i] != game.board[i])
			game_status("You have been synchoronized");
		if (fboard[i] == BLACK || fboard[i] == WHITE
		    || fboard[i] == EMPTY)
			game.board[i] = fboard[i];
	}
	game_update_scores();
	return 0;
}

void game_update_scores(void)
{
	int i;
	game.white = 0;
	game.black = 0;
	for (i = 0; i < 64; i++) {
		if (game.board[i] == WHITE)
			game.white++;
		if (game.board[i] == BLACK)
			game.black++;
	}
}
