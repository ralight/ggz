/*
 * File: game.h
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Server
 * Date: 06/29/2000
 * Desc: default game functions
 * $Id: game.h 2189 2001-08-23 07:59:17Z jdorje $
 *
 * This file was originally taken from La Pocha by Rich Gade.  It now
 * contains the default game functions; that is, the set of game functions
 * that may be used as defaults.  Each game (in games/) will define its
 * own functions as well.
 *
 * Copyright (C) 2000 Brent Hendricks.
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

#include <stdio.h>		/* just for debugging */

#include "types.h"

/* Game-specific functions */

extern struct game_function_pointers game_funcs;


extern int game_is_valid_game();
extern void game_init_game();

extern void game_get_options();
extern int game_handle_option(char *, int);
extern char *game_get_option_text(char *, int, char *, int);

extern void game_set_player_message(player_t);

extern int game_get_bid_text(char *, int, bid_t);
extern void game_start_bidding();
extern int game_get_bid();
extern void game_handle_bid(bid_t);
extern void game_next_bid();

extern void game_start_playing();
extern char *game_verify_play(card_t);
extern void game_next_play();
extern void game_get_play(player_t);
extern void game_handle_play(card_t);

extern int game_deal_hand(void);
extern void game_end_trick(void);
extern void game_end_hand(void);

extern void game_start_game();
extern int game_test_for_gameover();
extern int game_handle_gameover();

extern card_t game_map_card(card_t);
extern int game_compare_cards(card_t, card_t);
extern int game_send_hand(player_t, seat_t);
