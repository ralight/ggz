/* 
 * File: ai.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: interface for AI module system
 * $Id: ai.c 3347 2002-02-13 04:17:07Z jdorje $
 *
 * This file contains the frontend for GGZCards' AI module.
 * Specific AI's are in the ai/ directory.  This file contains an array
 * of AI function sets.  Each game specifies its AI.
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ai/aicommon.h"
#include "ai.h"
#include "common.h"

extern struct ai_function_pointers random_ai_funcs;
extern struct ai_function_pointers spades_ai_funcs;
extern struct ai_function_pointers suaro_ai_funcs;

struct ai_function_pointers *ai_funcs[] = { &random_ai_funcs,
	&spades_ai_funcs,
	&suaro_ai_funcs
};

const char *ai_get_name(player_t p)
{
	const char *name;
	name = ai_funcs[game.ai_type]->get_name(p);
	if (name == NULL)
		name = get_player_name(p);
	if (name == NULL)
		name = "Bot";
	return name;
}

/* this inits AI static data at the start of a hand */
void ai_start_hand()
{
	ailib_start_hand();
	ai_funcs[game.ai_type]->start_hand();
}

/* this alerts the ai to someone else's bid/play */
void ai_alert_bid(player_t p, bid_t bid)
{
	ailib_alert_bid(p, bid);
	ai_funcs[game.ai_type]->alert_bid(p, bid);
}

void ai_alert_play(player_t p, card_t card)
{
	ailib_alert_play(p, card);
	ai_funcs[game.ai_type]->alert_play(p, card);
}

/* this gets a bid or play from the ai */
bid_t ai_get_bid(player_t p, bid_t * bid_choices, int bid_count)
{
	bid_t bid =
		ai_funcs[game.ai_type]->get_bid(p, bid_choices, bid_count);
#ifdef AI_DEBUG
	char buf[100];
	int i;
	game.funcs->get_bid_text(buf, sizeof(buf), bid);
	ai_debug("AI chose to bid %s.", buf);
	for (i = 0; i < bid_count; i++)
		if (bid_choices[i].bid == bid.bid)
			break;
	if (i > bid_count)
		ai_debug("AI chose invalid bid!!!");
#endif /* AI_DEBUG */
	return bid;
}

card_t ai_get_play(player_t p, seat_t s)
{
	card_t card = ai_funcs[game.ai_type]->get_play(p, s);
#ifdef AI_DEBUG
	int i;
	ai_debug("AI selected card (%d %d %d) to play.", card.face,
		 card.suit, card.deck);
	for (i = 0; i < game.seats[s].hand.hand_size; i++)
		if (cards_equal(game.seats[s].hand.cards[i], card) &&
		    game.funcs->verify_play(card) == NULL)
			break;
	if (i > game.seats[s].hand.hand_size)
		ai_debug("AI chose invalid play!");
#endif /* AI_DEBUG */
	return card;
}

void ai_debug(const char *fmt, ...)
{
	va_list ap;
	char buf[4096];
#if 0				/* FIXME: ifdef AI_DEBUG */
	char *mode;
	player_t player;
	const char *pname;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	switch (game.state) {
	case STATE_WAIT_FOR_BID:
		mode = "BID ";
		player = game.next_bid;	/* FIXME */
		break;
	case STATE_WAIT_FOR_PLAY:
		mode = "PLAY";
		player = game.curr_play;
		break;
	default:
		mode = "????";
		player = -1;
	}

	pname = get_player_name(player);

	(void) ggzdmod_log(game.ggz, "AI-DEBUG: %s: %s: %s", mode, pname,
			   buf);
#else  /* AI_DEBUG */
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	
	(void) ggzdmod_log(game.ggz, "AI-DEBUG: %s", buf);
#endif /* AI_DEBUG */
}
