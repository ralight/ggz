/* 
 * File: bid.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/13/2001
 * Desc: Functions and data for bidding system
 * $Id: bid.c 8149 2006-06-09 19:09:42Z jdorje $
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
#include <string.h>

#include <ggz.h>

#include "bid.h"
#include "common.h"
#include "message.h"
#include "net.h"

bool is_anyone_bidding(void)
{
	players_iterate(p) {
		if (game.players[p].bid_data.is_bidding)
			return TRUE;
	} players_iterate_end;
	return FALSE;
}

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
void request_client_bid(player_t p)
{
	bid_data_t *bid_data = &game.players[p].bid_data;

	ggz_debug(DBG_BID,
		    "Requesting a bid from player %d/%s; %d choices", p,
		    get_player_name(p), bid_data->bid_count);

	assert(!bid_data->is_bidding);
	bid_data->is_bidding = TRUE;

	set_player_message(p);
	set_game_state(STATE_WAIT_FOR_BID);

	net_send_bid_request(p, bid_data->bid_count, bid_data->bids);
}

/* Requests bids from all players that have a bid list.  This function is
   needed, rather than req_bid, because req_bid will not do actions in the
   correct order for multiple bids.  In fact, this function _could_ replace
   req_bid; the only drawback is that it's much slower. */
/* FIXME: I don't think this is necessary anymore. */
void request_all_client_bids(void)
{
	ggz_debug(DBG_BID, "Requesting bids from some/all players.");

	/* Mark all players as bidding. */
	players_iterate(p) {
		if (game.players[p].bid_data.bid_count > 0) {
			assert(!game.players[p].bid_data.is_bidding);
			game.players[p].bid_data.is_bidding = TRUE;
			set_player_message(p);
		}
	} players_iterate_end;

	set_game_state(STATE_WAIT_FOR_BID);

	/* Send all human-player bid requests */
	players_iterate(p) {
		if (game.players[p].bid_data.bid_count > 0)
			net_send_bid_request(p,
			                     game.players[p].bid_data.bid_count,
			                     game.players[p].bid_data.bids);
	} players_iterate_end;

	/* There's still a potential problem because as
	   soon as a player bids, that bid will generally become visible to
	   everyone.  It might be better to "hide" players' bids until
	   everyone has bid.  OTOH, this isn't how things work in real card
	   games, so it'll probably be okay if we just ignore this little
	   unfairness. */
}

void handle_client_bid(player_t p, int bid_choice)
{
	bid_data_t *bid_data = &game.players[p].bid_data;
	bid_t bid;

	/* Only players and bots can bid. */
	if (!IS_REAL_PLAYER(p))
		return;

	/* Is this a valid bid? */
	if (!game.players[p].bid_data.is_bidding) {
		/* Most likely, the client sent a bid when it wasn't supposed
		   to.  Of course, a smart client won't do this. */
		ggz_debug(DBG_CLIENT,
			  "Received out-of-turn bid from player %d/%s.",
			  p, get_player_name(p));
		return;
	}
	
	assert(bid_data->bid_count > 0);

	/* Compute the bid from the index */
	bid_choice %= bid_data->bid_count;
	if (bid_choice < 0)
		bid_choice += bid_data->bid_count;
	bid = bid_data->bids[bid_choice];

	/* Success! */
	ggz_debug(DBG_BID, "Received bid choice %d from player %d/%s",
		    bid_choice, p, get_player_name(p));
	handle_bid_event(p, bid);
}

/* This handles the event of someone making a bid */
void handle_bid_event(player_t p, bid_t bid)
{
	net_broadcast_bid(p, bid);
	
	/* If we send a bid request to a player when the game is on,
	   and then a player leaves, the game is stopped.  But we
	   still need to handle the bid response from that player,
	   although we don't proceed with the game until the table
	   is full again. */
	
	assert(game.players[p].bid_data.is_bidding);
	game.players[p].bid_data.is_bidding = FALSE;
	clear_bids(p);

	ggz_debug(DBG_BID, "Handling a bid event for player %d.", p);

	assert(game.state == STATE_WAIT_FOR_BID);

	/* determine the bid */
	game.players[p].bid = bid;

	/* handle the bid.  Bid counts are updated before the handle_bid
	   function is called; this may be illogical but changing it would
	   break some things. */
	game.players[p].bid_count++;
	game.bid_count++;
	game.data->handle_bid(p, bid);

	set_player_message(p);

	/* add the bid to the "bid list" */
	/* FIXME: this needs work!!! */
	if (p <= game.prev_bid)
		game.bid_rounds++;
	if (game.bid_rounds >= game.max_bid_rounds) {
		game.max_bid_rounds += 10;
		players_iterate(p2) {
			game.players[p2].allbids =
				ggz_realloc(game.players[p2].allbids,
					    game.max_bid_rounds *
					    sizeof(bid_t));
			memset(&game.players[p2].
			       allbids[game.max_bid_rounds - 10], 0,
			       10 * sizeof(bid_t));
		} players_iterate_end;
	}
	game.players[p].allbids[game.bid_rounds] = bid;
	send_bid_history();
	
	/* Also mark the previous bidder, so that next_bid() can
	   safely access it. */
	game.prev_bid = p;
	
	if (is_anyone_bidding()) {
		assert(game.state == STATE_WAIT_FOR_BID);
		return;
	}
	
	set_game_state(STATE_NEXT_BID);
	
	/* Get the game code to handle the bid. */		
	game.data->next_bid();

	/* This is a minor hack.  The game's next_bid function might have
	   changed the game's state.  If that happened, we don't want to
	   change it back!  And it's necessary for this to come below
	   the next_bid call, since next_bid may change bid_total... */
	if (game.state == STATE_NEXT_BID
	    && game.bid_count == game.bid_total)
		set_game_state(STATE_FIRST_TRICK);

	/* do next move */
	next_move();
}
