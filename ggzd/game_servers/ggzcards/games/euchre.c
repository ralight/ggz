/*
 * File: games/euchre.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for Euchre
 * $Id: euchre.c 2189 2001-08-23 07:59:17Z jdorje $
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

#include "../common.h"

#include "euchre.h"

static int euchre_is_valid_game();
static card_t euchre_map_card(card_t c);
static void euchre_init_game();
static void euchre_start_bidding();
static int euchre_get_bid();
static void euchre_handle_bid(bid_t bid);
static void euchre_next_bid();
static void euchre_start_playing();
static int euchre_deal_hand();
static int euchre_send_hand(player_t p, seat_t s);
static int euchre_get_bid_text(char* buf, int buf_len, bid_t bid);
static void euchre_set_player_message(player_t p);
static void euchre_end_trick();
static void euchre_end_hand();

struct game_function_pointers euchre_funcs = {
	euchre_is_valid_game,
	euchre_init_game,
	game_get_options,
	game_handle_option,
	game_get_option_text,
	euchre_set_player_message,
	euchre_get_bid_text,
	euchre_start_bidding,
	euchre_get_bid,
	euchre_handle_bid,
	euchre_next_bid,
	euchre_start_playing,
	game_verify_play,
	game_next_play,
	game_get_play,
	game_handle_play,
	euchre_deal_hand,
	euchre_end_trick,
	euchre_end_hand,
	game_start_game,
	game_test_for_gameover,
	game_handle_gameover,
	euchre_map_card,
	game_compare_cards,
	euchre_send_hand
};


static int euchre_is_valid_game()
{
	return (game.num_players == 4);
}

static card_t euchre_map_card(card_t c)
{
	if (c.face == JACK && c.suit == 3-game.trump) {
		c.suit = game.trump;
		c.face = ACE_HIGH+1;
	} else if (c.face == JACK && c.suit == game.trump)
		c.face = ACE_HIGH+2;
	return c;
}

static void euchre_init_game()
{
	player_t p;
	seat_t s;

	game.specific = alloc(sizeof(euchre_game_t));
	set_num_seats(4);
	for(p = 0; p < game.num_players; p++) {
		s = p;
		game.players[p].seat = s;
		game.seats[s].ggz = &ggz_seats[p];
	}
	game.deck_type = GGZ_DECK_EUCHRE;
	game.max_hand_length = 5;
	game.target_score = 10;
	EUCHRE.maker = -1;
	game.trump = -1;
}

static void euchre_start_bidding()
{
	game.bid_total = 8; /* twice around, at most */
	game.next_bid = (game.dealer + 1) % game.num_players;
}

static int euchre_get_bid()
{
	if (game.bid_count < 4) {
		/* first four bids: either "pass" or "take" */
		add_sbid(0, 0, EUCHRE_TAKE);
		add_sbid(0, 0, EUCHRE_PASS);
		return req_bid(game.next_bid);
	} else {
		char suit;
		for (suit=0; suit<4; suit++)
			add_sbid(0, suit, EUCHRE_TAKE_SUIT);
		add_sbid(0, 0, EUCHRE_PASS);
		return req_bid(game.next_bid);
	}
	/* TODO: dealer's last bid */
}

static void euchre_handle_bid(bid_t bid)
{
	if ( bid.sbid.spec == EUCHRE_TAKE ) {
		EUCHRE.maker = game.next_bid;
		game.trump = EUCHRE.up_card.suit;
	} else if ( bid.sbid.spec == EUCHRE_TAKE_SUIT ) {
		EUCHRE.maker = game.next_bid;
		game.trump = bid.sbid.suit;
	}
	/* bidding is ended automatically by game_next_bid */
}

static void euchre_next_bid()
{
	if (EUCHRE.maker >= 0)
		game.bid_total = game.bid_count;
	else if (game.bid_count == 8) {
		set_global_message("", "s", "Everyone passed; redealing.");
		set_game_state( WH_STATE_NEXT_HAND );
	} else
		game_next_bid();
}

static void euchre_start_playing()
{
	player_t p;
	seat_t s;

	game_start_playing();

	/* maker is set in game_handle_bid */
	set_global_message("", "%s is the maker in %s.", ggz_seats[EUCHRE.maker].name, suit_names[(int)game.trump]);
	game.leader = (game.dealer + 1) % game.num_players;
	/* resort/resend hand - this should probably be a function in itself... */
	for(s=0; s<game.num_seats; s++) {
		cards_sort_hand( &game.seats[ s ].hand );
		for (p=0; p<game.num_players; p++)
			game.funcs->send_hand(p, s);
	}
}

static int euchre_deal_hand()
{
	seat_t s;

	/* in Euchre, players 0-3 (seats 0, 1, 3, 4) get 5 cards each.
	 * the up-card (seat 5) gets one card, and the kitty (seat 2)
	 * gets the other 3. */
	cards_deal_hand(1, &game.seats[0].hand);
	EUCHRE.up_card = game.seats[0].hand.cards[0];
	set_global_message("", "The up-card is the %s of %s.",
		face_names[(int)EUCHRE.up_card.face], suit_names[(int)EUCHRE.up_card.suit]);
	game.hand_size = 5;

	/* in a regular deal, we just deal out hand_size cards to everyone */
	for (s = 0; s < game.num_seats; s++)
		cards_deal_hand(game.hand_size, &game.seats[s].hand);
	return 0;
}

static int euchre_send_hand(player_t p, seat_t s)
{
	/* reveal the up-card */
	return send_hand(p, s, game.players[p].seat == s || s == 5);
}

static int euchre_get_bid_text(char* buf, int buf_len, bid_t bid)
{
	if (bid.sbid.spec == EUCHRE_PASS) return snprintf(buf, buf_len, "Pass");
	if (bid.sbid.spec == EUCHRE_TAKE) return snprintf(buf, buf_len, "Take");
	if (bid.sbid.spec == EUCHRE_TAKE_SUIT) return snprintf(buf, buf_len, "Take at %s", suit_names[(int)bid.sbid.suit]);
	return snprintf(buf, buf_len, "%s", "");
}

static void euchre_set_player_message(player_t p)
{
	seat_t s = game.players[p].seat;

	clear_player_message(s);
	add_player_score_message(p);
	if (game.state == WH_STATE_FIRST_BID || game.state == WH_STATE_NEXT_BID || game.state == WH_STATE_WAIT_FOR_BID) {
		if (p == game.dealer)
			add_player_message(s, "dealer\n");
	} else
		if (p == EUCHRE.maker)
			add_player_message(s, "maker\n");
	if (game.state == WH_STATE_WAIT_FOR_PLAY || game.state == WH_STATE_NEXT_TRICK || game.state == WH_STATE_NEXT_PLAY)
		add_player_message(s, "Tricks: %d\n", game.players[p].tricks + game.players[(p+2)%4].tricks);
	add_player_action_message(p);
}

static void euchre_end_trick()
{
	game_end_trick();

	/* update teammate's info as well */
	set_player_message((game.winner+2)%4);
}

static void euchre_end_hand()
{
	int tricks, winning_team;
	tricks = game.players[EUCHRE.maker].tricks + game.players[(EUCHRE.maker+2)%4].tricks;
	if (tricks >= 3)
		winning_team = EUCHRE.maker % 2;
	else
		winning_team = (EUCHRE.maker + 1) % 2;
	/* TODO: point values other than 1 */
	game.players[winning_team].score += 1;
	game.players[winning_team+2].score += 1;
	EUCHRE.maker = -1;
	game.trump = -1;
}
