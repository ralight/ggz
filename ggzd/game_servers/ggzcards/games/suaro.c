/* 
 * File: games/suaro.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/02/2001
 * Desc: Game-dependent game functions for Suaro
 * $Id: suaro.c 8456 2006-08-02 06:00:35Z jdorje $
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
#include "options.h"
#include "score.h"

#include "suaro.h"

#define SUARO ( *(suaro_game_t *)(game.specific) )
typedef struct suaro_game_t {
	/* options */
	int shotgun;		/* are we playing shotgun suaro? */
	int unlimited_redoubling;	/* can doubling continue
					   indefinitely? */
	int persistent_doubles;	/* a double isn't negated by another bid */

	int pass_count;		/* number of passes in a row */

	/* contract information */
	int contract;		/* value of the contract */
	int kitty;		/* 0=>no kitty; 1=>kitty.  Only applicable in
				   shotgun suaro. */
	int contract_suit;	/* 0=>low, 5=>high, as above */
	int bonus;		/* 1 = regular; 2 = doubled; 4 = redoubled;
				   etc. */
	player_t declarer;	/* player with the contract */

	int kitty_revealed;
} suaro_game_t;

static bool suaro_is_valid_game(void);
static void suaro_init_game(void);
static void suaro_get_options(void);
static int suaro_handle_option(char *option, int value);
static char *suaro_get_option_text(char *buf, int bufsz, char *option,
				   int value);
static void suaro_start_bidding(void);
static void suaro_get_bid(void);
static void suaro_handle_bid(player_t p, bid_t bid);
static void suaro_next_bid(void);
static void suaro_start_playing(void);
static void suaro_deal_hand(void);
static void suaro_send_hand(player_t p, seat_t s);
static int suaro_get_bid_text(char *buf, size_t buf_len, bid_t bid);
static int suaro_get_bid_desc(char *buf, size_t buf_len, bid_t bid);
static void suaro_end_hand(void);
static void suaro_set_player_message(player_t p);
static void suaro_end_trick(void);

game_data_t suaro_data = {
	"suaro",
	N_("Suaro"),
	"http://suaro.dhs.org/",
	suaro_is_valid_game,
	suaro_init_game,
	suaro_get_options,
	suaro_handle_option,
	suaro_get_option_text,
	suaro_set_player_message,
	game_sync_player,
	suaro_get_bid_text,
	suaro_get_bid_desc,
	suaro_start_bidding,
	suaro_get_bid,
	suaro_handle_bid,
	suaro_next_bid,
	suaro_start_playing,
	game_verify_play,
	game_next_play,
	game_get_play,
	game_handle_play,
	suaro_deal_hand,
	suaro_end_trick,
	suaro_end_hand,
	game_start_game,
	game_test_for_gameover,
	game_handle_gameover,
	game_map_card,
	game_compare_cards,
	suaro_send_hand
};


 /* these should be low, clubs, diamonds, ..., high, but that won't fit in
    the client window */
char *short_suaro_suit_names[6] = { "lo", "C", "D", "H", "S", "hi" };
static char *long_suaro_suit_names[6] =
	{ "low", "clubs", "diamonds", "hearts", "spades", "high" };

static bool suaro_is_valid_game(void)
{
	return (game.num_players == 2);
}

static void suaro_init_game(void)
{
	game.specific = ggz_malloc(sizeof(suaro_game_t));
	set_num_seats(4);

	assign_seat(0, 0);	/* seat 0 => player 0 */
	assign_seat(2, 1);	/* seat 2 => player 1 */
	empty_seat(1, "Kitty");
	empty_seat(3, "Up-Card");

	game.deck_type = GGZ_DECK_SUARO;
	game.max_hand_length = 9;
	game.target_score = 50;

	SUARO.shotgun = 1;	/* shotgun suaro */
	SUARO.declarer = -1;
	game.last_trick = 1;	/* show "last trick" */
}

static void suaro_get_options(void)
{
	/* four options for now: shotgun -> boolean unlimited redoubling ->
	   boolean persistent doubles -> boolean target score -> 25, 50, 100, 
	   200, 500, 1000 */
	add_option("Suaro Options", "shotgun",
	           "In \"Shotgun Suaro\", you are allowed to bid on the kitty.",
	           1, 1,
	           "shotgun");
	add_option("Suaro Options", "unlimited_redoubling",
	           "With this variation, more than one redoubling is allowed.",
	           1, 0,
	           "unlimited redoubling");
	add_option("Suaro Options", "persistent_doubles",
	           "In this variation, if there is a (re)double and the bid "
	           "is subsequently changed, the (re)double will still apply.",
	           1, 0,
	           "persistent doubles");
	add_option("Suaro Options", "target",
	           "How many points does each player need to win?",
	           6, 1,
	           "Game to 25", "Game to 50", "Game to 100",
		   "Game to 200", "Game to 500", "Game to 1000");
	game_get_options();
}

static int suaro_handle_option(char *option, int value)
{
	if (!strcmp(option, "shotgun"))
		SUARO.shotgun = value;
	else if (!strcmp(option, "unlimited_redoubling"))
		SUARO.unlimited_redoubling = value;
	else if (!strcmp(option, "persistent_doubles"))
		SUARO.persistent_doubles = value;
	else if (!strcmp(option, "target")) {
		switch (value) {
			/* this highlights a problem with this protocol. Only 
			   discrete entries are possible. it would be better
			   if the client could just enter a number */
		case 0:
			game.target_score = 25;
			break;
		case 1:
			game.target_score = 50;
			break;
		case 2:
			game.target_score = 100;
			break;
		case 3:
			game.target_score = 200;
			break;
		case 4:
			game.target_score = 500;
			break;
		default:
		case 5:
			game.target_score = 1000;
			break;
		}
	} else
		return game_handle_option(option, value);
	return 0;
}

static char *suaro_get_option_text(char *buf, int bufsz, char *option,
				   int value)
{
	if (!strcmp(option, "shotgun"))
		snprintf(buf, bufsz, "You're %splaying Shotgun Suaro.",
			 value ? "" : "not ");
	else if (!strcmp(option, "unlimited_redoubling"))
		snprintf(buf, bufsz, "%s",
			 value ? "Unlimited redoubling is allowed." : "");
	else if (!strcmp(option, "persistent_doubles"))
		snprintf(buf, bufsz, "%s",
			 value ? "All doubles are persistent." : "");
	else if (!strcmp(option, "target"))
		/* shortcut by ignoring the "value" and just using the game's 
		   value */
		snprintf(buf, bufsz, "The game is being played to %d.",
			 game.target_score);
	else
		return game_get_option_text(buf, bufsz, option, value);
	return buf;
}

static void suaro_start_bidding(void)
{
	char suit;
	game.bid_total = -1;	/* no set total */

	/* key card determines first bidder */
	suit = game.seats[3].hand.cards[0].suit;
	game.next_bid = (suit == CLUBS || suit == SPADES) ? 0 : 1;

	SUARO.pass_count = 0;
	SUARO.bonus = 1;
}

static void suaro_get_bid(void)
{
	char val, suit;

	/* in suaro, a bid consists of a number and a suit. */

	/* make a list of regular bids */
	for (val = 5; val <= 9; val++) {
		for (suit = SUARO_LOW; suit <= SUARO_HIGH; suit++) {
			if (val < SUARO.contract)
				continue;
			if (val == SUARO.contract
			    && suit <= SUARO.contract_suit)
				continue;

			add_sbid(val, suit, 0);
			if (SUARO.shotgun)
				/* in "shotgun" suaro, you are allowed to bid 
				   on the kitty just like on your hand! */
				add_sbid(val, suit, SUARO_KITTY);
		}
	}

	/* make "double" or "redouble" bid */
	if (SUARO.contract > 0 && SUARO.bonus == 1)
		/* unless unlimited doubling is specifically allowed, only
		   double and redouble are possible */
		add_sbid(0, 0, SUARO_DOUBLE);
	else if (SUARO.contract > 0 &&
		 (SUARO.bonus < 4 || SUARO.unlimited_redoubling)) {
		add_sbid(0, 0, SUARO_REDOUBLE);
	}

	/* make "pass" bid */
	add_sbid(0, 0, SUARO_PASS);

	request_client_bid(game.next_bid);
}

static void suaro_handle_bid(player_t p, bid_t bid)
{
	assert(p == game.next_bid);
	if (bid.sbid.spec == SUARO_PASS) {
		SUARO.pass_count++;
	} else if (bid.sbid.spec == SUARO_DOUBLE ||
		   bid.sbid.spec == SUARO_REDOUBLE) {
		SUARO.pass_count = 1;	/* one more pass will end it */
		SUARO.bonus *= 2;
	} else {
		SUARO.declarer = p;
		SUARO.pass_count = 1;	/* one more pass will end it */
		if (!SUARO.persistent_doubles)
			SUARO.bonus = 1;	/* reset any doubles */
		SUARO.contract = bid.sbid.val;
		SUARO.kitty = (bid.sbid.spec == SUARO_KITTY);
		SUARO.contract_suit = bid.sbid.suit;
		if (bid.sbid.suit > SUARO_LOW && bid.sbid.suit < SUARO_HIGH)
			game.trump = bid.sbid.suit - 1;	/* a minor hack */
		else
			game.trump = -1;
	}
}

static void suaro_next_bid(void)
{
	if (SUARO.pass_count == 2) {
		/* done bidding */
		if (SUARO.contract == 0) {
			ggz_debug(DBG_GAME, "Two passes; redealing hand.");
			set_global_message("", "Everyone passed; redealing.");
			set_game_state(STATE_NEXT_HAND);	/* redeal
								   hand */
		} else {
			ggz_debug(DBG_GAME, "A pass; bidding is over.");
			game.bid_total = game.bid_count;
			/* contract was determined in suaro_handle_bid */
		}
	} else
		game_next_bid();
}

static void suaro_start_playing(void)
{
	player_t p;

	game.trick_total = game.hand_size;
	game.play_total = game.num_players;
	/* declarer is set in suaro_handle_bid */
	set_global_message("", "%s has the contract at %s%d %s%s.",
			   get_player_name(SUARO.declarer),
			   SUARO.kitty ? "kitty " : "",
			   SUARO.contract,
			   long_suaro_suit_names[(int) SUARO.contract_suit],
			   SUARO.bonus == 1 ? "" : SUARO.bonus ==
			   2 ? ", doubled" : ", redoubled");
	game.leader = 1 - SUARO.declarer;
	if (SUARO.kitty) {
		/* if it's a kitty bid, the declarer takes up the kitty and
		   lays their own hand down (face up) in its place */
		card_t *temp;
		seat_t s1, s2;
		s1 = 1;		/* kitty hand */
		s2 = game.players[SUARO.declarer].seat;
		/* this "swapping" only works because the cards in the hands
		   are specifically allocated by malloc.  Also note that
		   there are always 9 cards in both hands. */
		temp = game.seats[s1].hand.cards;
		game.seats[s1].hand.cards = game.seats[s2].hand.cards;
		game.seats[s2].hand.cards = temp;
		SUARO.kitty_revealed = 1;
		for (p = 0; p < game.num_players; p++) {
			/* resend the hands */
			game.data->send_hand(p, s1);
			game.data->send_hand(p, s2);
		}

	}
}

static void suaro_deal_hand(void)
{

	seat_t s;
	/* in suaro, players 0 and 1 (seats 0 and 2) get 9 cards each. the
	   kitty (seat 1) also gets 9 cards, and the face card (seat 3) is
	   just one card. */
	game.hand_size = 9;
	for (s = 0; s < 3; s++)
		deal_hand(game.deck, game.hand_size,
				&game.seats[s].hand);
	deal_hand(game.deck, 1, &game.seats[3].hand);
}

static void suaro_send_hand(player_t p, seat_t s)
{
	bool show_fronts;
	if (s == 1 && SUARO.kitty_revealed) {
		/* reveal the kitty after it's been turned up */
		show_fronts = TRUE;
	} else {
		/* each player can see their own hand plus the key card */
		show_fronts = (game.players[p].seat == s || s == 3);
	}
	send_hand(p, s, show_fronts, TRUE);
}

static int suaro_get_bid_text(char *buf, size_t buf_len, bid_t bid)
{
	if (bid.sbid.spec == SUARO_PASS)
		return snprintf(buf, buf_len, "Pass");
	if (bid.sbid.spec == SUARO_DOUBLE)
		return snprintf(buf, buf_len, "Double");
	if (bid.sbid.spec == SUARO_REDOUBLE)
		return snprintf(buf, buf_len, "Redouble");
	if (bid.sbid.val > 0)
		return snprintf(buf, buf_len, "%s%d %s",
				(bid.sbid.spec == SUARO_KITTY) ? "K " : "",
				bid.sbid.val,
				short_suaro_suit_names[(int) bid.sbid.suit]);
	return snprintf(buf, buf_len, "%s", "");
}

static int suaro_get_bid_desc(char *buf, size_t buf_len, bid_t bid)
{
	if (bid.sbid.spec == SUARO_PASS)
		return snprintf(buf, buf_len, "Pass - do not bid");
	if (bid.sbid.spec == SUARO_DOUBLE)
		return snprintf(buf, buf_len,
		                "Double the points earned on the hand");
	if (bid.sbid.spec == SUARO_REDOUBLE)
		return snprintf(buf, buf_len,
		                "Redouble the points earned on the hand");
	if (bid.sbid.val > 0)
		return snprintf(buf, buf_len, "Contract - %s%d %s",
				(bid.sbid.spec == SUARO_KITTY) ? "Kitty " : "",
				bid.sbid.val,
				long_suaro_suit_names[(int) bid.sbid.suit]);
	return snprintf(buf, buf_len, "%s", "");
}

static void suaro_end_hand(void)
{
	int points, tricks;
	player_t winner;

	tricks = game.players[SUARO.declarer].tricks;
	if (tricks >= SUARO.contract) {
		int overtricks = tricks - SUARO.contract;
		winner = SUARO.declarer;
		/* you get the value of the contract MINUS the number of
		   overtricks == 2 * contract - tricks */
		points = (SUARO.contract - overtricks) * SUARO.bonus;
		ggz_debug(DBG_GAME,
			    "Player %d/%s made their bid of %d, plus %d overtricks for %d points.",
			    winner, get_player_name(winner),
			    SUARO.contract, overtricks, points);
	} else {
		winner = 1 - SUARO.declarer;
		/* for setting, you just get the value of the contract */
		points = SUARO.contract * SUARO.bonus;
		ggz_debug(DBG_GAME,
			    "Player %d/%s set the bid of %d, earning %d points.",
			    winner, get_player_name(winner),
			    SUARO.contract, points);
	}
	set_global_message("", "%s %s the bid and earned %d point%s.",
			   get_player_name(winner),
			   winner == SUARO.declarer ? "made" : "set",
			   points, points == 1 ? "" : "s");
	change_score(game.players[winner].team, points);
	SUARO.declarer = -1;
	SUARO.kitty_revealed = 0;
	SUARO.contract = 0;
	set_player_message(winner);
}

static void suaro_set_player_message(player_t p)
{
	seat_t s = game.players[p].seat;

	clear_player_message(s);
	add_player_rating_message(p);
	add_player_score_message(p);
	add_player_tricks_message(p);
	if (game.state != STATE_NEXT_BID) {
		if (p == SUARO.declarer)
			add_player_message(s, "declarer\n");
		if (p == 1 - SUARO.declarer)
			add_player_message(s, "defender\n");
	}
	add_player_bid_message(p);
	add_player_action_message(p);
}

static void suaro_end_trick(void)
{
	player_t hi_player = game.leader, p, lo_player = game.leader;
	card_t hi_card = game.lead_card, lo_card = game.lead_card;

	/* default method of winning tricks: the winning card is the highest
	   card of the suit lead, or the highest trump if there is trump */
	for (p = 0; p < game.num_players; p++) {
		card_t card = game.seats[game.players[p].seat].table;
		card = game.data->map_card(card);
		if ((card.suit == game.trump
		     && (hi_card.suit != game.trump
			 || hi_card.face < card.face))
		    || (card.suit == hi_card.suit
			&& card.face > hi_card.face)) {
			hi_card = card;
			hi_player = p;
		}
		if (card.suit == lo_card.suit && card.face < lo_card.face) {
			/* tracking the low card is unnecessary in most
			   cases, but it saves us trouble in a few cases
			   (like "low" bids in Suaro) */
			lo_card = card;
			lo_player = p;
		}
	}

	if (SUARO.contract_suit == SUARO_LOW) {
		/* low card wins */
		hi_player = lo_player;
		hi_card = lo_card;
	}

	game.players[hi_player].tricks++;
	game.leader = game.winner = hi_player;

	set_player_message(hi_player);
}
