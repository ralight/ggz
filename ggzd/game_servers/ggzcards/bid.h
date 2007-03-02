/* 
 * File: bid.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/13/2001
 * Desc: Functions and data for bidding system
 * $Id: bid.h 8996 2007-03-02 23:19:59Z jdorje $
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-130
 */

#include "types.h"

bool is_anyone_bidding(void);

/* clear_bids clears the list of possible bids. It's called by
   handle_bid_event().  Thus, after a player responds to one bid request the
   game can start setting up their next bid. */
void clear_bids(player_t p);

/* add_bid and add_sbid add another bid to the list of possible bids.
   add_sbid is just a convenience.  They add to the bid list for the player
   noted by game.next_bid - this makes things easier for most games. */
void add_bid(player_t p, bid_t bid);
void add_sbid(player_t p, char val, char suit, char spec);

/* request_all_bids requests bids from all players whom have pending bid
   lists. */
void request_client_bids(void);

/* Called by network code when a player bid is received.  Calls
   handle_bid_event if bid is acceptable. */
void handle_client_bid(player_t p, int bid_choice);

/* Called by anyone to report a bid.  It should only be called by
   handle_client_bid, though. */
void handle_bid_event(player_t p, bid_t bid);
