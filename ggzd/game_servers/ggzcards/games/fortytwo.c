/*
 * File: games/fortytwo.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 04/21/2002
 * Desc: Game-dependent game functions for Forty-Two
 * $Id: fortytwo.c 4054 2002-04-23 00:52:21Z jdorje $
 *
 * Copyright (C) 2001-2002 GGZ Development Team.
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

#include "fortytwo.h"

#include "bid.h"
#include "common.h"
#include "game.h"
#include "games.h"
#include "message.h"
#include "team.h"

#define FORTYTWO ( *(fortytwo_game_t *)(game.specific) )
typedef struct {
	player_t declarer;
	int contract;
	
	char count[4];
} fortytwo_game_t;


static bool fortytwo_is_valid_game(void);
static void fortytwo_init_game(void);
static void fortytwo_set_player_message(player_t p);
static int fortytwo_get_bid_text(char *buf, size_t buf_len, bid_t bid);
static int fortytwo_get_bid_desc(char *buf, size_t buf_len, bid_t bid);
static void fortytwo_get_bid(void);
static void fortytwo_handle_bid(player_t p, bid_t bid);
static void fortytwo_start_playing(void);
//static char *fortytwo_verify_play(player_t p, card_t card);
static void fortytwo_handle_play(player_t p, seat_t s, card_t c);
static void fortytwo_end_trick(void);
static void fortytwo_end_hand(void);
static card_t fortytwo_map_card(card_t card);


game_data_t fortytwo_data = {
	"fortytwo",
	N_("Forty-two"),
	fortytwo_is_valid_game,
	fortytwo_init_game,
	game_get_options,
	game_handle_option,
	game_get_option_text,
	fortytwo_set_player_message,
	fortytwo_get_bid_text,
	fortytwo_get_bid_desc,
	game_start_bidding,
	fortytwo_get_bid,
	fortytwo_handle_bid,
	game_next_bid,
	fortytwo_start_playing,
	game_verify_play,
	game_next_play,
	game_get_play,
	fortytwo_handle_play,
	game_deal_hand,
	fortytwo_end_trick,
	fortytwo_end_hand,
	game_start_game,
	game_test_for_gameover,
	game_handle_gameover,
	fortytwo_map_card,
	game_compare_cards,
	game_send_hand
};

static bool fortytwo_is_valid_game(void)
{
	return game.num_players == 4;
}

static void fortytwo_init_game(void)
{
	seat_t s;

	game.specific = ggz_malloc(sizeof(fortytwo_game_t));
	
	set_num_seats(4);
	set_num_teams(2);
	for (s = 0; s < game.num_seats; s++) {
		assign_seat(s, s);
		assign_team(s % 2, s);
	}
	
	game.deck_type = GGZ_DECK_DOMINOES;
	game.target_score = 7;
	game.cumulative_scores = TRUE;
	game.rules_url = "---";
	
	/* TODO: specific fortytwo initializations */
	FORTYTWO.declarer = -1;
}

/* Sets the player message for a given player. */
static void fortytwo_set_player_message(player_t p)
{
	clear_player_message(game.players[p].seat);
	add_player_rating_message(p);
	add_player_score_message(p);
	add_player_tricks_message(p);
	
	if (game.state != STATE_NEXT_BID
	    && p == FORTYTWO.declarer)
		add_player_message(p, "Contract: %d\n", FORTYTWO.contract);
		
	if (game.state == STATE_NEXT_BID
	    || game.state == STATE_WAIT_FOR_BID)
		add_player_bid_message(p);
		
	add_player_action_message(p);
}

static int fortytwo_get_bid_text(char *buf, size_t buf_len, bid_t bid)
{
	if (bid.sbid.spec == FORTYTWO_PASS)
		return snprintf(buf, buf_len, "Pass");
	if (bid.sbid.spec == FORTYTWO_DOUBLE)
		return snprintf(buf, buf_len, "%d", 42 * bid.sbid.val);
	return snprintf(buf, buf_len, "%d", bid.sbid.val);
}

static int fortytwo_get_bid_desc(char *buf, size_t buf_len, bid_t bid)
{
	if (bid.sbid.spec == FORTYTWO_PASS)
		return snprintf(buf, buf_len, "Pass - do not bid");
	if (bid.sbid.spec == FORTYTWO_DOUBLE)
		return snprintf(buf, buf_len,
		                "Contract to take all 42 points "
		                "at x%d value.", bid.sbid.val);
	return snprintf(buf, buf_len, "Contract to take %d points", bid.sbid.val);
}

static void fortytwo_get_bid(void)
{
	char minbid = 30;
	char val;
	
	/* Bids up to (and including) 42 */
	if (FORTYTWO.declarer >= 0)
		minbid = FORTYTWO.contract + 1;
	for (val = minbid; val <= 42; val++)
		add_sbid(val, 0, FORTYTWO_BID);
	
	/* A higher bid: 84/126/168/210 */
	val = 2;
	while (val * 42 <= minbid)
		val++;
	add_sbid(val, 0, FORTYTWO_DOUBLE);
	
	/* Or pass */
	if (FORTYTWO.declarer >= 0 ||
	    game.next_bid != game.dealer)
		add_sbid(0, 0, FORTYTWO_PASS);
	
	request_client_bid(game.next_bid);
	
}

static void fortytwo_handle_bid(player_t p, bid_t bid)
{
	int bid_contract;
	
	if (bid.sbid.spec == FORTYTWO_PASS)
		return;
	
	if (bid.sbid.spec == FORTYTWO_DOUBLE)
		bid_contract = 42 * bid.sbid.val;
	else
		bid_contract = bid.sbid.val;
	assert(FORTYTWO.declarer <= 0 || FORTYTWO.contract < bid_contract);
	
	FORTYTWO.declarer = p;
	FORTYTWO.contract = bid_contract;
}

static void fortytwo_start_playing(void)
{
	player_t p;
		
	game_start_playing();
	
	for (p = 0; p < 4; p++)
		FORTYTWO.count[p] = 0;
	game.leader = FORTYTWO.declarer;
}

static void fortytwo_handle_play(player_t p, seat_t s, card_t c)
{
	if (game.play_count == 0 && game.trick_count == 0) {
		seat_t s;
		/* After the first play we know trump. */
		/* FIXME: this is wrong, wrong, wrong! */
		card_t card = game.seats[game.leader].table;
		game.trump = card.suit;
		set_global_message("Trump", "%s are trump.",
		                   get_suit_name(game.trump));
		set_global_message("", "Trump is %s.",
		                   get_suit_name(game.trump));
		for (s = 0; s < game.num_seats; s++) {
			player_t p;
			cards_sort_hand(&game.seats[s].hand);
			for (p = 0; p < game.num_players; p++)
				(void) game.data->send_hand(p, s);
		}
	}
}

static void fortytwo_end_trick(void)
{
	char count = 0;
	player_t p;

	for (p = 0; p < game.num_players; p++) {
		card_t card = game.seats[p].table;
		char card_count = card.suit + card.face;
		if (card_count % 5 != 0)
			card_count = 0;
		count += card_count;
	}
		
	game_end_trick();
	
	FORTYTWO.count[game.winner] += count;
	FORTYTWO.count[(game.winner + 2) % 4] += count;
}

static void fortytwo_end_hand(void)
{
	int points = 0;
	team_t declarer_team = FORTYTWO.declarer % 2;
	team_t opp_team = (declarer_team + 1) % 2;
	team_t winning_team;
	int target_points = FORTYTWO.contract > 42 ? 42 : FORTYTWO.contract;
	int score = (FORTYTWO.contract + 41) / 42;
	
	points += get_team_tricks(declarer_team);
	points += FORTYTWO.count[FORTYTWO.declarer];
	
	if (points >= target_points) {
		/* They made the bid! */
		winning_team = declarer_team;
		set_global_message("", "%s made the bid and earned %d points.",
		                   get_player_name(FORTYTWO.declarer),
		                   score);
	} else {
		/* They're set... */
		winning_team = opp_team;
		set_global_message("", "%s went set and gave up %d points.",
		                   get_player_name(FORTYTWO.declarer),
		                   score);
	}

	game.players[winning_team].score += score;
	game.players[winning_team + 2].score += score;
	map_func_to_team(winning_team, set_player_message);
	
	game.trump = -1;
	FORTYTWO.declarer = -1;
}

static card_t flop_suit(card_t card)
{
	char tmp = card.face;
	card.face = card.suit;
	card.suit = tmp;
	return card;
}

static card_t fortytwo_map_card(card_t card)
{
	/* Trump can never be of a different suit */
	if (card.face == game.trump)
		card = flop_suit(card);
		
	/* Non-trump may be of either suit when following lead,
	   but always of the higher suit when leading. */
	if (card.suit != game.trump
	    && game.state == STATE_WAIT_FOR_PLAY
	    && !game.players[game.leader].is_playing
	    && card.face == game.seats[game.leader].table.suit)
		card = flop_suit(card);
		
	/* Doubles count high */
	if (card.suit == card.face)
		card.face = 7;

	return card;
}
