/*
 * File: bid.c
 * Author: Jason Short
 * Project: GGZCards GGZ Server
 * Date: 07/13/2001
 * Desc: Functions and data for bidding system
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

#include <easysock.h>
#include <stdlib.h>

#include "common.h"


/* req_bid
 *   Request bid from current bidder
 *   parameters are: player to get bid from, number of possible bids, text entry for each bid
 */
int req_bid(player_t p, int num, char **bid_choices)
{
	int i, fd = ggz_seats[p].fd, status = 0;

	ggz_debug("Requesting a bid from player %d/%s; %d choices", p,
		  ggz_seats[p].name, num);

	/* although the game_* functions probably track this data
	   themselves, we track it here as well just in case. */
	game.num_bid_choices = num;
	game.next_bid = p;
	game.bid_text_ref = bid_choices;

	set_game_state(WH_STATE_WAIT_FOR_BID);
	set_player_message(p);


	if (ggz_seats[p].assign == GGZ_SEAT_BOT) {
		/* request a bid from the ai */
		handle_bid_event(ai_get_bid(p));
	} else {
		/* request a bid from the client */
		if (bid_choices == NULL) {
			bid_choices = game.bid_texts;
			for (i = 0; i < num; i++)
				game.funcs->get_bid_text(bid_choices[i],
							 game.max_bid_length,
							 game.bid_choices[i]);
		}

		if (fd == -1 ||
		    es_write_int(fd, WH_REQ_BID) < 0 ||
		    es_write_int(fd, num) < 0)
			status = -1;
		for (i = 0; i < num; i++) {
			if (es_write_string(fd, bid_choices[i]) < 0)
				status = -1;
		}
	}

	if (status != 0)
		ggz_debug("ERROR: req_bid: status is %d.", status);
	return status;
}

/* rec_bid
 *   Receive a bid from an arbitrary player.  Test to make sure it's not out-of-turn.
 *   Note that a return of -1 here indicates a GGZ error, which will disconnect the
 *   player.
 */
int rec_bid(player_t p, int *bid_index)
{
	int fd = ggz_seats[p].fd;

	if (es_read_int(fd, bid_index) < 0)
		return -1;

	*bid_index = *bid_index % game.num_bid_choices;
	if (*bid_index < 0)
		*bid_index += game.num_bid_choices;

	/* First of all, is this a valid bid? */
	if (p != game.next_bid) {
		ggz_debug
			("It's player %d/%s's turn to bid, not player %d/%s's.",
			 game.next_bid, ggz_seats[game.next_bid].name, p,
			 ggz_seats[p].name);
		return -1;
	} else if (!(game.state == WH_STATE_WAIT_FOR_BID ||
		     (game.state == WH_STATE_WAITFORPLAYERS
		      && game.saved_state == WH_STATE_WAIT_FOR_BID))) {
		ggz_debug("We're not currently waiting for a bid!");
		return -1;
	}

	ggz_debug("Received bid choice %d (%s) from player %d/%s",
		  *bid_index, game.bid_texts[*bid_index], p,
		  ggz_seats[p].name);
	return 0;
}
