


/*
 * File: random.c
 * Author: Jason Short
 * Project: A "random" ai routine
 * Date: 07/03/2001
 *
 * This file contains the AI functions for playing any game.
 * The AI routines follow the none-too-successful algorithm of
 * always picking a random move.  At least they'll work with any
 * game!
 *
 * Copyright (C) 1998 Brent Hendricks.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ai.h"
#include "../common.h"
#include "../ggz.h"
#include "../types.h"

static char* get_name(player_t p);
static void start_hand();
static void alert_bid( player_t p, bid_t bid );
static void alert_play( player_t p, card_t card );
static bid_t get_bid( player_t p );
static card_t get_play( player_t p, seat_t s );

struct ai_function_pointers random_ai_funcs = {
	get_name,
	start_hand,
	alert_bid,
	alert_play,
	get_bid,
	get_play
};

static char* get_name(player_t p)
{
	char buf[17];
	snprintf(buf, sizeof(buf), "Random %d", p);
	return strdup(buf);
}

/* this inits AI static data at the start of a hand */
static void start_hand()
{
	/* nothing */
}

/* this alerts the ai to someone else's bid/play */
static void alert_bid( player_t p, bid_t bid )
{
	/* nothing */
}

static void alert_play( player_t p, card_t card )
{
	/* nothing */
}

/* this gets a bid or play from the ai */
static bid_t get_bid( player_t p )
{
	int choice = random() % game.num_bid_choices;
	return game.bid_choices[ choice ];
}

static card_t get_play( player_t p, seat_t s )
{
	int choice;
	hand_t *hand = &game.seats[s].hand;
	card_t selection;
	char* error;

	while (1) {
		choice = random() % hand->hand_size;
		selection = hand->cards[choice];
		error = game.funcs->verify_play( selection );
		if ( error == NULL)
			return selection;
		ggz_debug("random ai: card (%d %d %d) was invalid because %s.", selection.face, selection.suit, selection.deck, error);
	}
	return UNKNOWN_CARD;
}
