/*
 * File: main.c
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers Client
 * Date: 01/01/2001
 * Desc: Main loop and supporting logic
 *
 * Copyright (C) 2001 Richard Gade.
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

#include "main.h"
#include "game.h"
#include "easysock.h"
#include "protocol.h"
#include "ggz.h"
#include "display.h"


static int get_seat(void);
static int get_players(void);
static int get_opponent_move(void);
static int get_move_response(void);
static int get_gameover_msg(void);


int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	game_init();

	gtk_main();
	return 0;
}



void main_io_handler(gpointer data, gint source, GdkInputCondition cond)
{
	int op, status;

	if(es_read_int(game.fd, &op) < 0) {
		/* FIXME: do something here... */
		return;
	}

	status = 0;
	switch(op) {
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
		case CC_REQ_MOVE:
			game_notify_our_turn();
			break;
		case CC_RSP_MOVE:
			status = get_move_response();
			break;
		default:
			fprintf(stderr, "Unknown opcode received %d\n", op);
			status = -1;
			break;
	}

	if(status < 0) {
		fprintf(stderr, "Ouch!\n");
		close(game.fd);
		exit(-1);
	}
}


static int get_seat(void)
{
	if(es_read_int(game.fd, &game.players) < 0
	   || es_read_int(game.fd, &game.me) < 0)
		return -1;

	game_init_board();

	return 0;
}


static int get_players(void)
{
	int i;

	if(es_read_int(game.fd, &game.players) < 0)
		return -1;
	for(i=0; i<game.players; i++) {
		if(es_read_int(game.fd, &game.seats[i]) < 0)
			return -1;
		if(game.seats[i] != GGZ_SEAT_OPEN) {
			if(es_read_string(game.fd, (char*)&game.names[i], 17)<0)
				return -1;
			display_set_name(i, game.names[i]);
		}
	}

	game.got_players++;

	return 0;
}


static int get_opponent_move(void)
{
	int	seat;
	char	ro, co, rd, cd;

	if(es_read_int(game.fd, &seat) < 0
	   || es_read_char(game.fd, &ro) < 0
	   || es_read_char(game.fd, &co) < 0
	   || es_read_char(game.fd, &rd) < 0
	   || es_read_char(game.fd, &cd) < 0)
		return -1;

	game_opponent_move(seat, ro, co, rd, cd);

	return 0;
}


static int get_move_response(void)
{
	char	status;

	if(es_read_char(game.fd, &status) < 0)
		return -1;

	if(status == 0)
		display_statusbar("Move accepted, waiting for opponents");

	return status;
}


static int get_gameover_msg(void)
{
	char winner;
	char *msg;

	if(es_read_char(game.fd, &winner) < 0)
		return -1;

	if(winner == game.me)
		msg = "Game over!  You won!";
	else
		msg = g_strdup_printf("Game over!  %s won!",
				      game.names[(int)winner]);
	display_statusbar(msg);
	if(winner != game.me)
		g_free(msg);

	return 0;
}
