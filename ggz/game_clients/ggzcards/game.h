/*
 * File: game.h
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Handles user-interaction with game screen
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

#include "hand.h"

#ifndef __GAME_H
#define __GAME_H

/* if this is defined, the animation code will be used.
 * I've disable it because I broke it... */
/* #define ANIMATION */

struct seat_t {
 	int seat;						/* ggz seating info */
	char name[17];			/* player's name */
	char message[100]; 	/* message info */
	card_t table_card; /* card on table, index into hand */
	struct hand_t hand; /* player's hand */
};

struct game_t {
	int fd;			/* the socket for the server connection */
	char state;		/* the state the game is, i.e. LA_STATE_<something> */
	int play_hand;		/* the hand we're playing from */
	int num_players;	/* starts at 0 so we know once we've received them */
	struct seat_t *players;	/* player info; must be allocated */
};
extern struct game_t game;

extern void game_send_bid(int);
extern void game_send_options(int option_cnt, int* options);
extern void game_play_card(card_t card);
extern void game_handle_table_click(char);
extern void set_game_state(char);

/* NOTE: the following protocol information is copied from server code */
/* GGZCards protocol */
/* Messages from server */
/* TODO: numbers aren't finalized */
#define WH_REQ_NEWGAME		0
#define WH_MSG_NEWGAME		1
#define WH_MSG_GAMEOVER		2
#define WH_MSG_PLAYERS		3
#define WH_MSG_HAND     	4
#define WH_REQ_BID		5
#define WH_REQ_PLAY     	6
#define WH_MSG_BADPLAY 		7
#define WH_MSG_PLAY		8
#define WH_MSG_TRICK		9
#define WH_MESSAGE_GLOBAL	10
#define WH_MESSAGE_PLAYER	11
#define WH_REQ_OPTIONS		12
#define WH_MSG_TABLE		13

/* Messages from client */
#define WH_RSP_NEWGAME		0
#define WH_RSP_OPTIONS		1
#define WH_RSP_PLAY		2
#define WH_RSP_BID		3
#define WH_REQ_SYNC		4






/* GGZCards client game states */
#define WH_STATE_INIT	  	0 /* game hasn't started yet */
#define WH_STATE_WAIT	  	1 /* waiting for others */
#define WH_STATE_PLAY		2 /* our turn to play */
#define WH_STATE_BID		3 /* our turn to bid */
#ifdef ANIMATION
#define WH_STATE_ANIM		4 /* we're animating stuff */
#endif /* ANIMATION */
#define WH_STATE_DONE		5 /* game's over */
#define WH_STATE_OPTIONS	6 /* determining options */

/* GGZ define */
#define GGZ_SEAT_OPEN		-1

/* substitutes for snprintf.
 * Returns the number of characters put into the buffer. */
extern int ggz_snprintf(char*, int, char *, ...);
void ggz_debug(const char *fmt, ...);

/* preliminary internationalization macros */
#define _(String) (String)
#define N_(String) (String)
#define textdomain(Domain)
#define bindtextdomain(Package, Directory)

#endif /* __GAME_H */


