/*
 * File: game.h
 * Author: Josef Spillner
 * Original Author: Brent Hendricks
 * Project: GGZ Hastings1066 game module
 * Date: 9/10/00
 * Desc: Hastings1066 game functions
 * $Id: game.h 5269 2002-12-02 00:29:39Z jdorje $
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

#ifndef HASTINGS_GAME_H
#define HASTINGS_GAME_H

/* Hastings1066 protocol */
/* Messages from server */
#define HASTINGS_MSG_SEAT     0
#define HASTINGS_MSG_PLAYERS  1
#define HASTINGS_MSG_MOVE     2
#define HASTINGS_MSG_GAMEOVER 3
#define HASTINGS_REQ_MOVE     4
#define HASTINGS_RSP_MOVE     5
#define HASTINGS_SND_SYNC     6
#define HASTINGS_MSG_MAPS     7

/* Move errors */
#define HASTINGS_ERR_STATE   -1
#define HASTINGS_ERR_TURN    -2
#define HASTINGS_ERR_BOUND   -3
#define HASTINGS_ERR_EMPTY   -4
#define HASTINGS_ERR_FULL    -5
#define HASTINGS_ERR_DIST    -6
#define HASTINGS_ERR_MAP     -7

/* Messages from client */
#define HASTINGS_SND_MOVE     0
#define HASTINGS_REQ_SYNC     1
#define HASTINGS_REQ_INIT     2
#define HASTINGS_SND_MAP      3

/* Hastings client game states */
#define STATE_INIT       0
#define STATE_WAIT       1
#define STATE_SELECT     2
#define STATE_MOVE       3
#define STATE_DONE       4
#define STATE_PREINIT    5
#define STATE_MAPS       6

/* Data structure for the Game */
struct game_state_t {
	/* Basic info about connection */
	int fd; /* file descriptor */
	int num; /* current player */
	int seats[8]; /* maximum 8 seats */
	int parties[8]; /* black or white, bad or good */
	int teams[8]; /* countries, earldoms, nations */
	char knightnames[8][17]; /* player names */
	char teamnames[8][17]; /* team names */

	/* Hastings1066 game specifics */
	char boardmap[6][19]; /* the game board with hexagons */
	char board[6][19]; /* the game board with units */
	char state; /* current state */
	int move_src_x, move_src_y; /* used for moves: where a unit comes from */
	int move_dst_x, move_dst_y; /* used for moves: where a unit goes to */
	char playercount; /* number of players (isn't this already stored somewhere?) */
	int playernum;
	int self;
};

struct hastings_map_t {
	char board[10][30];
	char boardmap[10][30];
	char *author;
	char *version;
	char *title;
	int height, width;
};

/* Setup functions */
void game_handle_io(gpointer data, gint fd, GdkInputCondition cond);
void game_init(void);

/* Functions to handle incoming data from server*/
int get_seat(void);
int get_players(void);
int get_move_status(void);
int get_opponent_move(void);
int get_sync(void);
int get_gameover(void);

/* Functions to send data to server */
int send_options(void);
int send_my_move(void);

/* Requests */
int request_sync(void);

#endif

