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

#include "../games.h"
#include "../game.h"
#include "../ggz.h"
#include "../message.h"

#include "spades.h"

static void spades_init_game();
static int spades_get_options(int fd);
static void spades_handle_options(int* options);
static int spades_get_bid();
static int spades_deal_hand();
static int spades_get_bid_text(char* buf, int buf_len, bid_t bid);
static void spades_set_player_message(player_t p);
static void spades_end_trick();
static void spades_end_hand();

struct game_function_pointers spades_funcs = {
	spades_init_game,
	spades_get_options,
	spades_handle_options,
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
	game.name = "Spades";
}

static int spades_get_options(int fd)
{
	/* three options:
	 *   target score: 100, 250, 500, 1000
	 *   nil value: 50, 100
	 *   minimum team bid: 0, 1, 2, 3, 4
	 */
	game.num_options = 3;
	if (es_write_int(fd, WH_REQ_OPTIONS) < 0 ||
		es_write_int(fd, game.num_options) < 0 || /* number of options */
		es_write_int(fd, 2) < 0 || /* first option: 2 choices */
		es_write_int(fd, 1) < 0 || /* first option: default is 1 */
		es_write_string(fd, "Nil is worth 50") < 0 ||
		es_write_string(fd, "Nil is worth 100") < 0 ||
		es_write_int(fd, 4) < 0 || /* second option: 4 choices */
		es_write_int(fd, 2) < 0 || /* second option: default is 2 */
		es_write_string(fd, "Game to 100") < 0 ||
		es_write_string(fd, "Game to 250") < 0 ||
		es_write_string(fd, "Game to 500") < 0 ||
		es_write_string(fd, "Game to 1000") < 0 ||
		es_write_int(fd, 5) < 0 || /* third option: 4 choices */
		es_write_int(fd, 3) < 0 || /* third option: default is 3 */
		es_write_string(fd, "Minimum bid 0") < 0 ||
		es_write_string(fd, "Minimum bid 1") < 0 ||
		es_write_string(fd, "Minimum bid 2") < 0 ||
		es_write_string(fd, "Minimum bid 3") < 0 ||
		es_write_string(fd, "Minimum bid 4") < 0
	   )
		return -1;
	return 0;
}

static void spades_handle_options(int* options)
{
	switch (options[0]) {
		case 0: GSPADES.nil_value = 50;  break;
		case 1: GSPADES.nil_value = 100; break;
		default: break;
	}
	switch (options[1]) {
		case 0: game.target_score = 100; break;
		case 1: game.target_score = 250; break;
		case 2: game.target_score = 500; break;
		case 3: game.target_score = 1000; break;
		default: break;
	}
	if (options[2] >= 0) GSPADES.minimum_team_bid = options[2];
	set_global_message("Options",
		"Nil is worth %d points.\nThe first team to %d points wins.",
		GSPADES.nil_value, game.target_score);
	game.options_initted = 1;
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

	len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Score: %d (%d)\n", game.players[p].score, GSPADES.bags[p%2]);
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
	if (game.state == WH_STATE_NEXT_BID || game.state == WH_STATE_WAIT_FOR_BID) {
			char bid_text[game.max_bid_length];
			game.funcs->get_bid_text(bid_text, game.max_bid_length, game.players[p].bid);
			if (*bid_text) len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Bid: %s\n", bid_text);
	}
	if (game.state == WH_STATE_WAIT_FOR_BID && p == game.next_bid)
		len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Bidding..."); /* "Waiting for bid" won't fit */	
	if (game.state == WH_STATE_WAIT_FOR_PLAY && p == game.curr_play)
		len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Playing..."); /* "Waiting for play" won't fit */;

	send_player_message_toall(s);
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
	}
}
