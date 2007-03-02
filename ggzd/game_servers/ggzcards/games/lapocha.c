/* 
 * File: games/lapocha.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for La Pocha
 * $Id: lapocha.c 8996 2007-03-02 23:19:59Z jdorje $
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

#include <stdio.h>

#include <ggz.h>

#include "ggz_dio.h"
#include "net_common.h"

#include "bid.h"
#include "common.h"
#include "game.h"
#include "message.h"
#include "net.h"
#include "score.h"

#include "lapocha.h"

/* la pocha specific data */
#define LAPOCHA ( *(lapocha_game_t *)(game.specific) )
typedef struct lapocha_game_t {
	int bid_sum;		/* what the sum of the bids is */
} lapocha_game_t;

static bool lapocha_is_valid_game(void);
static void lapocha_init_game(void);
static void lapocha_handle_gameover(void);
static void lapocha_start_bidding(void);
static void lapocha_get_bid(void);
static void lapocha_handle_bid(player_t p, bid_t bid);
static void lapocha_next_bid(void);
static bool lapocha_test_for_gameover(void);
static void lapocha_deal_hand(void);
static int lapocha_get_bid_text(char *buf, size_t buf_len, bid_t bid);
static void lapocha_set_player_message(player_t p);
static void lapocha_end_hand(void);

/* these send lapocha-specific game messages. */
static void lap_send_trump_request(player_t p);
static void lap_send_bid_request(player_t p);
static void lap_send_dealer(void);
static void lap_send_trump(void);
static void lap_send_bid(player_t bidder, bid_t bid);
static void lap_send_scores(void);

game_data_t lapocha_data = {
	"lapocha",
	N_("La Pocha"),
	"http://www.ggzgamingzone.org/gameservers/ggzcards/",
	lapocha_is_valid_game,
	lapocha_init_game,
	game_get_options,
	game_handle_option,
	game_get_option_text,
	lapocha_set_player_message,
	game_sync_player,
	lapocha_get_bid_text,
	game_get_bid_desc,
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


static bool lapocha_is_valid_game(void)
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

static void lapocha_handle_gameover(void)
{
	player_t p;
	int hi_score = -9999;
	player_t winners[game.num_players];
	int winner_cnt = 0;

	/* in La Pocha, anyone who has 0 at the end automatically wins */
	for (p = 0; p < game.num_players; p++) {
		if (get_player_score(p) == 0) {
			winners[winner_cnt] = p;
			winner_cnt++;
		}
	}

	if (winner_cnt == 0) {
		/* in the default case, just take the highest score(s) this
		   should automatically handle the case of teams! */
		for (p = 0; p < game.num_players; p++) {
			int score = get_player_score(p);

			if (score > hi_score) {
				winner_cnt = 1;
				winners[0] = p;
				hi_score = score;
			} else if (score == hi_score) {
				winners[winner_cnt] = p;
				winner_cnt++;
			}
		}
	}

	handle_gameover_event(winner_cnt, winners);
}

static void set_trump(char suit)
{
	set_trump_suit(suit);
	set_global_message("", "Trump is %s.", get_suit_name(game.trump));
	lap_send_trump();
}

static void lapocha_start_bidding(void)
{
	game.bid_total = 5;
	/* all 4 players bid once, but the first bid determines the trump */
	if (game.hand_size != 10) {
		set_trump(deal_card(game.deck).suit);
		game.bid_count = 1;
		game.next_bid = (game.dealer + 1) % game.num_players;
	}
	LAPOCHA.bid_sum = 0;
	lap_send_dealer();
}

static void lapocha_get_bid(void)
{
	if (game.bid_count == 0) {
		/* determine the trump suit */
		char suit;
		for (suit = 0; suit < 4; suit++)
			add_sbid(game.dealer, 0, suit, LAPOCHA_TRUMP);
		(void)lap_send_trump_request(game.dealer);
	} else {	/* get a player's numerical bid */
		int i;
		assert(game.next_bid ==
		       (game.dealer + game.bid_count) % 4);
		for (i = 0; i <= game.hand_size; i++) {
			/* the dealer can't make the sum of the bids equal
			   the hand size; somebody's got to go down */
			if (game.next_bid == game.dealer &&
			    LAPOCHA.bid_sum + i == game.hand_size)
				continue;
			add_sbid(game.next_bid, i, NO_SUIT, LAPOCHA_BID);
		}
		(void)lap_send_bid_request(game.next_bid);
	}
	request_client_bids();
}

static void lapocha_handle_bid(player_t p, bid_t bid)
{
	assert(game.next_bid == p);
	if (bid.sbid.spec == LAPOCHA_TRUMP) {
		set_trump(bid.sbid.suit);
	} else {
		LAPOCHA.bid_sum += bid.sbid.val;
		lap_send_bid(p, bid);
	}
}

static void lapocha_next_bid(void)
{
	if (game.bid_count == 1)
		game.next_bid = (game.dealer + 1) % game.num_players;
	else
		game_next_bid();
}

static bool lapocha_test_for_gameover(void)
{
	return (game.hand_num >= 28);
}

static void lapocha_deal_hand(void)
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

	/* must be determined later */
	/* TODO: shouldn't go here */
	set_trump_suit(NO_SUIT);

	/* in a regular deal, we just deal out hand_size cards to everyone */
	for (s = 0; s < game.num_seats; s++)
		deal_hand(game.deck, game.hand_size, &game.seats[s].hand);
}

static int lapocha_get_bid_text(char *buf, size_t buf_len, bid_t bid)
{
	if (bid.sbid.spec == LAPOCHA_TRUMP)
		return snprintf(buf, buf_len, "%s",
				get_suit_name(bid.sbid.suit % 4));
	return snprintf(buf, buf_len, "%d", (int)bid.sbid.val);
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
				   (int)game.players[p].bid.bid);
	}
	add_player_tricks_message(p);
	add_player_bid_message(p);
	add_player_action_message(p);
}

static void lapocha_end_hand(void)
{
	player_t p;

	for (p = 0; p < game.num_players; p++) {
		ggz_debug(DBG_GAME,
			  "Player %d/%s got %d tricks on a bid of %d", p,
			  get_player_name(p), game.players[p].tricks,
			  (int)game.players[p].bid.bid);
		assert(p == game.players[p].team);
		if (game.players[p].tricks == game.players[p].bid.bid) {
			change_score(p, 10 + 5 * game.players[p].bid.bid);
		} else {
			change_score(p, -5 * game.players[p].bid.bid);
		}
	}
	lap_send_scores();
	set_global_message("", "No trump set.");	/* TODO: give
							   information about
							   previous hand */
}


/* What follows are game-specific extensions.  They can be ignored for most
   games. */

/* these send lapocha-specific game messages. */
static void lap_send_trump_request(player_t p)
{
	GGZDataIO *dio = get_player_dio(p);

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_GAME_SPECIFIC);
	ggz_dio_put_char(dio, LAP_REQ_TRUMP);
	ggz_dio_packet_end(dio);
}

static void lap_send_bid_request(player_t p)
{
	GGZDataIO *dio = get_player_dio(p);

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_GAME_SPECIFIC);
	ggz_dio_put_char(dio, LAP_REQ_BID);
	ggz_dio_packet_end(dio);
}

static void lap_send_dealer(void)
{
	int p;

	for (p = 0; p < game.num_players; p++) {
		GGZDataIO *dio = get_player_dio(p);

		ggz_dio_packet_start(dio);
		write_opcode(dio, MSG_GAME_SPECIFIC);
		ggz_dio_put_char(dio, LAP_MSG_DEALER);
		ggz_dio_put_int(dio, CONVERT_SEAT(game.dealer, p));
		ggz_dio_packet_end(dio);
	}
}

static void lap_send_trump(void)
{
	int p;

	for (p = 0; p < game.num_players; p++) {
		GGZDataIO *dio = get_player_dio(p);

		ggz_dio_packet_start(dio);
		write_opcode(dio, MSG_GAME_SPECIFIC);
		ggz_dio_put_char(dio, LAP_MSG_TRUMP);
		ggz_dio_put_char(dio, game.trump);
		ggz_dio_packet_end(dio);
	}
}

static void lap_send_bid(player_t bidder, bid_t bid)
{
	int p;
	int the_bid = bid.sbid.val;

	assert(game.players[bidder].seat == bidder);

	for (p = 0; p < game.num_players; p++) {
		GGZDataIO *dio = get_player_dio(p);

		ggz_dio_packet_start(dio);
		write_opcode(dio, MSG_GAME_SPECIFIC);
		ggz_dio_put_char(dio, LAP_MSG_BID);
		ggz_dio_put_int(dio, bidder);
		ggz_dio_put_int(dio, the_bid);
		ggz_dio_packet_end(dio);
	}
}

static void lap_send_scores(void)
{
	player_t p;

	for (p = 0; p < game.num_players; p++) {
		GGZDataIO *dio = get_player_dio(p);
		seat_t s_r;

		ggz_dio_packet_start(dio);
		write_opcode(dio, MSG_GAME_SPECIFIC);
		ggz_dio_put_char(dio, LAP_MSG_SCORES);
		for (s_r = 0; s_r < game.num_seats; s_r++) {
			seat_t s_abs = UNCONVERT_SEAT(s_r, p);

			assert(game.seats[s_abs].player == s_abs);
			assert(game.players[s_abs].team == s_abs);
			ggz_dio_put_int(dio, get_team_score(s_abs));
		}
		ggz_dio_packet_end(dio);
	}
}
