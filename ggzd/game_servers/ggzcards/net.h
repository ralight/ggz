/* 
 * File: net.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: Game-independent game network functions
 * $Id: net.h 8465 2006-08-03 07:29:12Z jdorje $
 *
 * This file contains code that controls the flow of a general
 * trick-taking game.  Game states, event handling, etc. are all
 * handled within this code.  For specialty handling we call a
 * game-specific function from game.c or games/<game>.c.
 *
 * Copyright (C) 2001-2002 GGZ Development Team
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

#include "types.h"

/** convert an "absolute" seat number s to the
 * "relative" seat number used by player p */
#define CONVERT_SEAT(s, p) (convert_seat(s, p))
seat_t convert_seat(seat_t s_abs, player_t p);
#define UNCONVERT_SEAT(s, p) (unconvert_seat(s, p))
seat_t unconvert_seat(seat_t s_rel, player_t p);

/* Functions to send packets to the client. */

void net_send_player_list(player_t p);
void net_broadcast_player_list(void);

void net_send_scores(player_t p, int hand_num);
void net_send_all_scores(player_t p);
void net_broadcast_scores(int hand_num);

void net_send_options_request(player_t p,
                              int num_options,
			      char **option_types,
                              char **option_descs,
                              int *num_choices,
                              int *option_defaults,
                              char ***option_choices);

void net_broadcast_play(seat_t player, card_t card);

void net_broadcast_gameover(int winner_cnt, player_t * winners);

void net_send_table(player_t p);

void net_send_bid_request(player_t p, int bid_count, bid_t * bids);

void net_send_bid(player_t p, player_t bidder, bid_t bid);
void net_broadcast_bid(player_t bidder, bid_t bid);

void net_send_play_request(player_t p, seat_t s);

void net_send_badplay(player_t p, char *msg);

void net_send_hand(player_t p, seat_t s,
                   bool show_fronts, bool show_backs);
void net_broadcast_hands(void);

void net_broadcast_trick(player_t winner);

void net_send_newgame_request(player_t p);

/* Send a newgame to one person. */
void net_send_newgame(player_t p);
void net_broadcast_newgame(void);

void net_broadcast_newhand(void);

void net_send_global_text_message(player_t p, const char *mark,
			      const char *message);
void net_broadcast_global_text_message(const char *mark, const char* message);

void net_send_player_text_message(player_t p, seat_t s, const char *message);

void net_send_global_cardlist_message(player_t p, const char *mark, int *lengths,
				  card_t ** cardlist);
void net_broadcast_global_cardlist_message(const char *mark, int *lengths,
				       card_t ** cardlist);

/* Functions to receive packets from the client. */
void net_read_player_data(player_t p);
