/*
 * File: main.c
 * Author: Josef Spillner
 * Original Author: Brent Hendricks
 * Project: GGZ Hastings1066 game module
 * Date: 09/13/00
 * Desc: Main loop
 * $Id: main.c 2918 2001-12-17 10:11:39Z jdorje $
 *
 * Copyright (C) 2000 Josef Spillner
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>

#include <easysock.h>
#include <ggzmod.h>

#include <game.h>
#include <main_win.h>

#include "ggzintl.h"

/* main window widget */
extern GtkWidget *main_win;

/* Global game variables */
struct game_state_t game;

/* Main function: connect and set up everything */
int main(int argc, char* argv[])
{
	ggz_intl_init("hastings");
		
	gtk_init (&argc, &argv);

	main_win = create_main_win();
	gtk_widget_show(main_win);

	game.fd = ggzmod_connect();
	if (game.fd < 0) return -1;

	gdk_input_add(game.fd, GDK_INPUT_READ, game_handle_io, NULL);

	gtk_main();

/* FIXME: um, doesn't this printf get executed only at the end? --JDS */
printf("** Game started.\n");

	if (ggzmod_disconnect() < 0)
		return -2;

	return 0;
}

/* Handle input from Hastings game server */
void game_handle_io(gpointer data, gint source, GdkInputCondition cond)
{
	int op;

	if (es_read_int(game.fd, &op) < 0)
	{
		/* FIXME: do something here...*/
		return;
	}

printf("** Input arrived.\n");

	/* Distinguish between different server responses */
	switch(op)
	{
		case HASTINGS_MSG_SEAT:
			get_seat();
			break;
		case HASTINGS_MSG_PLAYERS:
			get_players();
			game_init();
			break;
		case HASTINGS_REQ_MOVE:
			game.state = STATE_SELECT;
			game_status(_("Your move: select a knight (you are number %d)"), game.self);
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

/* Read in own data: seat number */
int get_seat(void)
{
	game_status(_("Getting seat number"));

	if (es_read_int(game.fd, &game.num) < 0) return -1;

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
	if (es_read_int(game.fd, &game.playernum) < 0) return -1;
	printf("Detected %i players!!\n", game.playernum);

	/* Receive 8 players as a maximum, or less */
	for (i = 0; i < game.playernum; i++)
	{
		if (es_read_int(game.fd, &game.seats[i]) < 0) return -1;

		if(game.seats[i] != GGZ_SEAT_OPEN)
		{
			if (es_read_string(game.fd, (char*)&game.knightnames[i], 17) < 0) return -1;
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

	if ((es_read_int(game.fd, &game.num) < 0)
	|| (es_read_int(game.fd, &game.move_src_x) < 0)
	|| (es_read_int(game.fd, &game.move_src_y) < 0)
	|| (es_read_int(game.fd, &game.move_dst_x) < 0)
	|| (es_read_int(game.fd, &game.move_dst_y) < 0))
		return -1;

	/* Apply move: Clear old position, move to new one */
	game.board[game.move_src_x][game.move_src_y] = -1;
	game.board[game.move_dst_x][game.move_dst_y] = game.num;

/* DEBUG */
printf("Opponent %i: From %i/%i to %i/%i!\n", game.num, game.move_src_x, game.move_src_y, game.move_dst_x, game.move_dst_y);

	return 0;
}

int request_sync(void)
{
	game_status(_("Requesting synchronization"));

	es_write_int(game.fd, HASTINGS_REQ_SYNC);

	return 0;
}

/* Ahhh, here it is. ;) */
int get_sync(void)
{
	int i, j;
	char space;

	game_status(_("Getting re-sync"));

	if (es_read_int(game.fd, &game.num) < 0) return -1;

	game_status(_("Player %d's turn"), game.num);

        for (i = 0; i < 6; i++)
	{
		for (j = 0; j < 19; j++)
		{
			if (es_read_char(game.fd, &space) < 0) return -1;
			game.board[i][j] = space;
		}
	}
        for (i = 0; i < 6; i++)
	{
	        for (j = 0; j < 19; j++)
		{
			if (es_read_char(game.fd, &space) < 0) return -1;
			game.boardmap[i][j] = space;
		}
	}

	game_status(_("Sync completed"));

	return 0;
}

/* Handle game over situation */
int get_gameover(void)
{
	char winner;

	game_status(_("Game over"));

	if (es_read_char(game.fd, &winner) < 0) return -1;

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

	es_write_int(game.fd, HASTINGS_REQ_INIT);
}

/* Forwarding my move to the Hastings server */
int send_my_move(void)
{
	game_status(_("Sending my move: %d/%d to %d/%d"), game.move_src_x, game.move_src_y, game.move_dst_x, game.move_dst_y);

	if ((es_write_int(game.fd, HASTINGS_SND_MOVE) < 0)
	    || (es_write_int(game.fd, game.move_src_x) < 0)
	    || (es_write_int(game.fd, game.move_src_y) < 0)
	    || (es_write_int(game.fd, game.move_dst_x) < 0)
	    || (es_write_int(game.fd, game.move_dst_y) < 0))
		return -1;

	game.state = STATE_WAIT;
	game_status(_("My status: I'm waiting (what for? AI players!)"));
	return 0;
}

/* TicTacToe had this. I am too lazy to delete. */
int send_options(void)
{
	game_status("Sending NULL options");
	return (es_write_int(game.fd, 0));
}

/* Important: Know how the server has responded to move request. */
/* TODO: HASTINGS_OK_* states */
int get_move_status(void)
{
	char status;

	if (es_read_char(game.fd, &status) < 0) return -1;

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
