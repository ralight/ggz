/*
 * File: game.h
 * Author: Josef Spillner
 * Original Author: Brent Hendricks
 * Project: GGZ Hastings1066 game module
 * Date: 09/10/00
 * Desc: Game functions
 * $Id: game.h 6892 2005-01-25 04:09:21Z jdorje $
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

/* GGZdmod includes */
#include "ggzdmod.h"

/* Hastings protocol */
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
#define HASTINGS_ERR_ENEMY   -8

/* Messages from client */
#define HASTINGS_SND_MOVE     0
#define HASTINGS_REQ_SYNC     1
#define HASTINGS_REQ_INIT     2
#define HASTINGS_SND_MAP      3

/* Hastings game states */
#define HASTINGS_STATE_INIT        0
#define HASTINGS_STATE_WAIT        1
#define HASTINGS_STATE_PLAYING     2
#define HASTINGS_STATE_DONE        3
#define HASTINGS_STATE_MAPS        4
#define HASTINGS_STATE_WAITFORMAP  5

/* Hastings game events */
#define HASTINGS_EVENT_LAUNCH      0
#define HASTINGS_EVENT_JOIN        1
#define HASTINGS_EVENT_LEAVE       2
#define HASTINGS_EVENT_MOVE        3

/* Map format*/
#define HASTINGS_MAP_FORMAT "0.0.9"

/* Data structure for Hastings1066 */
struct hastings_game_t {
	GGZdMod *ggz; /* GGZ state object */
	char state; /* current state */
	int turn; /* who has to move? */
	int move_src_x, move_src_y; /* used for moves: where a unit comes from */
	int move_dst_x, move_dst_y; /* used for moves: where a unit goes to */
	char playernum; /* number of players (isn't this already stored somewhere?) */ /*dirty hack!*/
	int players[8]; /* 1 for active player, 0 for dead one */

	int parties[8]; /* e.g. countries */
	int teams[8]; /* 1 or 0, north or south, ... */
	char teamnames[8]; /* to be named */
};

struct hastings_map_t {
	char board[10][30];
	char boardmap[10][30];
	char *author;
	char *version;
	char *title;
	char *graphics;
	int height, width;
	int players;
};

void game_init(GGZdMod *ggz);
void game_handle_ggz(GGZdMod *ggz, GGZdModEvent event, const void *data);
void game_handle_player(GGZdMod *ggz, GGZdModEvent event,
			const void *seat_data);
void game_handle_spectator(GGZdMod *ggz, GGZdModEvent event,
			   const void *spectator_data);

int game_send_seat(int seat);
int game_send_players(void);
int game_send_move(int num);
int game_send_sync(int num, int fd);
int game_send_gameover(char winner);

int game_move(void);
int game_req_move(int num);
int game_handle_move(int num);
void game_bot_move(int me);

char game_check_move(int num, int enemyallowed);
char game_check_win(void);

int game_update(int event, void* data);

int game_setupmap(int seat);

#endif

