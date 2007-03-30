/*
 * File: games/whist.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 02/21/2002
 * Desc: Game-dependent game functions for Whist
 * $Id: whist.c 9021 2007-03-30 17:46:59Z jdorje $
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
#  include <config.h>	/* Site-specific config */
#endif

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "game.h"
#include "message.h"
#include "score.h"
#include "team.h"

#include "whist.h"

static bool whist_is_valid_game(void);
static void whist_init_game(void);
static void whist_set_player_message(player_t p);
static void whist_start_bidding(void);
static void whist_end_hand(void);

game_data_t whist_data = {
	"whist",
	N_("Whist"),
	"http://pagat.com/whist/whist.html",
	whist_is_valid_game,
	whist_init_game,
	game_get_options,
	game_handle_option,
	game_get_option_text,
	whist_set_player_message,
	game_sync_player,
	game_get_bid_text,
	game_get_bid_desc,
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

static bool whist_is_valid_game(void)
{
	return game.num_players == 4;
}

static void whist_init_game(void)
{
	seat_t s;

	set_num_seats(4);
	set_num_teams(2);
	for (s = 0; s < game.num_players; s++) {
		assign_seat(s, s);
		assign_team(s % 2, s);
	}

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

	add_player_tricks_message(p);
	add_player_action_message(p);
}

static void whist_start_bidding(void)
{
	card_t trump_card = game.seats[game.dealer].hand.cards[myrand(13)];

	/* TODO: send a cardlist message */
	set_trump_suit(trump_card.suit);
	set_global_message("", "The dealer's up-card is the %s of %s.",
			   get_face_name(trump_card.face),
			   get_suit_name(trump_card.suit));
	set_global_message("Trump", "Trump is %s.",
			   get_suit_name(game.trump));
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
			   get_player_name(team + 2), points);

	change_score(team, points);
	set_player_message(team);
	set_player_message(team + 2);
}
