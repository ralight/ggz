/* 
 * File: ai/random.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: a "random" AI
 * $Id: random.c 3491 2002-02-27 09:42:22Z jdorje $
 *
 * This file contains the AI functions for playing any game.
 * The AI routines follow the none-too-successful algorithm of
 * always picking a random move.  At least they'll work with any
 * game!
 *
 * Copyright (C) 2001-2002 Brent Hendricks.
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

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ggz.h>

#include "client.h"

#include "game.h"

void start_hand(void);
void alert_bid(int p, bid_t bid);
void alert_play(int p, card_t card);
bid_t get_bid(bid_t * bid_choices, int bid_count);
card_t get_play(int play_hand, int *valid_plays);

/* this inits AI static data at the start of a hand */
void start_hand(void)
{
	/* nothing */
}

/* this alerts the ai to someone else's bid/play */
void alert_bid(int p, bid_t bid)
{
	/* nothing */
}

void alert_play(int p, card_t card)
{
	/* nothing */
}

/* this gets a bid or play from the ai */
bid_t get_bid(bid_t * bid_choices, int bid_count)
{
	int choice = random() % bid_count;
	return bid_choices[choice];
}

card_t get_play(int play_hand, int *valid_plays)
{
	hand_t *hand = &ggzcards.players[play_hand].hand;
	int choice;
	
	choice = random() % hand->hand_size;
	
	return hand->cards[choice];
}
