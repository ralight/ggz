/*
 * File: play.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 02/21/2002
 * Desc: Functions and data for playing system
 * $Id: play.c 3992 2002-04-15 09:36:11Z jdorje $
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

#include "common.h"
#include "message.h"
#include "net.h"
#include "play.h"

void req_play(player_t p, seat_t s)
{
	/* Sanity checks */
	player_t p2;
	assert(!game.players[p].is_playing);
        for (p2 = 0; p2 < game.num_players; p2++)
        	if (game.players[p2].is_playing)
        		assert(game.players[p2].play_seat != s);
	
	/* although the game_* functions probably track this data themselves,
	   we track it here as well just in case. */
	game.players[p].is_playing = TRUE;
	game.players[p].play_seat = s;

	set_game_state(STATE_WAIT_FOR_PLAY);
	set_player_message(p);
	
	(void) send_play_request(p, s);
}

void handle_client_play(player_t p, card_t card)
{
	hand_t *hand;
	int i;
	char *err;

	/* Is is this player's turn to play? */
	if (!game.players[p].is_playing) {
		/* better to just ignore it; a MSG_BADPLAY requests a new
		   play */
		ggzdmod_log(game.ggz,
			    "SERVER/CLIENT BUG: player %d/%s played "
			    "out of turn!?!?",
			    p, get_player_name(p));
		return;
	}

	/* find the card played */
	hand = &game.seats[game.players[p].play_seat].hand;
	for (i = 0; i < hand->hand_size; i++)
		if (are_cards_equal(hand->cards[i], card))
			break;

	if (i == hand->hand_size) {
		(void) send_badplay(p,
				    "That card isn't even in your hand."
				    "  This must be a bug.");
		send_sync(p);
		ggzdmod_log(game.ggz, "CLIENT BUG: "
			    "player %d/%s played a card that wasn't "
			    "in their hand.",
			    p, get_player_name(p));
		return;
	}

	ggzdmod_log(game.ggz, "We received a play of card "
		    "(%d %d %d) from player %d/%s.", card.face, card.suit,
		    card.deck, p, get_player_name(p));

	/* we've verified that this card could have physically been played;
	   we still need to check if it's a legal play Note, however, that we
	   don't return -1 on an error here.  An error returned indicates a
	   GGZ error, which is not what happened.  This is just a player
	   mistake */
	err = game.data->verify_play(p, card);
	if (err == NULL)
		/* any AI routine would also call handle_play_event, so the
		   ai must also check the validity as above.  This could be
		   changed... */
		handle_play_event(p, card);
	else
		(void) send_badplay(p, err);
}
