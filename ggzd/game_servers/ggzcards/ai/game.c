/*
 * File: ai/game.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 02/10/2002
 * Desc: Client-callback routines for the AI functions
 * $Id: game.c 4067 2002-04-23 21:39:56Z jdorje $
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

#include <ggz.h>

#include "game.h"

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

void game_alert_player(int player, GGZSeatType status,
			      const char *name)
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
	
	for (i = 0; i < possible_bids; i++) {
		if (bid.bid == bid_choices[i].bid) {
			client_send_bid(i);
			return;
		}
	}
	
	assert(0);
	client_send_bid(random() % possible_bids);	
}

void game_alert_bid(int bidder, bid_t bid)
{
	alert_bid(bidder, bid);
}

static bool *valid_plays = NULL;
static card_t play_card;
static int num_valid_plays = 0;

static int find_card(hand_t *hand, card_t card)
{
	int i;

	for (i = 0; i < hand->hand_size; i++)
		if (are_cards_equal(hand->cards[i], card))
			return i;
	assert(0);
	return -1;
}

static void make_play(card_t play)
{
	hand_t *hand = &ggzcards.players[ggzcards.play_hand].hand;
	int num = find_card(hand, play);
	
	play_card = play;
	
	if (num < 0 || !valid_plays[num]) {
		assert(FALSE);
		client_send_sync_request();	
	} else
		client_send_play(play);
}

void game_get_play(int hand)
{
	int i;
	
	assert(hand == ggzcards.play_hand);

	num_valid_plays = ggzcards.players[hand].hand.hand_size;
	valid_plays = ggz_realloc(valid_plays,
	                          num_valid_plays * sizeof(*valid_plays));
	for (i = 0; i < num_valid_plays; i++)
		valid_plays[i] = TRUE;
	
	make_play(get_play(ggzcards.play_hand, valid_plays));
}

void game_alert_badplay(char *err_msg)
{
	int invalid = find_card(&ggzcards.players[ggzcards.play_hand].hand,
	                        play_card);
			
	assert(invalid >= 0);
	valid_plays[invalid] = FALSE;
	num_valid_plays--;
	
	if (invalid < 0 || num_valid_plays <= 0) {
		assert(FALSE);
		client_send_sync_request();
	} else
		make_play(get_play(ggzcards.play_hand, valid_plays));
}

void game_alert_play(int player, card_t card, int pos)
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
}

int game_get_options(int option_cnt,
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

int game_handle_game_message(int fd, const char *game, int size)
{
	/* nothing */
	return 0;
}
