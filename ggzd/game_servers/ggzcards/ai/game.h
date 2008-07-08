/*
 * File: ai/game.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 02/10/2002
 * Desc: Client-callback routines for the AI functions
 * $Id: game.h 10186 2008-07-08 03:15:31Z jdorje $
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

#include "shared.h"

#include "client.h"

/* Standard debugging types */
#define DBG_BID "bid"
#define DBG_PLAY "play"
#define DBG_AI "misc"

void start_hand(void);
void alert_bid(int player, bid_t bid);
void alert_play(int player, card_t card);
void alert_trick(int winner);
bid_t get_bid(bid_t * bid_choices, int bid_count);
card_t get_play(int play_hand, bool * valid_plays);
