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
static void bridge_start_game();

static void bridge_set_score_message();

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
	bridge_start_game,
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
	int i, j;
	game.next_bid = game.dealer; /* dealer bids first */
	game.bid_total = -1; /* no set total */
	BRIDGE.pass_count = 0;
	BRIDGE.bonus = 1;
	for(i=0; i<2; i++)
		for(j=0; j<5; j++)
			BRIDGE.opener[i][j] = -1;
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
		BRIDGE.contract = bid.sbid.val;
		BRIDGE.contract_suit = bid.sbid.suit;
		BRIDGE.bonus = 1;
		BRIDGE.pass_count = 1;

		if (BRIDGE.opener[game.next_bid % 2][BRIDGE.contract_suit] == -1)
			BRIDGE.opener[game.next_bid % 2][BRIDGE.contract_suit] = game.next_bid;
		BRIDGE.declarer = BRIDGE.opener[game.next_bid % 2][BRIDGE.contract_suit];
		BRIDGE.dummy = (BRIDGE.declarer + 2) % 4;

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

/*
	len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Score: %d|%d\n", BRIDGE.points_above_line[p%2], BRIDGE.points_below_line[p%2]);
*/
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

static void bridge_set_score_message()
{
	player_t team;
	int widths[2], len=0, i, g;
	char buf[4096] = "";

#define HORIZONTAL_LINE for (i=0; i<widths[0]+widths[1]+3; i++) \
		len += snprintf(buf+len, sizeof(buf)-len, "%c", '-'); \
	len += snprintf(buf+len, sizeof(buf)-len, "\n")
#define BLANK_LINE len += snprintf(buf+len, sizeof(buf)-len, "%*s | %*s\n", widths[0], "", widths[1], "")

	len = snprintf(buf, sizeof(buf), "%s/%s | %s/%s\n",
		 ggz_seats[0].name, ggz_seats[2].name,
		 ggz_seats[1].name, ggz_seats[3].name);
	for(team=0; team<2; team++)
		widths[team] = strlen(ggz_seats[team].name) + strlen(ggz_seats[team+2].name) + 1;

	HORIZONTAL_LINE;
	BLANK_LINE;

	len += snprintf(buf+len, sizeof(buf)-len, "%*d | %-*d\n", widths[0], BRIDGE.points_above_line[0], widths[1], BRIDGE.points_above_line[1]);

	BLANK_LINE;

	for (g=0; g<=BRIDGE.game_count; g++) {
		HORIZONTAL_LINE;
		len += snprintf(buf+len, sizeof(buf)-len, "%*d | %-*d\n", widths[0], BRIDGE.points_below_line[g][0], widths[1], BRIDGE.points_below_line[g][1]);
	}

	set_global_message("Scores", "%s", buf);
}

static void bridge_end_hand()
{
	int points_above = 0, points_below = 0, tricks, g;
	int winning_team, team;
	int tricks_above, tricks_below;
	int vulnerable = BRIDGE.vulnerable[BRIDGE.declarer % 2];
	char buf[512];
	char buf2[512] = "";
	char* bonus_text = BRIDGE.bonus == 1 ? "" : BRIDGE.bonus == 2 ? ", doubled" : ", redoubled";

	/* calculate tricks over book */
	tricks = game.players[BRIDGE.declarer].tricks + game.players[BRIDGE.dummy].tricks - 6;

	ggz_debug("Contract was %d.  Declarer made %d.", BRIDGE.contract, tricks);

	winning_team = (tricks >= BRIDGE.contract) ? BRIDGE.declarer % 2 : (BRIDGE.declarer+1) % 2;

	snprintf(buf2, sizeof(buf2), "%s and %s get:\n",
		 ggz_seats[winning_team].name, ggz_seats[winning_team+2].name);

	if (tricks >= BRIDGE.contract) {
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

		points_below *= BRIDGE.bonus;
		points_above *= BRIDGE.bonus;

		snprintf(buf2+strlen(buf2), sizeof(buf2)-strlen(buf2), "  %d points below the line for %d %s%s.",
			 points_below, BRIDGE.contract, long_bridge_suit_names[BRIDGE.contract_suit], bonus_text);
		if (points_above)
			snprintf(buf2+strlen(buf2), sizeof(buf2)-strlen(buf2), " %d points above the line for %d overtricks%s.",
				 points_above, tricks_above, bonus_text);

		/* you get a bonus just for making a doubled/redoubled contract */
		if (BRIDGE.bonus > 1) {
			int insult_bonus = BRIDGE.bonus > 2 ? 100 : 50;
			snprintf(buf2+strlen(buf2), sizeof(buf2)-strlen(buf2),
				 "  %d points above the line for the insult.", insult_bonus);
			points_above += insult_bonus;
		}

		if (BRIDGE.contract >= 6) {
			int slam_bonus;
			if (BRIDGE.contract == 6)
				slam_bonus = vulnerable ? 1000 : 500;
			else
				slam_bonus = vulnerable ? 1500 : 750;
			snprintf(buf2+strlen(buf2), sizeof(buf2)-strlen(buf2),
				 "  %d points for a %s slam%s.",
				 slam_bonus, BRIDGE.contract == 7 ? "grand" : "small",
				 vulnerable ? " while vulnerable" : "");
			points_above += slam_bonus;
		}
	} else {
		tricks_above = BRIDGE.contract - tricks;

		/* Penalty:          not vulnerable        vulnerable
		 * not doubled            50                  100
		 * doubled - 1st          100                 200
		 * doubled - 2nd,3rd      200 ea              300 ea
		 * doubled - 4th+         300 ea              300 ea
		 * redoubled           2x doubled          2x doubled
		 */

		if (BRIDGE.bonus == 1)
			points_above = tricks_above * (vulnerable ? 100 : 50);
		else {
			points_above = tricks_above * (vulnerable ? 200 : 100);
			if (tricks_above > 1)
				points_above += (tricks_above-1) * 100;
			if (tricks_above > 3)
				points_above += (tricks_above-3) * (vulnerable ? 0 : 100);
			if (BRIDGE.bonus == 4)
				points_above *= 2;        	
		}
		snprintf(buf2+strlen(buf2), sizeof(buf2)-strlen(buf2),
			 "  %d points above for setting by %d tricks%s%s.",
			 points_above, tricks_above,
			 bonus_text,
			 vulnerable ? ", vulnerable" : "");

	}

	BRIDGE.points_above_line[winning_team] += points_above;
	BRIDGE.points_below_line[BRIDGE.game_count][winning_team] += points_below;

	if (tricks >= BRIDGE.contract)
		snprintf(buf, sizeof(buf), "%s made the bid and earned %d|%d points.", ggz_seats[BRIDGE.declarer].name, points_above, points_below);
	else
		snprintf(buf, sizeof(buf), "%s went set, giving up %d points.", ggz_seats[BRIDGE.declarer].name, points_above);

	/* TODO: points for honors */

	if (BRIDGE.points_below_line[BRIDGE.game_count][winning_team] >= 100) {
		if (BRIDGE.vulnerable[winning_team]) {
			/* they've won a rubber */
			int rubber_bonus = BRIDGE.vulnerable[1-winning_team] ? 500 : 700;
			BRIDGE.points_above_line[winning_team] += rubber_bonus;
			for (team=0; team<2; team++) {
				for (g=0; g<=BRIDGE.game_count; g++) {
					BRIDGE.points_above_line[team] += BRIDGE.points_below_line[g][team];
					BRIDGE.points_below_line[g][team] = 0;
				}
			}
			BRIDGE.game_count = 0;
			snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), "  They won a %d-point rubber.", rubber_bonus);
			/* right now, we jsut go right on into the next rubber with a running score.  Instead, this
			 * should be the end of a game */
		} else {
			/* they've won their first game of the rubber */
			BRIDGE.game_count++;
			BRIDGE.vulnerable[winning_team] = 1;
			snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf), "  They won a game.");
		}
	}
	/* TODO: vulnerable, etc. */

	set_global_message("", "%s", buf);
	set_global_message("Hand Score", buf2);
	bridge_set_score_message();

	BRIDGE.declarer = BRIDGE.dummy = -1;
	BRIDGE.dummy_revealed = 0;
	BRIDGE.contract = 0;
}

static void bridge_start_game()
{
	/* TODO: zero other scores */
	bridge_set_score_message();
	game_start_game();
}
