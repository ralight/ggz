/*
 * File: main.c
 * Author: Brent Hendricks
 * Project: GGZ Tic-Tac-Toe game module
 * Date: 3/31/00
 * Desc: Main loop
 * $Id: main.c 10198 2008-07-08 04:02:19Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>	/* Site-specific config */
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <gtk/gtk.h>
#include <ggz.h>
#include <ggz_common.h>
#include <ggzmod.h>

#include "ggz_gtk.h"

#include "game.h"
#include "main_win.h"
#include "support.h"
#include "modversion.h"
#include "net.h"

/* main window widget */
extern GtkWidget *main_win;

/* Global game variables */
struct game_state_t game;

static void initialize_about_dialog(void);

static void initialize_debugging(void)
{
#ifdef WIN32
	char *file_name =
	    g_strdup_printf("%s\\.ggz\\ttt-gtk.debug",
			    getenv("APPDATA"));
#else
	char *file_name =
	    g_strdup_printf("%s/.ggz/ttt-gtk.debug", getenv("HOME"));
#endif
	const char *debugging_types[] = {
#ifdef DEBUG
		DBG_MAIN, DBG_BOARD,
#endif
		NULL
	};
	ggz_debug_init(debugging_types, file_name);
	g_free(file_name);

	ggz_debug(DBG_MAIN, "Starting TTT client.");
}

static void cleanup_debugging(void)
{
	/* ggz_cleanup_debug writes the data out to the file and does a memory 
	   check at the same time. */
	ggz_debug(DBG_MAIN, "Shutting down TTT client.");
#ifdef DEBUG
	ggz_debug_cleanup(GGZ_CHECK_MEM);
#else
	ggz_debug_cleanup(GGZ_CHECK_NONE);
#endif
}

int main(int argc, char *argv[])
{
	initialize_debugging();
	ggz_intl_init("tictactoe");

	gtk_init(&argc, &argv);
	initialize_about_dialog();

	main_win = create_main_win();
	gtk_widget_show(main_win);

	game_init();
	display_board();

	init_chat(game.ggzmod);

	game_status(_("Watching the game..."));

	gtk_main();

	if (ggzmod_disconnect(game.ggzmod) < 0)
		return -1;

	ggzmod_free(game.ggzmod);
	cleanup_debugging();
	return 0;
}

static void initialize_about_dialog(void)
{
	const char *content =
	    _("Copyright (C) 2000 by Brent Hendricks\n"
	      "\n"
	      "Website: http://www.ggzgamingzone.org/gameclients/tictactoe/");
	char *header;

	header = g_strdup_printf(_("GGZ Gaming Zone\n"
				   "Tic-Tac-Toe Version %s"),
				 GAME_VERSION);
	init_dlg_about(_("About Tic-Tac-Toe"), header, content);
	g_free(header);
}

static char get_player_symbol(int player)
{
	assert(player >= -1 && player <= 1);
	if (player == 0) {
		return 'O';
	} else if (player == 1) {
		return 'X';
	} else {
		return '-';
	}
}

static void game_handle_error()
{
	/* FIXME: do something... */
}

static void game_handle_message(int opcode)
{
	switch (opcode) {

	case reqmove:
		game.state = STATE_MOVE;
		game_status(_("It's your move."));
		break;

	case rspmove:
		receive_move_status(variables.status);
		display_board();
		break;

	case msgmove:
		receive_move(variables.move, variables.player);
		display_board();
		break;

	case sndsync:
		receive_sync(variables.turn, variables.space);
		display_board();
		break;

	case msggameover:
		receive_gameover(variables.winner);
		game.state = STATE_DONE;
		break;
	}
}


static gboolean game_handle_io(GGZMod * mod)
{
	ggzcomm_set_notifier_callback(game_handle_message);
	ggzcomm_set_error_callback(game_handle_error);
	ggzcomm_network_main(ggzmod_get_server_fd(mod));

	return TRUE;
}


/* The server doesn't usually inform us of our move.  But we get told about
   the opponent's move, and if we're a spectator we get to hear both. */
void receive_move(int move, int nummove)
{
	/* nummove is the player who made the move (0 or 1). */
	/* move is the move (0..8) */
	int is_spectator, seat_num;

	ggzmod_get_player(game.ggzmod, &is_spectator, &seat_num);

	if (is_spectator)
		game_status(_("%s (%c) has moved."),
			    ggzmod_get_seat(game.ggzmod, nummove).name,
			    get_player_symbol(nummove));
	else
		game_status(_("Your opponent has moved."));

	game.board[move] = (nummove == 1 ? 'o' : 'x');
}


void receive_sync(int turn, char spaces[9])
{
	int i;
	char space;

	game_status(_("Syncing with server..."));

	game_status(_("It's %s (%c)'s turn."),
		    ggzmod_get_seat(game.ggzmod, turn).name,
		    get_player_symbol(turn));

	for (i = 0; i < 9; i++) {
		space = spaces[i];
		if (space >= 0)
			game.board[i] = (space == 0 ? 'x' : 'o');
	}

	game_status(_("Sync completed."));
}


void receive_gameover(char winner)
{
	game_status(_("Game over."));

	switch (winner) {
	case 0:
	case 1:
		game_status(_("%s (%c) won"),
			    ggzmod_get_seat(game.ggzmod, (int)winner).name,
			    get_player_symbol(winner));
		break;
	case 2:
		game_status(_("Tie game!"));
		break;
	default:
		game_status(_("Protocol error!"));
		game.state = STATE_DONE;
		break;
	}
}


void game_init(void)
{
	int i;

	for (i = 0; i < 9; i++)
		game.board[i] = ' ';

	game.state = STATE_INIT;

	game.ggzmod = init_ggz_gtk(GTK_WINDOW(main_win), game_handle_io);
}


int send_my_move(void)
{
	int is_spectator, seat_num;

	ggzmod_get_player(game.ggzmod, &is_spectator, &seat_num);

	if ((is_spectator) || (seat_num < 0))
		return -1;
	game_status(_("Sending move."));

	variables.move_c = game.move;
	ggzcomm_sndmove();

	game.state = STATE_WAIT;
	return 0;
}

int send_options(void)
{
	game_status(_("Sending options."));
	assert(0);
	return 0;
}

void receive_move_status(char status)
{
	int is_spectator, seat_num;

	ggzmod_get_player(game.ggzmod, &is_spectator, &seat_num);

	switch (status) {
	case TTT_ERR_STATE:
		game_status(_("Server not ready!!"));
		break;
	case TTT_ERR_TURN:
		game_status(_("Not your turn!"));
		break;
	case TTT_ERR_BOUND:
		game_status(_("Move out of bounds!"));
		break;
	case TTT_ERR_FULL:
		game_status(_("Space already occupied!"));
		break;
	case 0:
		game_status(_("Move accepted."));
		game.board[game.move] = (seat_num == 0 ? 'x' : 'o');
		break;
	}
}
