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

#include "../common.h"

#include "spades.h"

static int spades_is_valid_game();
static void spades_init_game();
static void spades_get_options();
static int spades_handle_option(char* option, int value);
static char* spades_get_option_text(char* buf, int bufsz, char* option, int value);
static int spades_get_bid();
static int spades_get_bid_text(char* buf, int buf_len, bid_t bid);
static void spades_set_player_message(player_t p);
static void spades_end_trick();
static void spades_end_hand();

struct game_function_pointers spades_funcs = {
	spades_is_valid_game,
	spades_init_game,
	spades_get_options,
	spades_handle_option,
	spades_get_option_text,
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
	game_deal_hand,
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

static char* spades_get_option_text(char* buf, int bufsz, char* option, int value)
{
	if (!strcmp(option, "nil_value"))
		snprintf(buf, bufsz, "Nil is worth %d points.", GSPADES.nil_value);
	else if (!strcmp(option, "target_score"))
		snprintf(buf, bufsz, "The game is being played to %d.", game.target_score);
	else if (!strcmp(option, "minimum_bid"))
		snprintf(buf, bufsz, "The minimum team bid is %d.", GSPADES.minimum_team_bid);
	else
		return game_get_option_text(buf, bufsz, option, value);
	return buf;
}

static int spades_get_bid()
{
	int i;
	/* partner's bid (value) */
	char pard = (game.bid_count >= 2) ? game.players[ (game.next_bid + 2) % 4].bid.sbid.val : 0;

	for (i = 0; i <= game.hand_size - pard; i++) {
		/* the second bidder on each team must make sure the minimum bid count is met */
		if (game.bid_count >= 2 &&
		    pard + i < GSPADES.minimum_team_bid)
			continue;
		add_sbid(i, 0, 0);
	}
			
	/* "Nil" bid */
	/* If you're the first bidder, you can bid nil - your partner will be forced
	 * up to the minimum bid.  If you're the second bidder, you can't bid nil
	 * unless your partner has already met the minimum. */
	if (game.bid_count < 2 || pard >= GSPADES.minimum_team_bid)
		add_sbid(0, 0, SPADES_NIL);

	/* TODO: other specialty bids */

	return req_bid(game.next_bid);
}

static int spades_get_bid_text(char* buf, int buf_len, bid_t bid)
{
	if (bid.sbid.spec == SPADES_NIL) return snprintf(buf, buf_len, "Nil");
	return snprintf(buf, buf_len, "%d", (int)bid.sbid.val);
}

static void spades_set_player_message(player_t p)
{
	seat_t s = game.players[p].seat;

	add_player_score_message(p);
	if (game.state != WH_STATE_NEXT_BID && game.state != WH_STATE_WAIT_FOR_BID) {
		/* we show both the individual and team contract */
		char bid_text[512];
		int contract = game.players[p].bid.sbid.val + game.players[(p+2)%4].bid.sbid.val;
		game.funcs->get_bid_text(bid_text, sizeof(bid_text), game.players[p].bid);
		if (*bid_text)
			add_player_message(s, "Contract: %s (%d)\n", bid_text, contract);
	}
	if (game.state == WH_STATE_WAIT_FOR_PLAY || game.state == WH_STATE_NEXT_TRICK || game.state == WH_STATE_NEXT_PLAY) {
		add_player_message(s, "Tricks: %d (%d)\n",
			game.players[p].tricks, game.players[p].tricks + game.players[(p+2)%4].tricks);
	}
	add_player_bid_message(p);
	add_player_action_message(p);
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
