/*
 * File: ai/game.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 02/10/2002
 * Desc: Client-callback routines for the AI functions
 * $Id: game.h 3426 2002-02-20 03:51:03Z jdorje $
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

#include "client.h"

void game_get_newgame(void);
void game_alert_newgame(void);
void game_alert_newhand(void);
void game_handle_gameover(int num_winners, int *winners);
void game_alert_player(int player, GGZSeatType status,
			      const char *name);
void game_alert_num_players(int new, int old);
void game_alert_hand_size(int max_hand_size);
void game_display_hand(int player);
void game_get_bid(int possible_bids, bid_t *bid_choices, char **bid_descriptions);
void game_alert_bid(int bidder, bid_t bid);
void game_get_play(int hand);
void game_alert_badplay(char *err_msg);
void game_alert_play(int player, card_t card, int pos);
void game_alert_table(void);
void game_alert_trick(int player);
int game_get_options(int option_cnt, int *choice_cnt, int *defaults,
			    char ***option_choices);
void game_set_text_message(const char *mark, const char *msg);
void game_set_cardlist_message(const char *mark, int *lengths,
				      card_t ** cardlist);
void game_set_player_message(int player, const char *msg);
int game_handle_game_message(int fd, int game_num, int size);



extern void start_hand(void);
extern void alert_bid(int p, bid_t bid);
extern void alert_play(int p, card_t card);
extern bid_t get_bid(bid_t * bid_choices, int bid_count);
extern card_t get_play(int play_hand, int *valid_plays);
