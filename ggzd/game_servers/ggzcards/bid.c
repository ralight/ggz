/* 
 * File: bid.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/13/2001
 * Desc: Functions and data for bidding system
 * $Id: bid.c 3425 2002-02-20 03:45:35Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include <stdlib.h>
#include <ggz.h>

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

	ggzdmod_log(game.ggz,
		    "Requesting a bid from player %d/%s; %d choices", p,
		    get_player_name(p), bid_data->bid_count);

	if (bid_data->is_bidding)
		ggzdmod_log(game.ggz, "ERROR: req_bid: "
			    "requesting a bid from a player who's already bidding!");
	bid_data->is_bidding = 1;

	set_player_message(p);
	set_game_state(STATE_NONE);

	return send_bid_request(p, bid_data->bid_count,
					bid_data->bids);
}

/* Requests bids from all players that have a bid list.  This function is
   needed, rather than req_bid, because req_bid will not do actions in the
   correct order for multiple bids.  In fact, this function _could_ replace
   req_bid; the only drawback is that it's much slower. */
int request_all_bids(void)
{
	player_t p;
	int status = 0;

	ggzdmod_log(game.ggz, "Requesting bids from some/all players.");

	/* Mark all players as bidding. */
	for (p = 0; p < game.num_players; p++)
		if (game.players[p].bid_data.bid_count > 0) {
			if (game.players[p].bid_data.is_bidding)
				ggzdmod_log(game.ggz, "ERROR: req_bid: "
					    "requesting a bid from a player who's already bidding!");
			game.players[p].bid_data.is_bidding = 1;
			set_player_message(p);
		}

	set_game_state(STATE_NONE);

	/* Send all human-player bid requests */
	for (p = 0; p < game.num_players; p++)
		if (game.players[p].bid_data.bid_count > 0) {
			if (send_bid_request
			    (p, game.players[p].bid_data.bid_count,
			     game.players[p].bid_data.bids) < 0)
				status = -1;
		}

	/* OK, we're done.  We return now, and continue to wait for responses
	   from non-AI players. There's still a potential problem because as
	   soon as a player bids, that bid will generally become visible to
	   everyone.  It might be better to "hide" players' bids until
	   everyone has bid.  OTOH, this isn't how things work in real card
	   games, so it'll probably be okay if we just ignore this little
	   unfairness. */
	return status;
}

/* Receive a bid from an arbitrary player.  Test to make sure it's not
   out-of-turn.  Note that a return of -1 here indicates a GGZ error, which
   will disconnect the player. */
int rec_bid(player_t p, bid_t * bid)
{
	bid_data_t *bid_data = &game.players[p].bid_data;
	int fd = get_player_socket(p), index;

	/* Receive the bid index */
	if (ggz_read_int(fd, &index) < 0)
		return -1;

	/* Is this a valid bid? */
	if (!game.players[p].bid_data.is_bidding) {
		/* Most likely, the client sent a bid when it wasn't supposed
		   to.  Of course, a smart client won't do this. */
		ggzdmod_log(game.ggz,
			    "Received out-of-turn bid from player %d/%s.", p,
			    get_player_name(p));
	} else if (bid_data->bid_count == 0) {
		/* This is probably an error/bug in ggzcards-server. */
		ggzdmod_log(game.ggz,
			    "ERROR: rec_bid: we don't know what the choices are!");
		return -1;
	}

	/* Compute the bid from the index */
	index %= bid_data->bid_count;
	if (index < 0)
		index += bid_data->bid_count;
	*bid = bid_data->bids[index];

	/* Success! */
	ggzdmod_log(game.ggz, "Received bid choice %d from player %d/%s",
		    index, p, get_player_name(p));
	return 0;
}
