/*
 * File: games/spades.c
 * Author: Jason Short
 * Project: GGZCards game module
 * Date: 07/02/2001
 * Desc: Game-dependent game functions for Spades
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */


#include <easysock.h>
#include <string.h>

#include "../games.h"
#include "../game.h"
#include "../ggz.h"
#include "../message.h"
#include "../options.h"

#include "spades.h"

static int spades_is_valid_game();
static void spades_init_game();
static void spades_get_options();
static int spades_handle_option(char* option, int value);
static int spades_get_bid();
static int spades_deal_hand();
static int spades_get_bid_text(char* buf, int buf_len, bid_t bid);
static void spades_set_player_message(player_t p);
static void spades_end_trick();
static void spades_end_hand();

struct game_function_pointers spades_funcs = {
	spades_is_valid_game,
	spades_init_game,
	spades_get_options,
	spades_handle_option,
	spades_set_player_message,
	spades_get_bid_text,
	game_start_bidding,
	spades_get_bid,
	game_handle_bid,
	game_next_bid,
	game_start_playing,
	game_verify_play,
	game_next_play,
	game_get_play,
	game_handle_play,
	spades_deal_hand,
	spades_end_trick,
	spades_end_hand,
	game_start_game,
	game_test_for_gameover,
	game_handle_gameover,
	game_map_card,
	game_compare_cards,
	game_send_hand
};

static int spades_is_valid_game()
{
	return (game.num_players == 4);
}

static void spades_init_game()
{
	player_t p;
	seat_t s;

	game.specific = alloc(sizeof(spades_game_t));
	set_num_seats(4);
	for(p = 0; p < game.num_players; p++) {
		s = p;
		game.players[p].seat = s;
		game.seats[s].ggz = &ggz_seats[p];
	}
	/* most possible bids for spades: 0-13 + Nil = 15
	 * longest possible bid: "Nil" = length 4 */
	game.max_bid_choices = 15;
	game.max_bid_length = 4;
	game.must_break_trump = 1;	/* in spades, you can't lead trump until it's broken */
	game.target_score = 500;	/* adjustable by options */	
	GSPADES.nil_value = 100;
	game.trump = SPADES;
	game.ai_type = GGZ_AI_SPADES;
}

static void spades_get_options()
{
	/* three options:
	 *   target score: 100, 250, 500, 1000
	 *   nil value: 50, 100
	 *   minimum team bid: 0, 1, 2, 3, 4
	 */
	add_option("nil_value", 2, 1, "Nil is worth 50", "Nil is worth 100");
	add_option("target_score", 4, 2, "Game to 100", "Game to 250", "Game to 500", "Game to 1000");
	add_option("minimum_bid", 5, 3, "Minimum bid 0", "Minimum bid 1",
					"Minimum bid 2", "Minimum bid 3",
					"Minimum bid 4");
	game_get_options();
}

static int spades_handle_option(char* option, int value)
{
	if (!strcmp("nil_value", option))
		GSPADES.nil_value = (value == 0) ? 50 : 100;
	else if (!strcmp("target_score", option))
		switch (value) {
			case 0: game.target_score = 100; break;
			case 1: game.target_score = 250; break;
			case 2: game.target_score = 500; break;
			case 3: game.target_score = 1000; break;
			default: break;
		}
	else if (!strcmp("minimum_bid", option))
		GSPADES.minimum_team_bid = value;
	else
		return game_handle_option(option, value);
	return 0;
}

static int spades_get_bid()
{
	int index=0, i;
	bid_t partners_bid = game.players[ (game.next_bid + 2) % 4].bid;
	bid_t bid;
	bid.bid = 0;
	for (i = 0; i <= game.hand_size; i++) {
		/* the second bidder on each team must make sure the minimum bid count is met */
		if (game.bid_count >= 2 &&
		    partners_bid.sbid.val + i < GSPADES.minimum_team_bid)
			continue;
		bid.sbid.val = i;
		game.bid_choices[index] = bid;
		index++;
	}
			
	/* "Nil" bid */
	bid.bid = 0;
	bid.sbid.spec = SPADES_NIL;
	game.bid_choices[index] = bid;
	index++;

	/* TODO: other specialty bids */

	return req_bid(game.next_bid, index, NULL);
}

static int spades_deal_hand()
{
	seat_t s;

	game.hand_size = 52 / game.num_players;
	/* in a regular deal, we just deal out hand_size cards to everyone */
	for (s = 0; s < game.num_seats; s++)
		cards_deal_hand(game.hand_size, &game.seats[s].hand);
	return 0;
}

static int spades_get_bid_text(char* buf, int buf_len, bid_t bid)
{
	if (bid.sbid.spec == SPADES_NIL) return snprintf(buf, buf_len, "Nil");
	return snprintf(buf, buf_len, "%d", (int)bid.sbid.val);
}

static void spades_set_player_message(player_t p)
{
	seat_t s = game.players[p].seat;
	char* message = game.seats[s].message;
	int len = 0;

	len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Score: %d\n", game.players[p].score);
	if (game.state != WH_STATE_NEXT_BID && game.state != WH_STATE_WAIT_FOR_BID) {
		/* we show both the individual and team contract */
		char bid_text[game.max_bid_length];
		int contract = game.players[p].bid.sbid.val + game.players[(p+2)%4].bid.sbid.val;
		game.funcs->get_bid_text(bid_text, game.max_bid_length, game.players[p].bid);
		if (*bid_text) len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Bid: %s (%d)\n", bid_text, contract);
	}
	if (game.state == WH_STATE_WAIT_FOR_PLAY || game.state == WH_STATE_NEXT_TRICK || game.state == WH_STATE_NEXT_PLAY) {
		len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Tricks: %d (%d)\n",
			game.players[p].tricks, game.players[p].tricks + game.players[(p+2)%4].tricks);
	}
	if ( (game.state == WH_STATE_NEXT_BID || game.state == WH_STATE_WAIT_FOR_BID) && game.players[p].bid_count > 0) {
			char bid_text[game.max_bid_length];
			game.funcs->get_bid_text(bid_text, game.max_bid_length, game.players[p].bid);
			if (*bid_text) len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Bid: %s\n", bid_text);
	}
	if (game.state == WH_STATE_WAIT_FOR_BID && p == game.next_bid)
		len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Bidding...");	
	if (game.state == WH_STATE_WAIT_FOR_PLAY && p == game.curr_play)
		len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Playing...");
}

static void spades_end_trick()
{
	game_end_trick();
	/* update teammate's info as well */
	game_set_player_message((game.winner+2)%4);
}

static void spades_end_hand()
{
	player_t p;

	for (p=0; p<2; p++) {
		int tricks, bid, score;
		bid = game.players[p].bid.sbid.val + game.players[p+2].bid.sbid.val;
		tricks = game.players[p].tricks + game.players[p+2].tricks;
		if (tricks >= bid) {
			int bags = tricks-bid;
			score = 10 * bid + bags;
			GSPADES.bags[p] += bags;
			if (GSPADES.bags[p] >= 10) {
				/* you lose 100 points for 10 overtricks */
				GSPADES.bags[p] -= 10;
				score -= 100;
			}
		} else
			score = -10 * bid;
		ggz_debug("Team %d bid %d, took %d, earned %d.", (int)p, bid, tricks, score);
		game.players[p].score += score;
		game.players[p+2].score += score;
	}
	for (p=0; p<4; p++) {
		if (game.players[p].bid.sbid.spec == SPADES_NIL) {
			int score = (game.players[p].tricks == 0 ? GSPADES.nil_value : -GSPADES.nil_value);
			ggz_debug("Player %d/%s earned %d for going nil.", (int)p, ggz_seats[p].name, score);
			game.players[p].score += score;
			game.players[(p+2)%4].score += score;
		}
		set_player_message(p);
	}
}
