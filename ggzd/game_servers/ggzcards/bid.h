/* 
 * File: bid.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/13/2001
 * Desc: Functions and data for bidding system
 * $Id: bid.h 2731 2001-11-13 06:50:06Z jdorje $
 *
 * Copyright (C) 2001 Brent Hendricks.
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-130
 */

#include <config.h>		/* Site-specific config */

#include "types.h"

/* clear_bids clears the list of possible bids. It's called by
   handle_bid_event().  Thus, after a player responds to one bid request the
   game can start setting up their next bid. */
void clear_bids(player_t p);

/* add_bid and add_sbid add another bid to the list of possible bids.
   add_sbid is just a convenience.  They add to the bid list for the player
   noted by game.next_bid - this makes things easier for most games. */
void add_bid(bid_t bid);
void add_sbid(char val, char suit, char spec);

/* req_bid requests a bid from the client _or_ AI. */
int req_bid(player_t p);

/* request_all_bids requests bids from all players whom have pending bid
   lists. */
int request_all_bids(void);

/* rec_bid receives a bid from the client */
int rec_bid(player_t p, bid_t * bid);
