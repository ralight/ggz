/*
 * File: main.c
 * Author: Brent Hendricks
 * Project: GGZ Tic-Tac-Toe game module
 * Date: 3/31/00
 * Desc: Main loop
 * $Id: main.c 4341 2002-08-07 06:31:57Z jdorje $
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
#  include <config.h>			/* Site-specific config */
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>

#include <ggz.h>
#include <ggz_common.h>
#include <ggzmod.h>

#include <game.h>
#include <main_win.h>

#include "ggzintl.h"

#include "support.h"

/* main window widget */
extern GtkWidget *main_win;

/* Global game variables */
struct game_state_t game;

/* Various event handlers */
static void handle_ggzmod_server(GGZMod * mod, GGZModEvent e, void *data);
static void handle_ggz(gpointer data, gint source, GdkInputCondition cond);

int main(int argc, char* argv[])
{
	ggz_intl_init("tictactoe");
		
	gtk_init (&argc, &argv);

	main_win = create_main_win();
	gtk_widget_show(main_win);

	game_init();
	display_board();
	
	ggzmod_connect(game.ggzmod);
	gdk_input_add(ggzmod_get_fd(game.ggzmod), GDK_INPUT_READ, handle_ggz, NULL);

	gtk_main();

	if (ggzmod_disconnect(game.ggzmod) < 0)
		return -1;
	
	ggzmod_free(game.ggzmod);
	return 0;
}


static void handle_ggz(gpointer data, gint source, GdkInputCondition cond)
{
	ggzmod_dispatch(game.ggzmod);
}


static void handle_ggzmod_server(GGZMod * ggzmod, GGZModEvent e, void *data)
{
	int fd = *(int*)data;

	ggzmod_set_state(game.ggzmod, GGZMOD_STATE_PLAYING);
	game.fd = fd;
	gdk_input_add(fd, GDK_INPUT_READ, game_handle_io, NULL);
}



void game_handle_io(gpointer data, gint source, GdkInputCondition cond)
{
	int op;

	if (ggz_read_int(source, &op) < 0) {
		/* FIXME: do something here...*/
		return;
	}
	
	switch(op) {
		
	case TTT_MSG_SEAT:
		get_seat();
		break;
		
	case TTT_MSG_PLAYERS:
		get_players();
		game.state = STATE_WAIT;
		break;
		
	case TTT_REQ_MOVE:
		game.state = STATE_MOVE;
		game_status(_("Your move"));
		break;
		
	case TTT_RSP_MOVE:
		get_move_status();
		display_board();
		break;
		
	case TTT_MSG_MOVE:
		get_opponent_move();
		display_board();
		break;
		
	case TTT_SND_SYNC:
		get_sync();
		display_board();
		break;
		
	case TTT_MSG_GAMEOVER:
		get_gameover();
		game.state = STATE_DONE;
		break;
	}

}		


int get_seat(void)
{
	game_status(_("Getting seat number"));

	if (ggz_read_int(game.fd, &game.num) < 0)
		return -1;
	
	return 0;
}


int get_players(void)
{
	int i;

	game_status(_("Getting player names"));
	for (i = 0; i < 2; i++) {
		if (ggz_read_int(game.fd, &game.seats[i]) < 0)
			return -1;
		
		if (game.seats[i] != GGZ_SEAT_OPEN) {
			if (ggz_read_string(game.fd, (char*)&game.names[i], 17) < 0)
				return -1;
			game_status("Player %d named: %s", i, game.names[i]);
		}
	}

	return 0;
}


int get_opponent_move(void)
{
	int move;

	game_status(_("Getting opponent's move"));
	
	if (ggz_read_int(game.fd, &move) < 0)
		return -1;

	game.board[move] = (game.num == 0 ? 'o' : 'x');	

	return 0;
}


int get_sync(void)
{
	int i;
	char turn;
	char space;
	
	game_status(_("Getting re-sync"));
	
	if (ggz_read_char(game.fd, &turn) < 0)
		return -1;

	game_status(_("Player %d's turn"), turn);
	
        for (i = 0; i < 9; i++) {
		if (ggz_read_char(game.fd, &space) < 0)
			return -1;
		game_status(_("Read a %d "), space);
		if (space >= 0)
			game.board[i] = (space == 0 ? 'x' : 'o');
	}
	
	game_status(_("Sync completed"));
	return 0;
}


int get_gameover(void)
{
	char winner;
	
	game_status(_("Game over"));

	if (ggz_read_char(game.fd, &winner) < 0)
		return -1;
	
	switch (winner) {
	case 0:
	case 1:
		game_status(_("Player %d won"), winner);
		break;
	case 2:
		game_status(_("Tie game!"));
		break;
	}

	return 0;
}


void game_init(void)
{
	int i;
	GGZMod *ggzmod;
	
	for (i = 0; i < 9; i++)
		game.board[i] = ' ';

	game.state = STATE_INIT;
	ggzmod = ggzmod_new(GGZMOD_GAME);
	ggzmod_set_handler(ggzmod, GGZMOD_EVENT_SERVER, &handle_ggzmod_server);
	game.ggzmod = ggzmod;
}


int send_my_move(void)
{
	game_status(_("Sending my move: %d"), game.move);
	if (ggz_write_int(game.fd, TTT_SND_MOVE) < 0
	    || ggz_write_int(game.fd, game.move) < 0)
		return -1;

	game.state = STATE_WAIT;
	return 0;
}


int send_options(void) 
{
	game_status(_("Sending NULL options"));
	return (ggz_write_int(game.fd, 0));
}


int get_move_status(void)
{
	char status;
	
	if (ggz_read_char(game.fd, &status) < 0)
		return -1;

	switch(status) {
	case TTT_ERR_STATE:
		game_status(_("Server not ready!!"));
		break;
	case TTT_ERR_TURN:
		game_status(_("Not my turn !!"));
		break;
	case TTT_ERR_BOUND:
		game_status(_("Move out of bounds"));
		break;
	case TTT_ERR_FULL:
		game_status(_("Space already occupied"));
		break;
	case 0:
		game_status(_("Move OK"));
		game.board[game.move] = (game.num == 0 ? 'x' : 'o');
		break;
	}

	return 0;
}
