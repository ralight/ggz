/* 
 * File: games/euchre.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for Euchre
 * $Id: euchre.c 3570 2002-03-16 06:36:32Z jdorje $
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "bid.h"
#include "common.h"
#include "game.h"
#include "message.h"
#include "options.h"
#include "play.h"
#include "team.h"

#include "euchre.h"

static int euchre_is_valid_game(void);
static card_t euchre_map_card(card_t c);
static void euchre_init_game(void);
static void euchre_get_options(void);
static int euchre_handle_option(char *option, int value);
static char *euchre_get_option_text(char *buf, int bufsz, char *option,
				    int value);
static void euchre_start_bidding(void);
static int euchre_get_bid(void);
static void euchre_handle_bid(player_t p, bid_t bid);
static void euchre_next_bid(void);
static void euchre_start_playing(void);
static void euchre_get_play(player_t p);
static void euchre_deal_hand(void);
static int euchre_get_bid_text(char *buf, size_t buf_len, bid_t bid);
static void euchre_set_player_message(player_t p);
static void euchre_end_hand(void);

struct game_function_pointers euchre_funcs = {
	euchre_is_valid_game,
	euchre_init_game,
	euchre_get_options,
	euchre_handle_option,
	euchre_get_option_text,
	euchre_set_player_message,
	euchre_get_bid_text,
	euchre_start_bidding,
	euchre_get_bid,
	euchre_handle_bid,
	euchre_next_bid,
	euchre_start_playing,
	game_verify_play,
	game_next_play,
	euchre_get_play,
	game_handle_play,
	euchre_deal_hand,
	game_end_trick,
	euchre_end_hand,
	game_start_game,
	game_test_for_gameover,
	game_handle_gameover,
	euchre_map_card,
	game_compare_cards,
	game_send_hand
};


static int euchre_is_valid_game(void)
{
	return (game.num_players == 4);
}

static card_t euchre_map_card(card_t c)
{
	/* The "right bauer" is the jack of the trump suit.  It is the high
	   trump.  The "left bauer" is the other jack of the same color; it
	   is the second highest trump. */
	if (c.face == JACK && c.suit == 3 - game.trump) {
		c.suit = game.trump;
		c.face = ACE_HIGH + 1;
	} else if (c.face == JACK && c.suit == game.trump)
		c.face = ACE_HIGH + 2;
	return c;
}

static void euchre_init_game(void)
{
	seat_t s;

	/* Game seat data */
	set_num_seats(4);
	set_num_teams(2);
	for (s = 0; s < game.num_seats; s++) {
		assign_seat(s, s);	/* one player per seat */
		assign_team(s % 2, s);
	}

	/* Game-type options */
	game.deck_type = GGZ_DECK_EUCHRE;
	game.max_hand_length = 5;
	game.target_score = 10;
	game.trump = -1;

	/* Game-specific data */
	game.specific = ggz_malloc(sizeof(euchre_game_t));
	EUCHRE.maker = -1;
}

static void euchre_get_options(void)
{
	add_option("screw_the_dealer", 1, 0, "Stick the dealer");
	add_option("super_euchre", 1, 0, "Allow \"super euchre\"");
	game_get_options();
}


static int euchre_handle_option(char *option, int value)
{
	if (!strcmp("screw_the_dealer", option))
		EUCHRE.screw_the_dealer = value;
	else if (!strcmp("super_euchre", option))
		EUCHRE.super_euchre = value;
	else
		return game_handle_option(option, value);
	return 0;
}


static char *euchre_get_option_text(char *buf, int bufsz, char *option,
				    int value)
{
	if (!strcmp("screw_the_dealer", option)) {
		if (value)
			snprintf(buf, bufsz, "Playing \"stick the dealer\".");
		else
			*buf = 0;
	} else if (!strcmp("super_euchre", option)) {
		if (value)
			snprintf(buf, bufsz, "Allowing \"super euchre\".");
		else
			*buf = 0;
	} else
		return NULL;
	return buf;
}

static void euchre_start_bidding(void)
{
	game.bid_total = 8;	/* twice around, at most */
	game.next_bid = (game.dealer + 1) % game.num_players;
	EUCHRE.req_alone_bid = 0;
	memset(EUCHRE.going_alone, 0, sizeof(*EUCHRE.going_alone) * 4);
}

static int euchre_get_bid(void)
{
	player_t p;
	if (EUCHRE.maker >= 0) {
		if (EUCHRE.req_alone_bid)	/* don't do it more than once 
						 */
			return 0;
		/* If the maker has been chosen, then we need to request bids 
		   from everyone to see if they're going alone or not. */
		for (p = 0; p < 4; p++) {
			if ((p + 2) % 4 != EUCHRE.maker) {
				game.next_bid = p;	/* hack */
				add_sbid(0, 0, EUCHRE_GO_ALONE);
				add_sbid(0, 0, EUCHRE_GO_TEAM);
			}
		}
		EUCHRE.req_alone_bid = 1;
		return request_all_bids();
	} else if (game.bid_count < 4) {
		/* Tirst four bids are either "pass" or "take".  The suit of
		   the up-card becomes trump. */
		add_sbid(0, 0, EUCHRE_PASS);
		add_sbid(0, 0, EUCHRE_TAKE);
		return req_bid(game.next_bid);
	} else {
		/* After we've bid around, the bidding becomes either "pass"
		   or "take" with a specific suit. */
		char suit;

		/* The dealer (8th and last bid) is not allowed to pass a
		   second time (if we're playing "screw the dealer", that is. 
		 */
		if (!EUCHRE.screw_the_dealer || game.bid_count != 7)
			add_sbid(0, 0, EUCHRE_PASS);
		for (suit = 0; suit < 4; suit++)
			add_sbid(0, suit, EUCHRE_TAKE_SUIT);
		return req_bid(game.next_bid);
	}
}

static void euchre_handle_bid(player_t p, bid_t bid)
{
	char bid_text[4096];
	euchre_get_bid_text(bid_text, sizeof(bid_text), bid);
	ggzdmod_log(game.ggz, "EUCHRE: handling bid %s.", bid_text);
	switch (bid.sbid.spec) {
	case EUCHRE_TAKE:
		EUCHRE.maker = p;
		game.trump = EUCHRE.up_card.suit;
		game.bid_total = game.bid_count + 4;	/* hack: 3 more bids
							   after this one */
		break;
	case EUCHRE_TAKE_SUIT:
		EUCHRE.maker = p;
		game.trump = bid.sbid.suit;
		game.bid_total = game.bid_count + 4;	/* hack: 3 more bids
							   after this one */
		break;
	case EUCHRE_GO_ALONE:
		EUCHRE.going_alone[p] = 1;
		break;
	case EUCHRE_GO_TEAM:
		EUCHRE.going_alone[p] = 0;	/* redundant */
		break;
	}
}

static void euchre_next_bid(void)
{
	if (EUCHRE.maker >= 0) {
		/* taken care of by euchre_handle_bid. Ugly! */
	} else if (game.bid_count == 8) {
		/* This should only happen if we aren't playing "screw the
		   dealer". */
		set_global_message("", "Everyone passed; redealing.");
		set_game_state(STATE_NEXT_HAND);
	} else
		game_next_bid();
}

static void euchre_start_playing(void)
{
	player_t p;
	seat_t s;

	game_start_playing();

	assert(EUCHRE.maker >= 0);	/* maker is set in euchre_handle_bid */

	/* Only one teammate can "go alone" */
	for (p = 0; p < 2; p++)
		if (EUCHRE.going_alone[p] && EUCHRE.going_alone[p + 2]) {
			EUCHRE.going_alone[p + 2 * (random() % 2)] = 0;
		}

	/* Players "going alone" don't have a partner to play. */
	game.play_total = 4;
	for (p = 0; p < 4; p++)
		if (EUCHRE.going_alone[p]) {
			set_player_message(p);
			game.play_total--;
			game.seats[(p + 2) % 4].hand.hand_size = 0;
		}

	set_global_message("", "%s is the maker in %s.",
			   get_player_name(EUCHRE.maker),
			   suit_names[(int) game.trump]);
	game.leader = (game.dealer + 1) % game.num_players;

	/* resort/resend hand - this should be a separate function */
	for (s = 0; s < game.num_seats; s++) {
		cards_sort_hand(&game.seats[s].hand);
		for (p = 0; p < game.num_players; p++)
			(void) game.funcs->send_hand(p, s);
	}
}

static void euchre_get_play(player_t p)
{
	while (EUCHRE.going_alone[(p + 2) % 4]) {
		int p2 = (p + 1) % 4;
		ggzdmod_log(game.ggz,
			    "EUCHRE: skipping player %d/%s; going on to player %d/%s",
			    p, get_player_name(p), p2, get_player_name(p2));
		p = p2;
	}

	game.next_play = p;	/* hack: we need to increment this too... */

	/* in almost all cases, we just want the player to play from their
	   own hand */
	req_play(p, p);
}

static void euchre_deal_hand(void)
{
	seat_t s;

	/* in Euchre, players 0-3 (seats 0, 1, 3, 4) get 5 cards each. the
	   up-card (seat 5) gets one card, and the kitty (seat 2) gets the
	   other 3. */
	EUCHRE.up_card = deal_card(game.deck);
	set_global_message("", "The up-card is the %s of %s.",
			   face_names[(int) EUCHRE.up_card.face],
			   suit_names[(int) EUCHRE.up_card.suit]);
	/* FIXME: this message should be cardlist-style instead. */
	set_global_message("Up-Card", "%s of %s",
			   face_names[(int) EUCHRE.up_card.face],
			   suit_names[(int) EUCHRE.up_card.suit]);
	game.hand_size = 5;

	/* in a regular deal, we just deal out hand_size cards to everyone */
	for (s = 0; s < game.num_seats; s++)
		deal_hand(game.deck, game.hand_size,
				&game.seats[s].hand);
}

static int euchre_get_bid_text(char *buf, size_t buf_len, bid_t bid)
{
	switch (bid.sbid.spec) {
	case EUCHRE_PASS:
		return snprintf(buf, buf_len, "Pass");
	case EUCHRE_TAKE:
		return snprintf(buf, buf_len, "Take");
	case EUCHRE_TAKE_SUIT:
		return snprintf(buf, buf_len, "Take at %s",
				suit_names[(int) bid.sbid.suit]);
	case EUCHRE_GO_ALONE:
		return snprintf(buf, buf_len, "Go alone");
	case EUCHRE_GO_TEAM:
		return snprintf(buf, buf_len, "Go team");	/* ? */
	}
	return snprintf(buf, buf_len, "<error: no bid message>");
}

static void euchre_set_player_message(player_t p)
{
	seat_t s = game.players[p].seat;

	clear_player_message(s);
	add_player_rating_message(p);
	add_player_score_message(p);
	if (game.state == STATE_FIRST_BID || game.state == STATE_NEXT_BID) {
		if (p == game.dealer)
			add_player_message(s, "dealer\n");
	} else if (p == EUCHRE.maker)
		add_player_message(s, "maker\n");
	add_player_bid_message(p);
	if (game.state == STATE_WAIT_FOR_PLAY
	    || game.state == STATE_NEXT_TRICK
	    || game.state == STATE_NEXT_PLAY) {
		if (EUCHRE.going_alone[p])
			add_player_message(s, "Going Alone\n");
	}
	add_player_tricks_message(p);
	add_player_action_message(p);
}

static void euchre_end_hand(void)
{
	int winning_team, value;
	char *msg, buf[4096];

	int tricks =
		game.players[EUCHRE.maker].tricks +
		game.players[(EUCHRE.maker + 2) % 4].tricks;
	int maker_made = (tricks >= 3);
	int maker_alone = EUCHRE.going_alone[EUCHRE.maker];
	int defender_alone = EUCHRE.going_alone[(EUCHRE.maker + 1) % 4]
		|| EUCHRE.going_alone[(EUCHRE.maker + 3) % 4];

	if (maker_made) {
		winning_team = EUCHRE.maker % 2;
		value = (tricks == 5) ? 2 : 1;
		msg = "The maker made the bid with %d tricks and earned %d point(s).";
	} else {
		/* Euchred!!! */
		tricks = 5 - tricks;
		winning_team = (EUCHRE.maker + 1) % 2;
		value = 2;
		if (tricks == 5 && EUCHRE.super_euchre)
			value = 4;
		msg = "The bid was Euchred!  The defenders took %d tricks and earn %d points.";
	}

	if (maker_made && maker_alone && tricks == 5) {
		value = 4;
	} else if (!maker_made && defender_alone) {
		value = 4;
	}

	game.players[winning_team].score += value;
	game.players[winning_team + 2].score += value;

	snprintf(buf, sizeof(buf), msg, tricks, value);
	/* This message is quickly overwritten by the up-card message.  Ugh. */
	set_global_message("", buf);
	set_global_message("Scoring History", buf);	/* FIXME: this should 
							   be added to the
							   history, not
							   overwrite it. */

	EUCHRE.maker = -1;
	game.trump = -1;
}
