/*
 * File: games/bridge.c
 * Author: Jason Short
 * Project: GGZCards game module
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for Bridge
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

#include <string.h>

#include "../games.h"
#include "../game.h"
#include "../ggz.h"
#include "../message.h"

#include "bridge.h"

static int bridge_is_valid_game();
static int bridge_compare_cards(card_t, card_t);
static void bridge_init_game();
static void bridge_start_bidding();
static int bridge_get_bid();
static void bridge_handle_bid(bid_t bid);
static void bridge_next_bid();
static void bridge_start_playing();
static void bridge_get_play(player_t p);
static void bridge_handle_play(card_t card);
static int bridge_test_for_gameover();
static int bridge_send_hand(player_t p, seat_t s);
static int bridge_get_bid_text(char* buf, int buf_len, bid_t bid);
static void bridge_set_player_message(player_t p);
static void bridge_end_trick();
static void bridge_end_hand();

struct game_function_pointers bridge_funcs = {
	bridge_is_valid_game,
	bridge_init_game,
	game_get_options,
	game_handle_option,
	game_get_option_text,
	bridge_set_player_message,
	bridge_get_bid_text,
	bridge_start_bidding,
	bridge_get_bid,
	bridge_handle_bid,
	bridge_next_bid,
	bridge_start_playing,
	game_verify_play,
	game_next_play,
	bridge_get_play,
	bridge_handle_play,
	game_deal_hand,
	bridge_end_trick,
	bridge_end_hand,
	game_start_game,
	bridge_test_for_gameover,
	game_handle_gameover,
	game_map_card,
	bridge_compare_cards,
	bridge_send_hand
};


/* these should be low, clubs, diamonds, ..., high, but that won't fit in the client window */
static char* short_bridge_suit_names[5] = {"C", "D", "H", "S", "NT"};
static char* long_bridge_suit_names[5] = {"clubs", "diamonds", "hearts", "spades", "notrump"};


static int bridge_is_valid_game()
{
	return (game.num_players == 4);
}


static int bridge_compare_cards(card_t card1, card_t card2)
{
	/* in Bridge, the trump suit is always supposed to be shown on the left */
	if (card1.suit == game.trump && card2.suit != game.trump)
		return -1;
	if (card2.suit == game.trump && card1.suit != game.trump)
		return 1;

	return game_compare_cards(card1, card2);
}

static void bridge_init_game()
{
	player_t p;
	seat_t s;

	game.specific = alloc(sizeof(bridge_game_t));
	set_num_seats(4);
	for(p = 0; p < game.num_players; p++) {
		s = p;
		game.players[p].seat = s;
		game.seats[s].ggz = &ggz_seats[p];
	}
	/* most possible bids for bridge: 7 * 5 suit bids + (re)double + pass = 37
	 * longest possible bid: "redouble" = 9 */
	game.max_bid_choices = 37;
	game.max_bid_length = 9;

	game.cumulative_scores = 0;

	/* TODO: for now we won't use bridge scoring */
	BRIDGE.declarer = -1;
}

static void bridge_start_bidding()
{
	game.next_bid = game.dealer; /* dealer bids first */
	game.bid_total = -1; /* no set total */
	BRIDGE.pass_count = 0;
	BRIDGE.bonus = 1;
}

static int bridge_get_bid()
{
	int index=0;
	bid_t bid;
	bid.bid = 0;

	/* this is based closely on the Suaro code, below */

	/* make a list of regular bids */
	for (bid.sbid.val = 1; bid.sbid.val <= 7; bid.sbid.val++) {
		for (bid.sbid.suit = CLUBS; bid.sbid.suit <= BRIDGE_NOTRUMP; bid.sbid.suit++) {
			if (bid.sbid.val < BRIDGE.contract) continue;
			if (bid.sbid.val == BRIDGE.contract && bid.sbid.suit <= BRIDGE.contract_suit) continue;
			game.bid_choices[index] = bid;
			index++;
		}
	}

	/* make "double" or "redouble" bid */
	if (BRIDGE.contract != 0 &&
	    BRIDGE.bonus == 1 &&
	    (game.next_bid == (BRIDGE.declarer+1) % 4 || game.next_bid == (BRIDGE.declarer+3) % 4)) {
		bid.bid = 0;
		bid.sbid.spec = BRIDGE_DOUBLE;
		game.bid_choices[index] = bid;
		index++;
	} else if (BRIDGE.contract != 0 &&
		   BRIDGE.bonus == 2 &&
		   (game.next_bid == BRIDGE.declarer || game.next_bid == (BRIDGE.declarer+2) % 4)) {
		bid.bid = 0;
		bid.sbid.spec = BRIDGE_REDOUBLE;
		game.bid_choices[index] = bid;
		index++;
	}

	/* make "pass" bid */
	bid.bid = 0;
	bid.sbid.spec = BRIDGE_PASS;
	game.bid_choices[index] = bid;
	index++;

	return req_bid(game.next_bid, index, NULL);
}

static void bridge_handle_bid(bid_t bid)
{
	/* closely based on the Suaro code*/
	ggz_debug("The bid chosen is %d %s %d.", bid.sbid.val, short_bridge_suit_names[(int)bid.sbid.suit], bid.sbid.spec);
	
	if (bid.sbid.spec == BRIDGE_PASS) {
		BRIDGE.pass_count++;
	} else if (bid.sbid.spec == BRIDGE_DOUBLE || bid.sbid.spec == BRIDGE_REDOUBLE) {
		BRIDGE.pass_count = 1;
		BRIDGE.bonus *= 2;
	} else {
		/* TODO: declarer is the first person to name the suit, not the last bidder */
		BRIDGE.declarer = game.next_bid;
		BRIDGE.dummy = (BRIDGE.declarer + 2) % 4;
		BRIDGE.bonus = 1;
		BRIDGE.pass_count = 1;
		BRIDGE.contract = bid.sbid.val;
		BRIDGE.contract_suit = bid.sbid.suit;
		ggz_debug("Setting bridge contract to %d %s.", BRIDGE.contract, long_bridge_suit_names[BRIDGE.contract_suit]);
		if (bid.sbid.suit != BRIDGE_NOTRUMP)
			game.trump = bid.sbid.suit;
		else
			game.trump = -1;
	}
}

static void bridge_next_bid()
{
	/* closely based on Suaro code, below */
	if (BRIDGE.pass_count == 4) {
		/* done bidding */
		if (BRIDGE.contract == 0) {
			ggz_debug("Four passes; redealing hand.");
			set_global_message("", "%s", "Everyone passed; redealing.");
			set_game_state( WH_STATE_NEXT_HAND ); /* redeal hand */
		} else {
			ggz_debug("Three passes; bidding is over.");
			game.bid_total = game.bid_count;
			/* contract was determined in game_handle_bid */
		}
	} else {
		if (game.bid_count == 0)
			game.next_bid = game.dealer;
		else
			game.next_bid = (game.next_bid + 1) % game.num_players;
	}
}

static void bridge_start_playing()
{
	game.trick_total = game.hand_size;
	game.play_total = game.num_players;

	/* declarer is set in game_handle_bid */
	set_global_message("", "Contract: %d %s%s.",
		BRIDGE.contract, long_bridge_suit_names[(int)BRIDGE.contract_suit],
		BRIDGE.bonus == 1 ? "" : BRIDGE.bonus == 2 ? ", doubled" : ", redoubled");
	game.leader = (BRIDGE.declarer + 1) % game.num_players;
}

static void bridge_get_play(player_t p)
{
	if (p == BRIDGE.dummy)
		/* the declarer plays the dummy's hand */
		req_play(BRIDGE.declarer, game.players[p].seat);
	else
		game_get_play(p);
}

static void bridge_handle_play(card_t card)
{
	if (game.play_count == 0 && game.trick_count == 0) {
		/* after the first play of the hand, we reveal
		 * the dummy's hand to everyone */
		player_t p;
		seat_t dummy_seat = game.players[BRIDGE.dummy].seat;
		cards_sort_hand(&game.seats[dummy_seat].hand);
		BRIDGE.dummy_revealed = 1;
		for (p=0; p<game.num_players; p++) {
			/* if (p == BRIDGE.dummy) continue; */
			game.funcs->send_hand(p, dummy_seat);
		}
	}
}

static int bridge_test_for_gameover()
{
	/* TODO: implement bridge scoring */
	return 0;
}

static int bridge_send_hand(player_t p, seat_t s)
{
	/* we explicitly send out the dummy hand, but a player who
	 * joins late won't see it.  We have the same problem with Suaro. */
	if (s == BRIDGE.dummy /* player/seat crossover; ok because it's bridge */
	    && BRIDGE.dummy_revealed)
		return send_hand(p, s, 1);

	return game_send_hand(p, s);
}

static int bridge_get_bid_text(char* buf, int buf_len, bid_t bid)
{

	if (bid.sbid.spec == BRIDGE_PASS) return snprintf(buf, buf_len, "Pass");
	if (bid.sbid.spec == BRIDGE_DOUBLE) return snprintf(buf, buf_len, "Double");
	if (bid.sbid.spec == BRIDGE_REDOUBLE) return snprintf(buf, buf_len, "Redouble");
	if (bid.sbid.val > 0) return snprintf(buf, buf_len, "%d %s", bid.sbid.val, short_bridge_suit_names[(int)bid.sbid.suit]);
	return snprintf(buf, buf_len, "%s", "");
}

static void bridge_set_player_message(player_t p)
{
	seat_t s = game.players[p].seat;
	char* message = game.seats[s].message;
	int len = 0;

	len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Score: %d|%d\n", BRIDGE.points_above_line[p%2], BRIDGE.points_below_line[p%2]);
	if (game.state != WH_STATE_NEXT_BID && game.state != WH_STATE_WAIT_FOR_BID) {
		if (p == BRIDGE.declarer)
			len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "declarer\n");
		if (p == BRIDGE.dummy)
			len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "dummy\n");
	}
	if (game.state == WH_STATE_WAIT_FOR_PLAY || game.state == WH_STATE_NEXT_TRICK || game.state == WH_STATE_NEXT_PLAY)
			len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Tricks: %d\n", game.players[p].tricks+game.players[(p+2)%4].tricks);
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

static void bridge_end_trick()
{
	game_end_trick();

	/* update teammate's info as well */
	set_player_message((game.winner+2)%4);
	
}

static void bridge_end_hand()
{
	int points_above, points_below, tricks;
	int winning_team, team;
	int tricks_above, tricks_below;
	char buf[512];

	/* calculate tricks over book */
	tricks = game.players[BRIDGE.declarer].tricks + game.players[BRIDGE.dummy].tricks - 6;

	ggz_debug("Contract was %d.  Declarer made %d.", BRIDGE.contract, tricks);

	if (tricks >= BRIDGE.contract) {
		winning_team = BRIDGE.declarer % 2;
		tricks_below = BRIDGE.contract;
		tricks_above = tricks - BRIDGE.contract;
		switch (BRIDGE.contract_suit) {
			case CLUBS:
			case DIAMONDS:
				points_below = 20 * tricks_below;
				points_above = 20 * tricks_above;
				break;
			case HEARTS:
			case SPADES:
				points_below = 30 * tricks_below;
				points_above = 30 * tricks_above;
				break;
			case BRIDGE_NOTRUMP:
			default:
				points_below = 30 * tricks_below + (tricks_below > 0) ? 10 : 0;
				points_above = 30 * tricks_above;
				break;
		}		
	} else {
		winning_team = (BRIDGE.declarer + 1) % 2;
		tricks_below = points_below = 0;
		tricks_above = BRIDGE.contract - tricks;
		points_above = 50 * tricks_above;
	}

	points_below *= BRIDGE.bonus;
	points_above *= BRIDGE.bonus;
	BRIDGE.points_above_line[winning_team] += points_above;
	BRIDGE.points_below_line[winning_team] += points_below;

	if (tricks >= BRIDGE.contract)
		snprintf(buf, sizeof(buf), "%s made the bid and earned %d|%d points.", ggz_seats[BRIDGE.declarer].name, points_above, points_below);
	else
		snprintf(buf, sizeof(buf), "%s went set, giving up %d points.", ggz_seats[BRIDGE.declarer].name, points_above);

	if (BRIDGE.points_below_line[winning_team] >= 100) {
		/* 500 point bonus for winning a game */
		BRIDGE.points_above_line[winning_team] += 500;
		for (team=0; team<2; team++) {
			BRIDGE.points_above_line[team] += BRIDGE.points_below_line[team];
			BRIDGE.points_below_line[team] = 0;
		}
		snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), "  They won a game.");
	}
	/* TODO: vulnerable, etc. */

	set_global_message("", "%s", buf);

	BRIDGE.declarer = BRIDGE.dummy = -1;
	BRIDGE.dummy_revealed = 0;
	BRIDGE.contract = 0;
}
