/*
 * File: game.h
 * Author: Rich Gade
 * Project: GGZ La Pocha game module
 * Date: 06/29/2000
 * Desc: Game functions
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

#ifndef GGZ_CARDS_INCLUDED
#  include "cards.h"
#endif

/* La Pocha protocol */
/* Messages from server */
#define LP_MSG_SEAT     0
#define LP_MSG_PLAYERS  1
#define LP_MSG_GAMEOVER 2
#define LP_MSG_HAND     3
#define LP_REQ_BID	4
#define LP_RSP_BID	5
#define LP_MSG_BID	6
#define LP_REQ_PLAY     7
#define LP_RSP_PLAY     8
#define LP_MSG_PLAY	9
#define LP_SND_SYNC     10

/* Play errors */
#define LP_ERR_STATE   -1
#define LP_ERR_TURN    -2
#define LP_ERR_INVALID -3

/* Messages from client */
#define LP_SND_PLAY     0
#define LP_SND_BID      1
#define LP_REQ_SYNC     2
#define LP_REQ_NEWGAME  3

/* La Pocha game states */
#define LP_STATE_INIT        0
#define LP_STATE_WAIT        1
#define LP_STATE_NEW_HAND    2
#define LP_STATE_PLAYING     3
#define LP_STATE_DONE        4

/* La Pocha game events */
#define LP_EVENT_LAUNCH      0
#define LP_EVENT_JOIN        1
#define LP_EVENT_LEAVE       2
#define LP_EVENT_BID	     3
#define LP_EVENT_PLAY        4

/* Data structure for La Pocha game */
struct lp_game_t {
	char state;
	char turn;
	char dealer;
	char hand_num;
	int score[4];
	struct hand_t hand[4];
};

extern struct lp_game_t game;

extern void game_init(void);
extern int game_handle_ggz(int, int *);
extern int game_handle_player(int);
