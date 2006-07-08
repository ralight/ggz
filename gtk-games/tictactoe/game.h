/*
 * File: game.h
 * Author: Brent Hendricks
 * Project: GGZ Tic-Tac-Toe game module
 * Date: 4/11/00
 * Desc: TTT game functions
 * $Id: game.h 8333 2006-07-08 00:51:56Z jdorje $
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

#include <ggz_dio.h>

#include <ggzmod.h>

/* Tic-Tac-Toe protocol */
/* Messages from server */
#define TTT_MSG_SEAT     0
#define TTT_MSG_PLAYERS  1
#define TTT_MSG_MOVE     2
#define TTT_MSG_GAMEOVER 3
#define TTT_REQ_MOVE     4
#define TTT_RSP_MOVE     5
#define TTT_SND_SYNC     6

/* Move errors */
#define TTT_ERR_STATE   -1
#define TTT_ERR_TURN    -2
#define TTT_ERR_BOUND   -3
#define TTT_ERR_FULL    -4

/* Messages from client */
#define TTT_SND_MOVE     0
#define TTT_REQ_SYNC     1

/* ttt client game states */
#define STATE_INIT       0
#define STATE_WAIT       1
#define STATE_MOVE       2
#define STATE_DONE       3

/* Data structure for Tac-Toe-Game */
struct game_state_t {
	/* Basic info about connection */
	GGZMod *ggzmod;
	GGZDataIO *dio;
	int num;
	int seats[2];
	char names[2][17];

	/* TTT game specifics */
	char board[9];
	char state;
	int move;
	char move_count;
};


/* Setup functions */
void game_init(void);

/* Functions to handle incoming data from server*/
void receive_seat(void);
void receive_players(void);
void receive_move_status(void);
void receive_move(void);
void receive_sync(void);
void receive_gameover(void);

/* Functions to send data to server */
int send_options(void);
int send_my_move(void);
