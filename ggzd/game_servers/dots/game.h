/*
 * File: game.h
 * Author: Rich Gade
 * Project: GGZ Connect the Dots game module
 * Date: 04/27/2000
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


/* Connect the Dots protocol */
/* Messages from server */
#define DOTS_MSG_SEAT     0
#define DOTS_MSG_PLAYERS  1
#define DOTS_MSG_MOVE_H   2
#define DOTS_MSG_MOVE_V   3
#define DOTS_MSG_GAMEOVER 4
#define DOTS_REQ_MOVE     5
#define DOTS_RSP_MOVE     6
#define DOTS_SND_SYNC     7
#define DOTS_MSG_OPTIONS  8

/* Move errors */
#define DOTS_ERR_STATE   -1
#define DOTS_ERR_TURN    -2
#define DOTS_ERR_BOUND   -3
#define DOTS_ERR_FULL    -4

/* Messages from client */
#define DOTS_SND_MOVE_H   0
#define DOTS_SND_MOVE_V   1
#define DOTS_REQ_SYNC     2

/* Connect the Dots game states */
#define DOTS_STATE_INIT        0
#define DOTS_STATE_WAIT        1
#define DOTS_STATE_PLAYING     2
#define DOTS_STATE_DONE        3

/* Connect the Dots game events */
#define DOTS_EVENT_LAUNCH      0
#define DOTS_EVENT_JOIN        1
#define DOTS_EVENT_LEAVE       2
#define DOTS_EVENT_MOVE_H      3
#define DOTS_EVENT_MOVE_V      4


/* Madximum board sizes */
#define MAX_BOARD_WIDTH		25
#define MAX_BOARD_HEIGHT	25

/* Data structure for Connect the Dots game */
struct dots_game_t {
	unsigned char vert_board[MAX_BOARD_WIDTH][MAX_BOARD_HEIGHT-1];
	unsigned char horz_board[MAX_BOARD_WIDTH-1][MAX_BOARD_HEIGHT];
	unsigned char board_height, board_width;
	char state;
	char turn;
	int score[2];
	int opponent;
	unsigned char options[2];
};

extern struct dots_game_t dots_game;

void game_init(void);
int game_handle_ggz(int, int *);
int game_handle_player(int);

int game_send_seat(int);
int game_send_players(void);
int game_send_move(int, int, char, char);
int game_send_sync(int);
int game_send_options(int);
int game_send_gameover(char);

int game_move(void);
int game_req_move(int);
int game_handle_move(int, int, unsigned char *, unsigned char *);
int game_bot_move(void);

char game_check_move(int, int);
char game_check_win(void);

int game_update(int, void *, void *);
