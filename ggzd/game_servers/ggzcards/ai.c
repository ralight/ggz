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

#include "ai.h"

/* they're all empty for now... */

/* this inits AI static data at the start of a hand */
void ai_start_hand()
{
	ggz_debug("ERROR: ai_start_hand: not implemented.");
}

/* this alerts the ai to someone else's bid/play */
void ai_alert_bid( player_t p, bid_t bid )
{
	ggz_debug("ERROR: ai_alert_bid: not implemented.");
}

void ai_alert_play( player_t p, card_t card )
{
	ggz_debug("ERROR: ai_alert_bid: not implemented.");
}

/* this gets a bid or play from the ai */
bid_t ai_get_bid( player_t p )
{
	bid_t bid;
	bid.bid = 0;
	ggz_debug("ERROR: ai_get_bid: not implemented.");
	return bid;
}

card_t ai_get_play( player_t p, seat_t s )
{
	ggz_debug("ERROR: ai_get_play: not implemented.");
	return UNKNOWN_CARD;
}
