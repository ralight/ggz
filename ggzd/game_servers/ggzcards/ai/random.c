/* 
 * File: ai/random.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: a "random" AI
 * $Id: random.c 2732 2001-11-13 06:56:14Z jdorje $
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

#include "../common.h"

static char *get_name(player_t p);
static void start_hand(void);
static void alert_bid(player_t p, bid_t bid);
static void alert_play(player_t p, card_t card);
static bid_t get_bid(player_t p, bid_t * bid_choices, int bid_count);
static card_t get_play(player_t p, seat_t s);

struct ai_function_pointers random_ai_funcs = {
	get_name,
	start_hand,
	alert_bid,
	alert_play,
	get_bid,
	get_play
};

static char *get_name(player_t p)
{
	char buf[17];
	snprintf(buf, sizeof(buf), "Random %d", p);
	return ggz_strdup(buf);
}

/* this inits AI static data at the start of a hand */
static void start_hand(void)
{
	/* nothing */
}

/* this alerts the ai to someone else's bid/play */
static void alert_bid(player_t p, bid_t bid)
{
	/* nothing */
}

static void alert_play(player_t p, card_t card)
{
	/* nothing */
}

/* this gets a bid or play from the ai */
static bid_t get_bid(player_t p, bid_t * bid_choices, int bid_count)
{
	int choice = random() % bid_count;
	return bid_choices[choice];
}

static card_t get_play(player_t p, seat_t s)
{
	hand_t *hand = &game.seats[s].hand;
	int choices[hand->hand_size], i, max;

	for (i = 0; i < hand->hand_size; i++)
		choices[i] = i;

	/* This clever algorithm will search at most n times to find at
	   random a correct element from the set of n */
	for (max = hand->hand_size; max > 0; max--) {
		int choice = random() % max;
		card_t selection = hand->cards[choices[choice]];
		char *error = game.funcs->verify_play(selection);
		if (error == NULL)
			return selection;

		choices[choice] = choices[max - 1];
	}

	ggzd_debug("RANDOM-AI: get_play: " "no valid play found.");
	return UNKNOWN_CARD;
}
