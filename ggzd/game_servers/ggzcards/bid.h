/* 
 * File: bid.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/13/2001
 * Desc: Functions and data for bidding system
 * $Id: bid.h 2418 2001-09-09 03:42:21Z jdorje $
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

/* clear_bids clears the list of possible bids. It's called automatically by
   handle_bid_event(). */
void clear_bids();

/* add_bid and add_sbid add another bid to the list of possible bids.
   add_sbid is just a convenience. */
void add_bid(bid_t bid);
void add_sbid(char val, char suit, char spec);

/* req_bid requests a bid from the client _or_ AI. */
int req_bid(player_t);

/* rec_bid receives a bid from the client */
int rec_bid(player_t, bid_t *);
