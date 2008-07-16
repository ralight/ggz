/* 
 * File: ai.c
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 04/23/2002
 * Desc: AI for the client
 * $Id: ai.h 6293 2004-11-07 05:51:47Z jdorje $
 *
 * This file contains headers for the AI for the ggzcards client.  Drop in
 * any one of the server ai files, and it should work.  Useful for debugging.
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

#ifdef DEBUG	/* The client-side AI is just for debugging. */

#include "cards.h"

/* These definitions should not be changed when changing the AI; just change
   the code in ai.c.  Make sure these definitions match those in the server,
   in ai/game.h. */

void start_hand(void);
void alert_bid(int p, bid_t bid);
void alert_play(int p, card_t card);
void alert_trick(int winner);
bid_t get_bid(bid_t * bid_choices, int bid_count);
card_t get_play(int play_hand, bool * valid_plays);

#endif
