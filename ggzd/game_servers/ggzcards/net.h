/* 
 * File: net.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: Game-independent game network functions
 * $Id: net.h 3421 2002-02-19 10:59:53Z jdorje $
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

/* Functions to send packets to the client. */

void broadcast_player_list(void);

void broadcast_play(seat_t player, card_t card);

int send_gameover(int winner_cnt, player_t * winners);

int send_table(player_t p);

int send_sync(player_t p);
void broadcast_sync(void);

int send_bid_request(player_t p, int bid_count, bid_t * bids);

void broadcast_bid(player_t bidder, bid_t bid);

int send_play_request(player_t p, seat_t s);

int send_badplay(player_t p, char *msg);

int send_hand(player_t p, seat_t s, int reveal);

int send_trick(player_t winner);

int send_newgame_request(player_t p);

/* Send a newgame to one person. */
int send_newgame(player_t p);
void broadcast_newgame(void);

int send_global_text_message(player_t p, const char *mark,
			      const char *message);
void broadcast_global_text_message(const char *mark, const char* message);

int send_global_cardlist_message(player_t p, const char *mark, int *lengths,
				  card_t ** cardlist);
void broadcast_global_cardlist_message(const char *mark, int *lengths,
				       card_t ** cardlist);

/* Functions to receive packets from the client. */

int rec_play(player_t p);
