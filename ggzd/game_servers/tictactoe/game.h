/*
 * File: game.h
 * Author: Brent Hendricks
 * Project: GGZ Tic-Tac-Toe game module
 * Date: 3/31/00
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

#include "../libggzmod/ggz_server.h"


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

/* Tic-Tac-Toe game states */
#define TTT_STATE_INIT        0
#define TTT_STATE_WAIT        1
#define TTT_STATE_PLAYING     2
#define TTT_STATE_DONE        3

/* Tic-Tac-Toe game events */
#define TTT_EVENT_LAUNCH      0
#define TTT_EVENT_JOIN        1
#define TTT_EVENT_LEAVE       2
#define TTT_EVENT_MOVE        3


/* Data structure for Tac-Toe-Game */
struct ttt_game_t {
	char board[9];
	char state;
	char turn;
	char move_count;
};

void game_init(void);
void game_handle_ggz(ggzd_event_t event, void *data);
void game_handle_player(ggzd_event_t event, void *data);

int game_send_seat(int seat);
int game_send_players(void);
int game_send_move(int num, int move);
int game_send_sync(int num);
int game_send_gameover(char winner);

int game_move(void);
int game_req_move(int num);
int game_handle_move(int num, int* move);
int game_bot_move(int num);

char game_check_move(int num, int move);
char game_check_win(void);

int game_update(int event, void* data);

