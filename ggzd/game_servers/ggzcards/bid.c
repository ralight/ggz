/* 
 * File: bid.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/13/2001
 * Desc: Functions and data for bidding system
 * $Id: bid.c 2418 2001-09-09 03:42:21Z jdorje $
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

/* TODO: declaring these static is OK for now, but eventually they'll have to 
   be a part of the game struct */
static bid_t *bids = NULL;
static int bid_size = 0;
static int bid_count = 0;

/* clear_bids clears the list of bids */
void clear_bids()
{
	bid_count = 0;
}

/* add_bid adds the bid to the list of bid choices we're going to give the
   client */
void add_bid(bid_t bid)
{
	bid_count++;
	if (bid_count > bid_size) {
		bid_size = bid_size ? 2 * bid_size : 8;
		bids = realloc(bids, bid_size * sizeof(bid_t));
	}
	bids[bid_count - 1] = bid;
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

/* req_bid Request bid from current bidder parameters are: player to get bid
   from, number of possible bids, text entry for each bid */
int req_bid(player_t p)
{
	int i, fd = ggzd_get_player_socket(p), status = 0;

	ggzd_debug("Requesting a bid from player %d/%s; %d choices", p,
		   ggzd_get_player_name(p), bid_count);

	/* although the game_* functions probably track this data themselves, 
	   we track it here as well just in case. */
	game.next_bid = p;

	set_game_state(WH_STATE_WAIT_FOR_BID);
	set_player_message(p);

	if (ggzd_get_seat_status(p) == GGZ_SEAT_BOT) {
		/* request a bid from the ai */
		handle_bid_event(ai_get_bid(p, bids, bid_count));
	} else {
		/* request a bid from the client */
		if (fd == -1 ||
		    write_opcode(fd, WH_REQ_BID) < 0 ||
		    es_write_int(fd, bid_count) < 0)
			status = -1;
		for (i = 0; i < bid_count; i++) {
			char bid_text[4096];
			game.funcs->get_bid_text(bid_text, sizeof(bid_text),
						 bids[i]);
			if (es_write_string(fd, bid_text) < 0)
				status = -1;
		}
	}

	if (status != 0)
		ggzd_debug("ERROR: req_bid: status is %d.", status);
	return status;
}

/* rec_bid Receive a bid from an arbitrary player.  Test to make sure it's
   not out-of-turn.  Note that a return of -1 here indicates a GGZ error,
   which will disconnect the player. */
int rec_bid(player_t p, bid_t * bid)
{
	int fd = ggzd_get_player_socket(p), index;

	if (es_read_int(fd, &index) < 0)
		return -1;

	index = index % bid_count;
	if (index < 0)
		index += bid_count;

	/* First of all, is this a valid bid? */
	if (p != game.next_bid) {
		ggzd_debug
			("It's player %d/%s's turn to bid, not player %d/%s's.",
			 game.next_bid, ggzd_get_player_name(game.next_bid),
			 p, ggzd_get_player_name(p));
		return -1;
	} else if (!(game.state == WH_STATE_WAIT_FOR_BID ||
		     (game.state == WH_STATE_WAITFORPLAYERS
		      && game.saved_state == WH_STATE_WAIT_FOR_BID))) {
		ggzd_debug("We're not currently waiting for a bid!");
		return -1;
	}

	*bid = bids[index];

	ggzd_debug("Received bid choice %d from player %d/%s",
		   index, p, ggzd_get_player_name(p));
	return 0;
}
