/*
 * File: game.h
 * Author: Rich Gade (Modified for Escape by Roger Light)
 * Project: GGZ Escape game module
 * Date: 28th June 2001
 * Desc: Game functions
 * $Id: game.h 2285 2001-08-27 19:53:11Z jdorje $
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


/* Escape protocol */
/* Messages from server */
#define ESCAPE_MSG_SEAT     0
#define ESCAPE_MSG_PLAYERS  1
#define ESCAPE_MSG_MOVE     2
#define ESCAPE_MSG_GAMEOVER 3
#define ESCAPE_REQ_MOVE     4
#define ESCAPE_RSP_MOVE     5
#define ESCAPE_SND_SYNC     6
#define ESCAPE_MSG_OPTIONS  7
#define ESCAPE_REQ_OPTIONS  8
#define ESCAPE_MSG_CHAT     9
#define ESCAPE_RSP_CHAT    10

/* Move errors */
#define ESCAPE_ERR_STATE   -1
#define ESCAPE_ERR_TURN    -2
#define ESCAPE_ERR_BOUND   -3
#define ESCAPE_ERR_FULL    -4

/* Messages from client */
#define ESCAPE_SND_MOVE     0
#define ESCAPE_REQ_SYNC     1
#define ESCAPE_SND_OPTIONS  2
#define ESCAPE_REQ_NEWGAME  3

/* Escape game states */
#define ESCAPE_STATE_INIT        0
#define ESCAPE_STATE_OPTIONS     1
#define ESCAPE_STATE_WAIT        2
#define ESCAPE_STATE_PLAYING     3
#define ESCAPE_STATE_DONE        4

/* Escape game events */
#define ESCAPE_EVENT_LAUNCH      0
#define ESCAPE_EVENT_JOIN        1
#define ESCAPE_EVENT_LEAVE       2
#define ESCAPE_EVENT_OPTIONS     3
#define ESCAPE_EVENT_MOVE	   4

//typedef enum {
#define	dtEmpty    1
#define	dtPlayer1  2
#define	dtPlayer2  3
#define	dtBlocked  4
//} EscapeDot;


/* Madximum board sizes */
#define MAX_WALLWIDTH	10
#define MAX_GOALWIDTH	10
#define MAX_BOARDHEIGHT	20

/* Data structure for Escape game */
struct escape_game_t {
	char boardheight;
	char goalwidth;
	char wallwidth;
	int board[MAX_WALLWIDTH*2 + MAX_GOALWIDTH+3][MAX_BOARDHEIGHT+3][10];
	int x;
	int y;
	char state;
	char turn;
	char repeatmove;
	int opponent;
	unsigned char options[2];
	unsigned char play_again;
};

extern struct escape_game_t escape_game;

void game_init(void);
int game_update(int, void *);

int game_send_seat(int);
int game_send_players(void);
int game_send_move(int, int, char);
int game_send_sync(int);
int game_send_options(int);
int game_send_gameover(char);

int game_move(void);
int game_req_move(int);
int game_handle_move(int, unsigned char *);
int game_bot_move(void);

char game_check_move(int, int);
char game_check_win(void);


void ggz_update(ggzd_event_t event, void *data);
void game_handle_player(ggzd_event_t event, void* data);

unsigned char revdir(unsigned char direction);

