/* 
 * File: bid.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/13/2001
 * Desc: Functions and data for bidding system
 * $Id: bid.c 2730 2001-11-13 06:29:00Z jdorje $
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

#include <stdlib.h>

#include <easysock.h>

#include "common.h"

/* clear_bids clears the list of bids */
void clear_bids(player_t p)
{
	game.players[p].bid_data.bid_count = 0;
}

#define MAX(a, b) ( (a) > (b) ? (a) : (b) )

/* add_bid adds the bid to the list of bid choices we're going to give the
   client */
void add_bid(bid_t bid)
{
	bid_data_t *bid_data = &game.players[game.next_bid].bid_data;

	bid_data->bid_count++;
	if (bid_data->bid_count > bid_data->bid_size) {
		bid_data->bid_size = MAX(2 * bid_data->bid_size, 8);
		bid_data->bids =
			ggz_realloc(bid_data->bids,
				    bid_data->bid_size * sizeof(bid_t));
	}
	bid_data->bids[bid_data->bid_count - 1] = bid;
}

void add_sbid(char val, char suit, char spec)
{
	bid_t bid;
	bid.bid = 0;
	bid.sbid.val = val;
	bid.sbid.suit = suit;
	bid.sbid.spec = spec;
	add_bid(bid);
}

/* Request bid from current bidder parameters are: player to get bid from,
   number of possible bids, text entry for each bid */
int req_bid(player_t p)
{
	bid_data_t *bid_data = &game.players[p].bid_data;

	ggzd_debug("Requesting a bid from player %d/%s; %d choices", p,
		   ggzd_get_player_name(p), bid_data->bid_count);

	set_game_state(WH_STATE_WAIT_FOR_BID);
	if (bid_data->is_bidding)
		ggzd_debug
			("ERROR: req_bid: requesting a bid from a player who's already bidding!");
	bid_data->is_bidding = 1;

	set_player_message(p);

	if (ggzd_get_seat_status(p) == GGZ_SEAT_BOT) {
		/* request a bid from the ai */
		handle_bid_event(p,
				 ai_get_bid(p, bid_data->bids,
					    bid_data->bid_count));
		return 0;
	} else
		return send_bid_request(p, bid_data->bid_count,
					bid_data->bids);
}

/* Receive a bid from an arbitrary player.  Test to make sure it's not
   out-of-turn.  Note that a return of -1 here indicates a GGZ error, which
   will disconnect the player. */
int rec_bid(player_t p, bid_t * bid)
{
	bid_data_t *bid_data = &game.players[p].bid_data;
	int fd = ggzd_get_player_socket(p), index;

	/* Receive the bid index */
	if (es_read_int(fd, &index) < 0)
		return -1;

	/* Is this a valid bid? */
	if (!game.players[p].bid_data.is_bidding) {
		/* Most likely, the client sent a bid when it wasn't supposed 
		   to.  Of course, a smart client won't do this. */
		ggzd_debug("Received out-of-turn bid from player %d/%s.",
			   p, ggzd_get_player_name(p));
	} else if (bid_data->bid_count == 0) {
		/* This is probably an error/bug in ggzcards-server. */
		ggzd_debug
			("ERROR: rec_bid: we don't know what the choices are!");
		return -1;
	}

	/* Compute the bid from the index */
	index %= bid_data->bid_count;
	if (index < 0)
		index += bid_data->bid_count;
	*bid = bid_data->bids[index];

	/* Success! */
	ggzd_debug("Received bid choice %d from player %d/%s",
		   index, p, ggzd_get_player_name(p));
	return 0;
}
