/*
 * File: games/lapocha.c
 * Author: Jason Short
 * Project: GGZCards game module
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for La Pocha
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

#include "../games.h"
#include "../game.h"
#include "../ggz.h"
#include "../message.h"

#include "lapocha.h"

static void lapocha_init_game();
static int lapocha_handle_gameover();
static void lapocha_start_bidding();
static int lapocha_get_bid();
static void lapocha_handle_bid(bid_t bid);
static void lapocha_next_bid();
static int lapocha_test_for_gameover();
static int lapocha_deal_hand();
static int lapocha_get_bid_text(char* buf, int buf_len, bid_t bid);
static void lapocha_set_player_message(player_t p);
static void lapocha_end_hand();

struct game_function_pointers lapocha_funcs = {
	lapocha_init_game,
	game_get_options,
	game_handle_option,
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


static void lapocha_init_game()
{
	player_t p;
	seat_t s;

	game.specific = alloc(sizeof(lapocha_game_t));
	set_num_seats(4);
	for(p = 0; p < game.num_players; p++) {
		s = p;
		game.players[p].seat = s;
		game.seats[s].ggz = &ggz_seats[p];
	}
	game.deck_type = GGZ_DECK_LAPOCHA;
	game.max_bid_choices = 12;
	game.max_bid_length = 4;
	game.max_hand_length = 10;
	game.must_overtrump = 1;	/* in La Pocha, you *must* overtrump if you can */
	game.name = "La Pocha";
}

static int lapocha_handle_gameover()
{
	player_t p;
	int hi_score = -9999;
	player_t winners[game.num_players];
	int winner_cnt = 0;

	/* in La Pocha, anyone who has 0 at the end automatically wins */
	for (p=0; p<game.num_players; p++) {
		if(game.players[p].score == 0) {
			winners[winner_cnt] = p;
			winner_cnt++;
		}
	}

	if (winner_cnt == 0) {
		/* in the default case, just take the highest score(s)
		 * this should automatically handle the case of teams! */
		for (p=0; p<game.num_players; p++) {
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

static void lapocha_start_bidding()
{
	/* all 4 players bid once, but the first bid determines the trump */
	game.bid_total = 5;
	LAPOCHA.bid_sum = 0;
}

static int lapocha_get_bid()
{
	int status = 0;
	bid_t bid;
	bid.bid = 0;

	if (game.bid_count == 0) { /* determine the trump suit */
		/* handled just like a bid */
		if(game.hand_size != 10) {
			bid.bid = (long)cards_deal_card().suit;
			handle_bid_event(bid);
		} else
			status = req_bid(game.dealer, 4, suit_names);
	} else { /* get a player's numerical bid */
		int i, num=0;
		for (i = 0; i <= game.hand_size; i++) {
			/* the dealer can't make the sum of the bids equal the hand size;
			 * somebody's got to go down */
			if (game.next_bid == game.dealer &&
			    LAPOCHA.bid_sum + i == game.hand_size) {
				ggz_debug("Disallowing a bid of %d because that makes the bid sum of %d to equal the hand size of %d.",
					i, LAPOCHA.bid_sum, game.hand_size);
				continue;
			}
			game.bid_choices[num].bid = i;
			num++;
		}
		status = req_bid(game.next_bid, num, NULL);
	}
	return status;
}

static void lapocha_handle_bid(bid_t bid)
{
	if (game.bid_count == 0) {
		game.trump = bid.bid;
		set_global_message("", "Trump is %s.", suit_names[(int)game.trump % 4]);
	} else
		LAPOCHA.bid_sum += bid.bid;
}

static void lapocha_next_bid()
{
	if (game.bid_count == 1)
		game.next_bid = (game.dealer + 1) % game.num_players;
	else
		game_next_bid();
}

static int lapocha_test_for_gameover()
{
	return (game.hand_num == 29);
}

static int lapocha_deal_hand()
{
	seat_t s;

	/* The number of cards dealt each hand; it's
	 * easier just to write it out than use any kind of function */
	int lap_card_count[] = { 1, 1, 1, 1,
			2, 3, 4, 5, 6, 7, 8, 9,
			10, 10, 10, 10,
			9, 8, 7, 6, 5, 4, 3, 2,
			1, 1, 1, 1 };
	/* in la pocha, there are a predetermined number of cards
	 * each hand, as defined by lap_card_count above */
	game.hand_size = lap_card_count[game.hand_num];
	game.trump = -1; /* must be determined later */ /* TODO: shouldn't go here */


	/* in a regular deal, we just deal out hand_size cards to everyone */
	for (s = 0; s < game.num_seats; s++)
		cards_deal_hand(game.hand_size, &game.seats[s].hand);

	return 0;
}

static int lapocha_get_bid_text(char* buf, int buf_len, bid_t bid)
{
	return snprintf(buf, buf_len, "%d", (int)bid.bid);
}

static void lapocha_set_player_message(player_t p)
{
	seat_t s = game.players[p].seat;
	char* message = game.seats[s].message;
	int len = 0;

	len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Score: %d\n", game.players[p].score);
	if (p == game.dealer)
		len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "dealer\n");
	if (game.state >= WH_STATE_FIRST_TRICK && game.state <= WH_STATE_WAIT_FOR_PLAY) {
		len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Contract: %d\n", (int)game.players[p].bid.bid);
	}
	if (game.state == WH_STATE_WAIT_FOR_PLAY || game.state == WH_STATE_NEXT_TRICK || game.state == WH_STATE_NEXT_PLAY)
			len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Tricks: %d\n", game.players[p].tricks);
	if (game.state == WH_STATE_NEXT_BID || game.state == WH_STATE_WAIT_FOR_BID) {
		char bid_text[game.max_bid_length];
		game.funcs->get_bid_text(bid_text, game.max_bid_length, game.players[p].bid);
		if (*bid_text) len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Bid: %s\n", bid_text);
	}
	if (game.state == WH_STATE_WAIT_FOR_BID && p == game.next_bid)
		len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Bidding...");	
	if (game.state == WH_STATE_WAIT_FOR_PLAY && p == game.curr_play)
		len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Playing...");
}

static void lapocha_end_hand()
{
	player_t p;

	for(p=0; p<game.num_players; p++) {
		ggz_debug("Player %d/%s got %d tricks on a bid of %d", p, ggz_seats[p].name,
			  game.players[p].tricks, (int)game.players[p].bid.bid);
		if(game.players[p].tricks == game.players[p].bid.bid)
			game.players[p].score += 10 + (5 * game.players[p].bid.bid);
		else
			game.players[p].score -= 5 * game.players[p].bid.bid;
	}
}
