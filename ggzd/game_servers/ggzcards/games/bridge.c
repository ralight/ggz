/* 
 * File: games/bridge.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for Bridge
 * $Id: bridge.c 8240 2006-06-21 15:35:15Z jdorje $
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

#include <stdio.h>
#include <string.h>

#include "bid.h"
#include "common.h"
#include "game.h"
#include "message.h"
#include "net.h"
#include "play.h"
#include "team.h"

#include "bridge.h"

#define BRIDGE ( *(bridge_game_t *)(game.specific) )
typedef struct bridge_game_t {
	int pass_count;		/* number of passes in a row */

	/* we ignore the regular player_t score field altogether */
	int points_above_line[2];
	int game_count;		/* number of games completed; 0-2 */
	int points_below_line[3][2];
	int vulnerable[2];	/* also represents # of games won */

	/* contract information */
	player_t opener[2][5];	/* records which player on each team first
				   opened in each suit */
	int contract;		/* value of the contract */
	int contract_suit;	/* suit of the contract; 0-4 */
	int bonus;		/* 1=regular; 2=doubled; 4=redoubled */
	player_t declarer;	/* player with the contract */
	player_t dummy;		/* dummy player; the declarer's partner */

	int dummy_revealed;
} bridge_game_t;

static bool bridge_is_valid_game(void);
static int bridge_compare_cards(card_t, card_t);
static void bridge_init_game(void);
static void bridge_start_bidding(void);
static void bridge_get_bid(void);
static void bridge_handle_bid(player_t p, bid_t bid);
static void bridge_next_bid(void);
static void bridge_start_playing(void);
static void bridge_get_play(player_t p);
static void bridge_handle_play(player_t p, seat_t s, card_t card);
static bool bridge_test_for_gameover(void);
static void bridge_send_hand(player_t p, seat_t s);
static int bridge_get_bid_text(char *buf, size_t buf_len, bid_t bid);
static void bridge_set_player_message(player_t p);
static void bridge_end_hand(void);
static void bridge_start_game(void);

static void bridge_set_score_message(void);

game_data_t bridge_data = {
	"bridge",
	N_("Bridge"),
	"http://pagat.com/boston/bridge.html",
	bridge_is_valid_game,
	bridge_init_game,
	game_get_options,
	game_handle_option,
	game_get_option_text,
	bridge_set_player_message,
	game_sync_player,
	bridge_get_bid_text,
	game_get_bid_desc,
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
	game_end_trick,
	bridge_end_hand,
	bridge_start_game,
	bridge_test_for_gameover,
	game_handle_gameover,
	game_map_card,
	bridge_compare_cards,
	bridge_send_hand
};


/* these should be low, clubs, diamonds, ..., high, but that won't fit in the 
   client window */
static char *short_bridge_suit_names[5] = { "C", "D", "H", "S", "NT" };
static char *long_bridge_suit_names[5] =
	{ "clubs", "diamonds", "hearts", "spades", "notrump" };


static bool bridge_is_valid_game(void)
{
	return (game.num_players == 4);
}


static int bridge_compare_cards(card_t card1, card_t card2)
{
	/* in Bridge, the trump suit is always supposed to be shown on the
	   left */
	if (card1.suit == game.trump && card2.suit != game.trump)
		return -1;
	if (card2.suit == game.trump && card1.suit != game.trump)
		return 1;

	return game_compare_cards(card1, card2);
}

static void bridge_init_game(void)
{
	seat_t s;

	game.specific = ggz_malloc(sizeof(bridge_game_t));
	
	set_num_seats(4);
	set_num_teams(2);
	for (s = 0; s < game.num_seats; s++) {
		assign_seat(s, s);	/* one player per seat */
		assign_team(s % 2, s);
	}

	game.cumulative_scores = 0;

	/* TODO: for now we won't use bridge scoring */
	BRIDGE.declarer = -1;
}

static void bridge_start_bidding(void)
{
	int i, j;
	game.next_bid = game.dealer;	/* dealer bids first */
	game.bid_total = -1;	/* no set total */
	BRIDGE.pass_count = 0;
	BRIDGE.bonus = 1;
	for (i = 0; i < 2; i++)
		for (j = 0; j < 5; j++)
			BRIDGE.opener[i][j] = -1;
}

static void bridge_get_bid(void)
{
	char suit, val;

	/* this is based closely on the Suaro bidding code */

	/* make a list of regular bids */
	for (val = 1; val <= 7; val++) {
		for (suit = CLUBS; suit <= BRIDGE_NOTRUMP; suit++) {
			if (val < BRIDGE.contract)
				continue;
			if (val == BRIDGE.contract
			    && suit <= BRIDGE.contract_suit)
				continue;

			add_sbid(val, suit, 0);
		}
	}

	/* make "double" or "redouble" bid */
	if (BRIDGE.contract != 0 &&
	    BRIDGE.bonus == 1 &&
	    (game.next_bid == (BRIDGE.declarer + 1) % 4
	     || game.next_bid == (BRIDGE.declarer + 3) % 4))
		add_sbid(0, 0, BRIDGE_DOUBLE);
	else if (BRIDGE.contract != 0 &&
		 BRIDGE.bonus == 2 &&
		 (game.next_bid == BRIDGE.declarer
		  || game.next_bid == (BRIDGE.declarer + 2) % 4))
		add_sbid(0, 0, BRIDGE_REDOUBLE);

	/* make "pass" bid */
	add_sbid(0, 0, BRIDGE_PASS);

	request_client_bid(game.next_bid);
}

static void bridge_handle_bid(player_t p, bid_t bid)
{
	assert(game.next_bid == p);
	/* closely based on the Suaro code */
	ggz_debug(DBG_GAME, "The bid chosen is %d %s %d.", bid.sbid.val,
		    short_bridge_suit_names[(int) bid.sbid.suit],
		    bid.sbid.spec);

	if (bid.sbid.spec == BRIDGE_PASS) {
		BRIDGE.pass_count++;
	} else if (bid.sbid.spec == BRIDGE_DOUBLE
		   || bid.sbid.spec == BRIDGE_REDOUBLE) {
		BRIDGE.pass_count = 1;
		BRIDGE.bonus *= 2;
	} else {
		BRIDGE.contract = bid.sbid.val;
		BRIDGE.contract_suit = bid.sbid.suit;
		BRIDGE.bonus = 1;
		BRIDGE.pass_count = 1;

		if (BRIDGE.opener[p % 2][BRIDGE.contract_suit] == -1)
			BRIDGE.opener[p % 2][BRIDGE.contract_suit] = p;
		BRIDGE.declarer = BRIDGE.opener[p % 2][BRIDGE.contract_suit];
		BRIDGE.dummy = (BRIDGE.declarer + 2) % 4;

		ggz_debug(DBG_GAME, "Setting bridge contract to %d %s.",
			    BRIDGE.contract,
			    long_bridge_suit_names[BRIDGE.contract_suit]);
		if (bid.sbid.suit != BRIDGE_NOTRUMP)
			game.trump = bid.sbid.suit;
		else
			game.trump = -1;
	}
}

static void bridge_next_bid(void)
{
	/* closely based on Suaro code, below */
	if (BRIDGE.pass_count == 4) {
		/* done bidding */
		if (BRIDGE.contract == 0) {
			ggz_debug(DBG_GAME, "Four passes; redealing hand.");
			set_global_message("", "Everyone passed; redealing.");
			set_game_state(STATE_NEXT_HAND);	/* redeal
								   hand */
		} else {
			ggz_debug(DBG_GAME,
				    "Three passes; bidding is over.");
			game.bid_total = game.bid_count;
			/* contract was determined in game_handle_bid */
		}
	} else {
		if (game.bid_count == 0)
			game.next_bid = game.dealer;
		else
			game.next_bid =
				(game.next_bid + 1) % game.num_players;
	}
}

static void bridge_start_playing(void)
{
	game.trick_total = game.hand_size;
	game.play_total = game.num_players;

	/* declarer is set in game_handle_bid */
	set_global_message("", "Contract: %d %s%s.",
			   BRIDGE.contract,
			   long_bridge_suit_names[(int) BRIDGE.contract_suit],
			   BRIDGE.bonus == 1 ? "" : BRIDGE.bonus ==
			   2 ? ", doubled" : ", redoubled");
	game.leader = (BRIDGE.declarer + 1) % game.num_players;
}

static void bridge_get_play(player_t p)
{
	if (p == BRIDGE.dummy)
		/* the declarer plays the dummy's hand */
		request_client_play(BRIDGE.declarer, game.players[p].seat);
	else
		game_get_play(p);
}

static void bridge_handle_play(player_t p, seat_t s, card_t card)
{
	game_handle_play(p, s, card);

	if (game.play_count == 1 && game.trick_count == 0) {
		/* after the first play of the hand, we reveal the dummy's
		   hand to everyone */
		player_t p2;
		seat_t dummy_seat = game.players[BRIDGE.dummy].seat;
		cards_sort_hand(&game.seats[dummy_seat].hand);
		BRIDGE.dummy_revealed = 1;
		for (p2 = 0; p2 < game.num_players; p2++) {
			/* if (p2 == BRIDGE.dummy) continue; */
			game.data->send_hand(p2, dummy_seat);
		}
	}
}

static bool bridge_test_for_gameover(void)
{
	/* TODO: implement bridge scoring */
	return 0;
}

static void bridge_send_hand(player_t p, seat_t s)
{
	/* we explicitly send out the dummy hand, but a player who joins late
	   won't see it.  We have the same problem with Suaro. */
	if (s == BRIDGE.dummy	/* player/seat crossover; ok because it's
				   bridge */
	    && BRIDGE.dummy_revealed)
		send_hand(p, s, TRUE, TRUE);
	else
		game_send_hand(p, s);
}

static int bridge_get_bid_text(char *buf, size_t buf_len, bid_t bid)
{

	if (bid.sbid.spec == BRIDGE_PASS)
		return snprintf(buf, buf_len, "Pass");
	if (bid.sbid.spec == BRIDGE_DOUBLE)
		return snprintf(buf, buf_len, "Double");
	if (bid.sbid.spec == BRIDGE_REDOUBLE)
		return snprintf(buf, buf_len, "Redouble");
	if (bid.sbid.val > 0)
		return snprintf(buf, buf_len, "%d %s", bid.sbid.val,
				short_bridge_suit_names[(int) bid.sbid.suit]);
	return snprintf(buf, buf_len, "%s", "");
}

static void bridge_set_player_message(player_t p)
{
	seat_t s = game.players[p].seat;

	put_player_message(s, "%s", "");
	if (game.state != STATE_NEXT_BID) {
		if (p == BRIDGE.declarer)
			add_player_message(s, "declarer\n");
		if (p == BRIDGE.dummy)
			add_player_message(s, "dummy\n");
	}
	
	/* we show both the individual and team contract, if applicable.
	   But is this really the best way to tell if it's applicable? */
	if (game.state != STATE_NEXT_BID
	    && game.state != STATE_WAIT_FOR_BID) {
	
	} else
		add_player_bid_message(p);
		
	add_player_tricks_message(p);
	add_player_action_message(p);
}

static void bridge_set_score_message(void)
{
	player_t team;
	int widths[2], len = 0, i, g;
	char buf[4096] = "";

#define HORIZONTAL_LINE for (i=0; i<widths[0]+widths[1]+3; i++) \
		len += snprintf(buf+len, sizeof(buf)-len, "%c", '-'); \
	len += snprintf(buf+len, sizeof(buf)-len, "\n")
#define BLANK_LINE len += snprintf(buf+len, sizeof(buf)-len, "%*s | %*s\n", widths[0], "", widths[1], "")

	len = snprintf(buf, sizeof(buf), "%s/%s | %s/%s\n",
		       get_player_name(0), get_player_name(2),
		       get_player_name(1), get_player_name(3));
	for (team = 0; team < 2; team++)
		widths[team] =
			strlen(get_player_name(team)) +
			strlen(get_player_name(team + 2)) + 1;

	HORIZONTAL_LINE;
	BLANK_LINE;

	len += snprintf(buf + len, sizeof(buf) - len, "%*d | %-*d\n",
			widths[0], BRIDGE.points_above_line[0], widths[1],
			BRIDGE.points_above_line[1]);

	BLANK_LINE;

	for (g = 0; g <= BRIDGE.game_count; g++) {
		HORIZONTAL_LINE;
		len += snprintf(buf + len, sizeof(buf) - len, "%*d | %-*d\n",
				widths[0], BRIDGE.points_below_line[g][0],
				widths[1], BRIDGE.points_below_line[g][1]);
	}

	set_global_message("Scores", "%s", buf);
}

static void bridge_end_hand(void)
{
	int points_above = 0, points_below = 0, tricks, g;
	int winning_team, team;
	int tricks_above, tricks_below;
	int vulnerable = BRIDGE.vulnerable[BRIDGE.declarer % 2];
	char buf[512];
	char buf2[512] = "";
	char *bonus_text =
		BRIDGE.bonus == 1 ? "" : BRIDGE.bonus ==
		2 ? ", doubled" : ", redoubled";

	/* calculate tricks over book */
	tricks = game.players[BRIDGE.declarer].tricks +
		game.players[BRIDGE.dummy].tricks - 6;

	ggz_debug(DBG_GAME, "Contract was %d.  Declarer made %d.",
		    BRIDGE.contract, tricks);

	winning_team =
		(tricks >=
		 BRIDGE.contract) ? BRIDGE.declarer % 2 : (BRIDGE.declarer +
							   1) % 2;

	snprintf(buf2, sizeof(buf2), "%s and %s get:\n",
		 get_player_name(winning_team),
		 get_player_name(winning_team + 2));

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
			points_below =
				30 * tricks_below + (tricks_below >
						     0) ? 10 : 0;
			points_above = 30 * tricks_above;
			break;
		}

		points_below *= BRIDGE.bonus;
		points_above *= BRIDGE.bonus;

		snprintf(buf2 + strlen(buf2), sizeof(buf2) - strlen(buf2),
			 "  %d points below the line for %d %s%s.",
			 points_below, BRIDGE.contract,
			 long_bridge_suit_names[BRIDGE.contract_suit],
			 bonus_text);
		if (points_above)
			snprintf(buf2 + strlen(buf2),
				 sizeof(buf2) - strlen(buf2),
				 " %d points above the line for %d overtricks%s.",
				 points_above, tricks_above, bonus_text);

		/* you get a bonus just for making a doubled/redoubled
		   contract */
		if (BRIDGE.bonus > 1) {
			int insult_bonus = BRIDGE.bonus > 2 ? 100 : 50;
			snprintf(buf2 + strlen(buf2),
				 sizeof(buf2) - strlen(buf2),
				 "  %d points above the line for the insult.",
				 insult_bonus);
			points_above += insult_bonus;
		}

		if (BRIDGE.contract >= 6) {
			int slam_bonus;
			if (BRIDGE.contract == 6)
				slam_bonus = vulnerable ? 1000 : 500;
			else
				slam_bonus = vulnerable ? 1500 : 750;
			snprintf(buf2 + strlen(buf2),
				 sizeof(buf2) - strlen(buf2),
				 "  %d points for a %s slam%s.", slam_bonus,
				 BRIDGE.contract == 7 ? "grand" : "small",
				 vulnerable ? " while vulnerable" : "");
			points_above += slam_bonus;
		}
	} else {
		tricks_above = BRIDGE.contract - tricks;

		/* Penalty: not vulnerable vulnerable not doubled 50 100
		   doubled - 1st 100 200 doubled - 2nd,3rd 200 ea 300 ea
		   doubled - 4th+ 300 ea 300 ea redoubled 2x doubled 2x
		   doubled */

		if (BRIDGE.bonus == 1)
			points_above = tricks_above * (vulnerable ? 100 : 50);
		else {
			points_above =
				tricks_above * (vulnerable ? 200 : 100);
			if (tricks_above > 1)
				points_above += (tricks_above - 1) * 100;
			if (tricks_above > 3)
				points_above +=
					(tricks_above -
					 3) * (vulnerable ? 0 : 100);
			if (BRIDGE.bonus == 4)
				points_above *= 2;
		}
		snprintf(buf2 + strlen(buf2), sizeof(buf2) - strlen(buf2),
			 "  %d points above for setting by %d tricks%s%s.",
			 points_above, tricks_above,
			 bonus_text, vulnerable ? ", vulnerable" : "");

	}

	BRIDGE.points_above_line[winning_team] += points_above;
	BRIDGE.points_below_line[BRIDGE.game_count][winning_team] +=
		points_below;

	if (tricks >= BRIDGE.contract)
		snprintf(buf, sizeof(buf),
			 "%s made the bid and earned %d|%d points.",
			 get_player_name(BRIDGE.declarer), points_above,
			 points_below);
	else
		snprintf(buf, sizeof(buf),
			 "%s went set, giving up %d points.",
			 get_player_name(BRIDGE.declarer), points_above);

	/* TODO: points for honors */

	if (BRIDGE.points_below_line[BRIDGE.game_count][winning_team] >= 100) {
		if (BRIDGE.vulnerable[winning_team]) {
			/* they've won a rubber */
			int rubber_bonus =
				BRIDGE.vulnerable[1 -
						  winning_team] ? 500 : 700;
			BRIDGE.points_above_line[winning_team] +=
				rubber_bonus;
			for (team = 0; team < 2; team++) {
				for (g = 0; g <= BRIDGE.game_count; g++) {
					BRIDGE.points_above_line[team] +=
						BRIDGE.
						points_below_line[g][team];
					BRIDGE.points_below_line[g][team] = 0;
				}
			}
			BRIDGE.game_count = 0;
			snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
				 "  They won a %d-point rubber.",
				 rubber_bonus);
			/* right now, we jsut go right on into the next
			   rubber with a running score.  Instead, this should 
			   be the end of a game */
		} else {
			/* they've won their first game of the rubber */
			BRIDGE.game_count++;
			BRIDGE.vulnerable[winning_team] = 1;
			snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
				 "  They won a game.");
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

static void bridge_start_game(void)
{
	/* TODO: zero other scores */
	bridge_set_score_message();
	game_start_game();
}
