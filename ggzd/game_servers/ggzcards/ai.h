/* 
 * File: ai.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: interface for AI module system
 * $Id: ai.h 2418 2001-09-09 03:42:21Z jdorje $
 *
 * This file contains the frontend for GGZCards' AI module.
 * Specific AI's are in the ai/ directory.  This file contains an array
 * of AI function sets.  Each game specifies its AI.
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

#ifndef __AI_H__
#define __AI_H__

#include <config.h>		/* Site-specific config */

#include "types.h"

#ifdef DEBUG
#  define AI_DEBUG
#endif /* DEBUG */

struct ai_function_pointers {
	char *(*get_name) (player_t);
	void (*start_hand) ();
	void (*alert_bid) (player_t, bid_t);
	void (*alert_play) (player_t, card_t);
	  bid_t(*get_bid) (player_t, bid_t *, int);
	  card_t(*get_play) (player_t, seat_t);
};

typedef enum {
	GGZ_AI_NONE = -1,
	GGZ_AI_RANDOM = 0,
	GGZ_AI_SPADES = 1
		/* others can be defined */
} ai_type_t;

/* get a name for an ai player sitting at the given spot */
const char *ai_get_name(player_t);

/* this inits AI static data at the start of a hand */
void ai_start_hand();

/* this alerts the ai to someone else's bid/play */
void ai_alert_bid(player_t, bid_t);
void ai_alert_play(player_t, card_t);

/* this gets a bid or play from the ai */
bid_t ai_get_bid(player_t, bid_t *, int);
card_t ai_get_play(player_t, seat_t);

/* Writes debugging information about the AI (if enabled). */
void ai_debug(const char *fmt, ...);

#endif /* __AI_H__ */
