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

#include <assert.h>

#include "hand.h"
#include "protocol.h"

#ifndef __GAME_H__
#define __GAME_H__

/* if this is defined, the animation code will be used.
 * I've disable it because I broke it... */
/* #define ANIMATION */

/* GGZCards client game states */
typedef enum {
	WH_STATE_INIT,	  	/* game hasn't started yet */
	WH_STATE_WAIT,	  	/* waiting for others */
	WH_STATE_PLAY,		/* our turn to play */
	WH_STATE_BID,		/* our turn to bid */
#ifdef ANIMATION
	WH_STATE_ANIM,		/* we're animating stuff */
#endif /* ANIMATION */
	WH_STATE_DONE,		/* game's over */
	WH_STATE_OPTIONS	/* determining options */
} client_state_t;

struct seat_t {
 	int seat;		/* ggz seating info */
	char name[17];		/* player's name */
	char message[100];	/* message info */
	card_t table_card;	/* card on table, index into hand */
	struct hand_t hand;	/* player's hand */
};

struct game_t {
	int fd;			/* the socket for the server connection */
	client_state_t state;	/* the state the game is, i.e. LA_STATE_<something> */
	int play_hand;		/* the hand we're playing from */
	int num_players;	/* starts at 0 so we know once we've received them */
	int max_hand_size;	/* the maximum number of cards in a hand */
	struct seat_t *players;	/* player info; must be allocated */
};
extern struct game_t game;

extern void game_send_bid(int);
extern void game_send_options(int option_cnt, int* options);
extern void game_play_card(card_t card);
extern void game_handle_table_click(char);
extern void set_game_state(client_state_t);

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

#endif /* __GAME_H__ */


