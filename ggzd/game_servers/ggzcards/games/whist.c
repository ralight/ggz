/*
 * File: games/whist.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 02/21/2002
 * Desc: Game-dependent game functions for Whist
 * $Id: whist.c 3483 2002-02-27 05:00:13Z jdorje $
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "game.h"
#include "message.h"

#include "whist.h"

static int whist_is_valid_game(void);
static void whist_init_game(void);
static void whist_set_player_message(player_t p);
static void whist_start_bidding(void);
static void whist_end_hand(void);

struct game_function_pointers whist_funcs = {
	whist_is_valid_game,
	whist_init_game,
	game_get_options,
	game_handle_option,
	game_get_option_text,
	whist_set_player_message,
	game_get_bid_text,
	whist_start_bidding,
	game_get_bid,
	game_handle_bid,
	game_next_bid,
	game_start_playing,
	game_verify_play,
	game_next_play,
	game_get_play,
	game_handle_play,
	game_deal_hand,
	game_end_trick,
	whist_end_hand,
	game_start_game,
	game_test_for_gameover,
	game_handle_gameover,
	game_map_card,
	game_compare_cards,
	game_send_hand
};

static int whist_is_valid_game(void)
{
	return game.num_players == 4;
}

static void whist_init_game(void)
{
	seat_t s;
	
	set_num_seats(4);
	for (s = 0; s < game.num_players; s++)
		assign_seat(s, s);
		
	game.target_score = 5;
}

static void whist_set_player_message(player_t p)
{
	seat_t s = game.players[p].seat;
	
	clear_player_message(s);
	add_player_rating_message(p);
	add_player_score_message(p);
	
	if (p == game.dealer)
		add_player_message(s, "dealer\n");
	
	/* Are these the right times? */
	/* (this part is identical to spades) */
	if (game.state == STATE_WAIT_FOR_PLAY
	    || game.state == STATE_NEXT_TRICK
	    || game.state == STATE_NEXT_PLAY) {
		add_player_message(s, "Tricks: %d (%d)\n",
				   game.players[p].tricks,
				   game.players[p].tricks +
				   game.players[(p + 2) % 4].tricks);
	}
	
	add_player_action_message(p);
}

static void whist_start_bidding(void)
{
	card_t trump_card = game.seats[game.dealer].hand.cards[random() % 13];
	
	/* TODO: send a cardlist message */
	game.trump = trump_card.suit;
	set_global_message("", "The dealer's up-card is the %s of %s.",
	                   face_names[(int) trump_card.face],
	                   suit_names[(int) trump_card.suit]);
	set_global_message("Trump", "Trump is %s.",
	                   suit_names[(int) game.trump]);
	set_game_state(STATE_FIRST_TRICK);
}

static void whist_end_hand(void)
{
	int team, tricks, points;
	
	for (team = 0; team < 2; team++) {
		tricks = game.players[team].tricks
		         + game.players[team + 2].tricks;
		if (tricks > 6)
			break;
	}

	assert(team < 2);
		
	tricks = game.players[team].tricks + game.players[team + 2].tricks;
		
	/* they won the hand */
	points = tricks - 6;
			
	set_global_message("", "%s/%s won the hand, earning %d points.",
	                   get_player_name(team),
	                   get_player_name(team + 2),
	                   points);
			
	game.players[team].score += points;
	game.players[team + 2].score += points;
	set_player_message(team);
	set_player_message(team + 2);
}
