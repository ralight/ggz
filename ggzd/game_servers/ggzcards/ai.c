/*
 * File: ai.c
 * Author: Jason Short
 * Project: Front end for AI routines
 * Date: 07/03/2001
 *
 * This file contains the AI functions for playing spades.
 * The AI routines were adapted from Britt Yenne's spades game for
 * the palm pilot.  Thanks Britt!
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>

#include "ai.h"
#include "common.h"
#include "ggz.h"

extern struct ai_function_pointers random_ai_funcs;
extern struct ai_function_pointers spades_ai_funcs;

struct ai_function_pointers *ai_funcs[] =
	{&random_ai_funcs,
	 &spades_ai_funcs};

char* ai_get_name(player_t p)
{
	char* name;
	name = ai_funcs[game.ai_type]->get_name(p);
	if (name == NULL) {
		char buf[17];
		snprintf(buf, 17, "Bot %d", p+1);
		name = strdup(buf);
	}
	if (name == NULL)
		name = "Bot";
	return name;
}

/* this inits AI static data at the start of a hand */
void ai_start_hand()
{
	ai_funcs[game.ai_type]->start_hand();
}

/* this alerts the ai to someone else's bid/play */
void ai_alert_bid( player_t p, bid_t bid )
{
	ai_funcs[game.ai_type]->alert_bid(p, bid);
}

void ai_alert_play( player_t p, card_t card )
{
	ai_funcs[game.ai_type]->alert_play( p, card );
}

/* this gets a bid or play from the ai */
bid_t ai_get_bid( player_t p )
{
	bid_t bid = ai_funcs[game.ai_type]->get_bid( p );
#ifdef DEBUG
	char buf[100];
	int i;
	game.funcs->get_bid_text(buf, sizeof(buf), bid);
	ggz_debug("AI chose to bid %s.", buf);
	for (i=0; i<game.num_bid_choices; i++)
		if (game.bid_choices[i].bid == bid.bid)
			break;
	if (i > game.num_bid_choices)
		ggz_debug("AI chose invalid bid!!!");
#endif /* DEBUG */
	return bid;
}

card_t ai_get_play( player_t p, seat_t s )
{
	card_t card = ai_funcs[game.ai_type]->get_play(p, s);
#ifdef DEBUG
	int i;
	ggz_debug("AI selected card (%d %d %d) to play.", card.face, card.suit, card.deck);
	for (i=0; i<game.seats[s].hand.hand_size; i++)
		if (cards_equal(game.seats[s].hand.cards[i], card) &&
		    game.funcs->verify_play(card) == NULL)
			break;
	if (i > game.seats[s].hand.hand_size)
		ggz_debug("AI chose invalid play!");
#endif /* DEBUG */
	return card;
}
