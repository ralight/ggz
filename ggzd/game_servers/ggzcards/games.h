/* 
 * File: games.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: multi-game code
 * $Id: games.h 2766 2001-11-28 07:31:42Z jdorje $
 *
 * This file contains the data and functions that allow the game type to
 * be picked and the right functions for that game to be set up.  It's
 * pretty messy.
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

#ifndef __GAMES_H__
#define __GAMES_H__

#include <config.h>		/* Site-specific config */

#include "types.h"

struct game_function_pointers {
	/* initializing */
	int (*is_valid_game) ();	/* is the game valid under the
					   current conditions? */
	void (*init_game) ();	/* initialize the game data */

	/* options */
	void (*get_options) ();	/* determine/request options from given
				   player fd */
	int (*handle_option) (char *, int);	/* handle an option => 0 on
						   success, -1 on failure */
	char *(*get_option_text) (char *, int, char *, int);	/* return a
								   descriptive 
								   string for 
								   the option 
								   chosen */

	/* messaging */
	void (*set_player_message) (player_t);	/* determine and send the
						   player message */

	/* bidding */
	int (*get_bid_text) (char *, size_t, bid_t);	/* determines the
							   textual string for 
							   the bid */
	void (*start_bidding) ();	/* updates data for the first bid */
	int (*get_bid) ();	/* gets a bid from next player */
	void (*handle_bid) (player_t, bid_t);	/* handles a bid from the
						   given bidder */
	void (*next_bid) ();	/* updates data for the next bid */

	/* playing */
	void (*start_playing) ();	/* updates data after the last
					   bid/before the playing starts */
	char *(*verify_play) (card_t);	/* verifies the play is legal */
	void (*next_play) ();	/* sets up for next play */
	void (*get_play) ();	/* retreives a play */
	void (*handle_play) (card_t);	/* handle a play */

	/* each hand */
	int (*deal_hand) ();	/* deal next hand */
	void (*end_trick) ();	/* end-of-trick calculations */
	void (*end_hand) ();	/* end-of-hand calculations */

	/* starting/ending games */
	void (*start_game) ();	/* start a game */
	int (*test_for_gameover) ();	/* returns TRUE iff gameover */
	int (*handle_gameover) ();	/* handle a gameover */

	/* miscellaneous */
	  card_t(*map_card) (card_t);
	int (*compare_cards) (card_t, card_t);
	int (*send_hand) (player_t, seat_t);	/* sends a hand to a player */
};

struct game_info {
	char *name;
	char *full_name;
	struct game_function_pointers *funcs;
};

/* Game types. */
/* The game enumerations must start at 0 and count upwards; they also must
   match with game_names, below */
typedef enum game_type_t {
	GGZ_GAME_UNKNOWN = -1,
	GGZ_GAME_SUARO = 0,	/* http://suaro.dhs.org */
	GGZ_GAME_SPADES = 1,
	GGZ_GAME_HEARTS = 2,
	GGZ_GAME_BRIDGE = 3,
	GGZ_GAME_LAPOCHA = 4,
	GGZ_GAME_EUCHRE = 5,
	GGZ_GAME_ROOK = 6,
	GGZ_GAME_SKAT = 7,
	GGZ_GAME_SUECA = 8
} game_type_t;

extern struct game_info game_data[];

int games_get_gametype(char *);	/* which game is this? */
void games_handle_gametype(int);	/* set the game */
int games_req_gametype();	/* what do you want to play today? */
int games_valid_game(int);	/* is the game valid? */

#endif /* __GAMES_H__ */
