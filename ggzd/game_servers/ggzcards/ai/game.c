/*
 * File: ai/game.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 02/10/2002
 * Desc: Client-callback routines for the AI functions
 * $Id: game.c 3426 2002-02-20 03:51:03Z jdorje $
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

#include <assert.h>
#include <stdlib.h>

#include <ggz.h>

#include "aicommon.h"

#include "game.h"

void game_get_newgame(void)
{
	client_send_newgame();
}

void game_alert_newgame(void)
{
	/* nothing */	
}

void game_alert_newhand(void)
{
	ailib_start_hand();
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

void game_get_bid(int possible_bids, bid_t *bid_choices, char **bid_descriptions)
{
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
	ailib_alert_bid(bidder, bid);
	alert_bid(bidder, bid);
}

static int *valid_plays = NULL;
static card_t play_card;

void game_get_play(int hand)
{
	int i;
	
	ailib_our_play(hand);

	valid_plays = ggz_realloc(valid_plays, ggzcards.players[hand].hand.hand_size * sizeof(*valid_plays));
	for (i = 0; i < ggzcards.players[hand].hand.hand_size; i++)
		valid_plays[i] = 1;
	
	play_card = get_play(ggzcards.play_hand, valid_plays);
	client_send_play(play_card);
}

void game_alert_badplay(char *err_msg)
{
	int i, hand = ggzcards.play_hand;
	
	for (i = 0; i < ggzcards.players[hand].hand.hand_size; i++)
		if (are_cards_equal(ggzcards.players[hand].hand.card[i], play_card)) {
			valid_plays[i] = 0;
			break;
		}
	
	play_card = get_play(ggzcards.play_hand, valid_plays);
	client_send_play(play_card);
}

void game_alert_play(int player, card_t card, int pos)
{
	ailib_alert_play(player, card);
	alert_play(player, card);
}

void game_alert_table(void)
{
	/* nothing */
}

void game_alert_trick(int player)
{
	ailib_alert_trick(player);
}

int game_get_options(int option_cnt, int *choice_cnt, int *defaults,
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

int game_handle_game_message(int fd, int game_num, int size)
{
	/* nothing */
	return 0;
}
