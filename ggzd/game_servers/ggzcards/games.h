/*
 * File: games.h
 * Author: Jason Short
 * Project: GGZCards game module
 * Date: 06/20/2001
 * Desc: Data and functions related to all games
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

#include "types.h"

struct game_function_pointers {
	/* initializing */
	void	(*init_game)();			/* initialize the game data */
	int	(*get_options)();		/* determine/request options */
	void	(*handle_options)();		/* handle options from player */

	/* messaging */
	void	(*set_player_message)(player_t); /* determine and send the player message */

	/* bidding */
	int	(*get_bid_text)(char*, int, bid_t);  /* determines the textual string for the bid */
	void	(*start_bidding)();		/* updates data for the first bid */
	int	(*get_bid)();			/* gets a bid from next player */
	int	(*handle_bid)(bid_t);		/* handles a bid from current bidder */
	void	(*next_bid)();			/* updates data for the next bid */

	/* playing */
	void	(*start_playing)();		/* updates data after the last bid/before the playing starts */
	char*	(*verify_play)(card_t);		/* verifies the play is legal */
	void	(*next_play)();			/* sets up for next play */
	void	(*get_play)();			/* retreives a play */
	void	(*handle_play)(card_t);		/* handle a play */

	/* each hand */
	int	(*deal_hand)();			/* deal next hand */
	void	(*end_trick)();			/* end-of-trick calculations */
	void	(*end_hand)();			 /* end-of-hand calculations */

	/* starting/ending games */
	void	(*start_game)();		/* start a game */
	int	(*test_for_gameover)();		/* returns TRUE iff gameover */
	int	(*handle_gameover)();		/* handle a gameover */ 	

	/* miscellaneous */
	card_t	(*map_card)(card_t);
	int	(*compare_cards)(const void *, const void *);
	int	(*send_hand)(player_t, seat_t);	 /* sends a hand to a player */
};

struct game_info {
	char* name;
	struct game_function_pointers *funcs;
};

/* Game types. */
/* NOTE: because of the way options are handled, these must start at 0 and
 * count up directly.   See games_req_gametype. */
/* They also must match with game_names, below */
typedef enum game_type_t {
	GGZ_GAME_UNKNOWN = -1,
	GGZ_GAME_SUARO = 0,		/* http://suaro.dhs.org */
	GGZ_GAME_SPADES = 1,
	GGZ_GAME_HEARTS = 2,
	GGZ_GAME_BRIDGE = 3,
	GGZ_GAME_LAPOCHA = 4,
	GGZ_GAME_EUCHRE = 5,
	GGZ_GAME_ROOK = 6,
	GGZ_GAME_SKAT = 7
} game_type_t;

/* this MUST correspond to the numbers above */
extern struct game_info game_data[];

#define GGZ_NUM_GAMES		8	/* the _number_ of valid games */

extern int game_types[];

extern int games_get_gametype(char*);	/* which game is this? */
extern int games_req_gametype();	/* what do you want to play today? */
extern int games_valid_game(int);	/* is the game valid? */

#endif /* __GAMES_H__ */
