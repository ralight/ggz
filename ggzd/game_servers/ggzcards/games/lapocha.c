/* 
 * File: games/lapocha.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for La Pocha
 * $Id: lapocha.c 2832 2001-12-09 21:41:07Z jdorje $
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

#include "common.h"

#include "lapocha.h"

static int lapocha_is_valid_game(void);
static void lapocha_init_game(void);
static int lapocha_handle_gameover(void);
static void lapocha_start_bidding(void);
static int lapocha_get_bid(void);
static void lapocha_handle_bid(player_t p, bid_t bid);
static void lapocha_next_bid(void);
static int lapocha_test_for_gameover(void);
static int lapocha_deal_hand(void);
static int lapocha_get_bid_text(char *buf, size_t buf_len, bid_t bid);
static void lapocha_set_player_message(player_t p);
static void lapocha_end_hand(void);

struct game_function_pointers lapocha_funcs = {
	lapocha_is_valid_game,
	lapocha_init_game,
	game_get_options,
	game_handle_option,
	game_get_option_text,
	lapocha_set_player_message,
	lapocha_get_bid_text,
	lapocha_start_bidding,
	lapocha_get_bid,
	lapocha_handle_bid,
	lapocha_next_bid,
	game_start_playing,
	game_verify_play,
	game_next_play,
	game_get_play,
	game_handle_play,
	lapocha_deal_hand,
	game_end_trick,
	lapocha_end_hand,
	game_start_game,
	lapocha_test_for_gameover,
	lapocha_handle_gameover,
	game_map_card,
	game_compare_cards,
	game_send_hand
};


static int lapocha_is_valid_game(void)
{
	return (game.num_players == 4);
}

static void lapocha_init_game(void)
{
	seat_t s;

	game.specific = ggz_malloc(sizeof(lapocha_game_t));
	set_num_seats(4);
	for (s = 0; s < game.num_seats; s++)
		assign_seat(s, s);	/* one player per seat */

	game.deck_type = GGZ_DECK_LAPOCHA;
	game.max_hand_length = 10;
	game.must_overtrump = 1;	/* in La Pocha, you *must* overtrump
					   if you can */
}

static int lapocha_handle_gameover(void)
{
	player_t p;
	int hi_score = -9999;
	player_t winners[game.num_players];
	int winner_cnt = 0;

	/* in La Pocha, anyone who has 0 at the end automatically wins */
	for (p = 0; p < game.num_players; p++) {
		if (game.players[p].score == 0) {
			winners[winner_cnt] = p;
			winner_cnt++;
		}
	}

	if (winner_cnt == 0) {
		/* in the default case, just take the highest score(s) this
		   should automatically handle the case of teams! */
		for (p = 0; p < game.num_players; p++) {
			if (game.players[p].score > hi_score) {
				winner_cnt = 1;
				winners[0] = p;
				hi_score = game.players[p].score;
			} else if (game.players[p].score == hi_score) {
				winners[winner_cnt] = p;
				winner_cnt++;
			}
		}
	}

	return send_gameover(winner_cnt, winners);
}

static void lapocha_start_bidding(void)
{
	/* all 4 players bid once, but the first bid determines the trump */
	game.bid_total = 5;
	LAPOCHA.bid_sum = 0;
}

static int lapocha_get_bid(void)
{
	if (game.bid_count == 0) {	/* determine the trump suit */
		/* handled just like a bid */
		if (game.hand_size != 10) {
			bid_t bid;
			bid.bid = 0;
			bid.sbid.suit = cards_deal_card().suit;
			bid.sbid.spec = LAPOCHA_TRUMP;
			handle_bid_event(game.next_bid, bid);
		} else {
			char suit;
			for (suit = 0; suit < 4; suit++)
				add_sbid(0, suit, LAPOCHA_TRUMP);
			return req_bid(game.dealer);
		}
	} else {		/* get a player's numerical bid */
		int i;
		for (i = 0; i <= game.hand_size; i++) {
			/* the dealer can't make the sum of the bids equal
			   the hand size; somebody's got to go down */
			if (game.next_bid == game.dealer &&
			    LAPOCHA.bid_sum + i == game.hand_size)
				continue;
			add_sbid(i, 0, 0);
		}
		return req_bid(game.next_bid);
	}
	return 0;
}

static void lapocha_handle_bid(player_t p, bid_t bid)
{
	assert(game.next_bid == p);
	if (bid.sbid.spec == LAPOCHA_TRUMP) {
		game.trump = bid.sbid.suit;
		set_global_message("", "Trump is %s.",
				   suit_names[(int) game.trump % 4]);
	} else
		LAPOCHA.bid_sum += bid.sbid.val;
}

static void lapocha_next_bid(void)
{
	if (game.bid_count == 1)
		game.next_bid = (game.dealer + 1) % game.num_players;
	else
		game_next_bid();
}

static int lapocha_test_for_gameover(void)
{
	return (game.hand_num >= 28);
}

static int lapocha_deal_hand(void)
{
	seat_t s;

	/* The number of cards dealt each hand; it's easier just to write it
	   out than use any kind of function */
	int lap_card_count[] = { 1, 1, 1, 1,
		2, 3, 4, 5, 6, 7, 8, 9,
		10, 10, 10, 10,
		9, 8, 7, 6, 5, 4, 3, 2,
		1, 1, 1, 1
	};
	/* in la pocha, there are a predetermined number of cards each hand,
	   as defined by lap_card_count above */
	game.hand_size = lap_card_count[game.hand_num];
	game.trump = -1;	/* must be determined later *//* TODO: shouldn't go here */


	/* in a regular deal, we just deal out hand_size cards to everyone */
	for (s = 0; s < game.num_seats; s++)
		cards_deal_hand(game.hand_size, &game.seats[s].hand);

	return 0;
}

static int lapocha_get_bid_text(char *buf, size_t buf_len, bid_t bid)
{
	if (bid.sbid.spec == LAPOCHA_TRUMP)
		return snprintf(buf, buf_len, "%s",
				suit_names[(int) bid.sbid.suit % 4]);
	return snprintf(buf, buf_len, "%d", (int) bid.sbid.val);
}

static void lapocha_set_player_message(player_t p)
{
	seat_t s = game.players[p].seat;

	clear_player_message(s);
	add_player_rating_message(p);
	add_player_score_message(p);
	if (p == game.dealer)
		add_player_message(s, "dealer\n");
	if (game.state >= STATE_FIRST_TRICK
	    && game.state <= STATE_WAIT_FOR_PLAY) {
		add_player_message(s, "Contract: %d\n",
				   (int) game.players[p].bid.bid);
	}
	add_player_tricks_message(p);
	add_player_bid_message(p);
	add_player_action_message(p);
}

static void lapocha_end_hand(void)
{
	player_t p;

	for (p = 0; p < game.num_players; p++) {
		ggzdmod_log(game.ggz,
			    "Player %d/%s got %d tricks on a bid of %d", p,
			    ggzd_get_player_name(p), game.players[p].tricks,
			    (int) game.players[p].bid.bid);
		if (game.players[p].tricks == game.players[p].bid.bid)
			game.players[p].score +=
				10 + (5 * game.players[p].bid.bid);
		else
			game.players[p].score -= 5 * game.players[p].bid.bid;
	}
	set_global_message("", "No trump set.");	/* TODO: give
							   information about
							   previous hand */
}
