/*
 * File: ai/game.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 02/10/2002
 * Desc: Client-callback routines for the AI functions
 * $Id: game.c 8427 2006-07-31 22:50:50Z jdorje $
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
#include <stdlib.h>
#include <unistd.h> /* sleep() */

#include <ggz.h>

#include "game.h"

static int num_human_players(void)
{
	int p, num = 0;
	for (p = 0; p < ggzcards.num_players; p++)
		if (ggzcards.players[p].status == GGZ_SEAT_PLAYER)
			num++;
	return num;
}

void game_alert_server(GGZDataIO *server_dio)
{
	/* nothing */
}

void game_get_newgame(void)
{
	client_send_newgame();
}

void game_alert_newgame(cardset_type_t cardset_type)
{
	/* nothing */
}

void game_alert_newhand(void)
{
	start_hand();
}

void game_handle_gameover(int num_winners, int *winners)
{
	/* nothing */
}

void game_alert_player(int player, GGZSeatType status, const char *name)
{
	/* nothing */
}

void game_alert_num_players(int new, int old)
{
	/* nothing */
}

void game_alert_hand_size(int max_hand_size)
{
	/* nothing */
}

void game_display_hand(int player)
{
	/* nothing */
}

void game_get_bid(int possible_bids,
                  bid_t *bid_choices,
                  char **bid_texts,
                  char **bid_descs)
{
	/* We ignore bid_texts and bid_descs */
	bid_t bid = get_bid(bid_choices, possible_bids);
	int i;

	ggz_debug(DBG_BID, "Making bid %d (%d/%d/%d).",
	          bid.bid, bid.sbid.val,
	          bid.sbid.suit, bid.sbid.spec);
	
	for (i = 0; i < possible_bids; i++) {
		if (bid.bid == bid_choices[i].bid) {
			client_send_bid(i);
			return;
		}
	}

	/* This can happen when some variant is enabled that the AI doesn't
	   support.  The best choice is to find the closest matching bid. */
	ggz_debug(DBG_BID, "Uh oh; invalid bid!");
	for (i = 0; i < possible_bids; i++) {
		if (bid.sbid.spec == bid_choices[i].sbid.spec) {
			client_send_bid(i);
			return;
		}
	}
	client_send_bid(random() % possible_bids);	
}

void game_alert_bid(int bidder, bid_t bid)
{
	alert_bid(bidder, bid);
}

static int find_card(hand_t *hand, card_t card)
{
	int i;

	for (i = 0; i < hand->hand_size; i++)
		if (are_cards_equal(hand->cards[i], card))
			return i;
	assert(FALSE);
	return -1;
}

static void make_play(card_t play)
{
	hand_t *hand = &ggzcards.players[ggzcards.play_hand].hand;
	int num = find_card(hand, play);
	
	if (num < 0) {
		assert(FALSE);
		client_send_sync_request();	
	} else
		client_send_play(play);
}

void game_get_play(int play_hand, int num_valid_cards, card_t *valid_cards)
{
	int play_num, hand_num;
	hand_t *hand = &ggzcards.players[play_hand].hand;
	bool valid_plays[hand->hand_size];
	card_t play;
	
	assert(play_hand == ggzcards.play_hand);

	for (hand_num = 0; hand_num < hand->hand_size; hand_num++)
		valid_plays[hand_num] = FALSE;
	for (play_num = 0; play_num < num_valid_cards; play_num++) {
		card_t play_card = valid_cards[play_num];
		for (hand_num = 0; hand_num < hand->hand_size; hand_num++) {
			card_t hand_card = hand->cards[hand_num];
			if (are_cards_equal(play_card, hand_card)) {
				valid_plays[hand_num] = TRUE;
				break;
			}
		}
		assert(hand_num < hand->hand_size);
	}

	play = get_play(ggzcards.play_hand, valid_plays);

	ggz_debug(DBG_PLAY, "We're playing the %s of %s.",
	          get_face_name(play.face),
	          get_suit_name(play.suit));
	
	make_play(play);
}

void game_alert_badplay(char *err_msg)
{
	assert(FALSE);
	client_send_sync_request();
}

void game_alert_play(int player, card_t card, int pos, int hand_pos)
{
	alert_play(player, card);
}

void game_alert_table(void)
{
	/* nothing */
}

void game_alert_trick(int winner)
{
	alert_trick(winner);

	/* If there are no human players at the table, we simulate
	   a delay - both to allow any spectators to keep up and to
	   prevent the game server/bots from hogging resources. */
	if (num_human_players() == 0)
		sleep(2);
}

int game_get_options(int option_cnt,
		     char **types,
                     char **descriptions,
                     int *choice_cnt,
                     int *defaults,
                     char ***option_choices)
{
	/* nothing */
	return -1;
}

void game_set_text_message(const char *mark, const char *msg)
{
	/* nothing */
}

void game_set_cardlist_message(const char *mark, int *lengths,
				      card_t ** cardlist)
{
	/* nothing */
}

void game_set_player_message(int player, const char *msg)
{
	/* nothing */
}

void game_handle_game_message(GGZDataIO *dio, const char *game)
{
	/* nothing */
}
