/* 
 * File: common.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: Functions and data common to all games
 * $Id: common.h 2969 2001-12-20 18:49:46Z jdorje $
 *
 * This file contains code that controls the flow of a general
 * trick-taking game.  Game states, event handling, etc. are all
 * handled within this code.  For specialty handling we call a
 * game-specific function from game.c or games/<game>.c.
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-130
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <config.h>		/* Site-specific config */

#include <assert.h>

#include <ggz.h>		/* libggz */
#include "ggzdmod.h"

#include "ai.h"
#include "bid.h"
#include "cards.h"
#include "game.h"
#include "games.h"
#include "message.h"
#include "net.h"
#include "options.h"
#include "protocol.h"
#include "types.h"

/* If defined, we'll use GGZ's stat tracking
   features.  These aren't fully implemented
   yet, so it's probably best not to use it. */
/* #define USE_GGZ_STATS */

/* GGZCards server game states */
typedef enum {
	STATE_NONE,		/**< no state is set; we'd better figure one out! */
	STATE_PRELAUNCH,	/**< before the launch happens */
	STATE_NOTPLAYING,	/**< no game started */
	STATE_WAITFORPLAYERS,	/**< waiting for players; saved state */
	STATE_NEXT_HAND,	/**< ready to create a new hand */
	STATE_FIRST_BID,	/**< about to have the first bid */
	STATE_NEXT_BID,	/**< asking for new bid */
	STATE_FIRST_TRICK,	/**< about to have the first trick of a hand */
	STATE_NEXT_TRICK,	/**< time for the next trick */
	STATE_NEXT_PLAY,	/**< asking for a new play */
	STATE_WAIT_FOR_PLAY	/**< waiting for a play */
} server_state_t;

/* Data structure for generic trick-taking card game */
struct game_t {
	/* GGZ data state object */
	GGZdMod *ggz;

	/* Game meta-data */
	game_type_t which_game;	/**< the type of game game */
	struct game_function_pointers *funcs;	/**< an array of game-specific functions */
	ai_type_t ai_type;	/**< the type of AI we're using */

	deck_type_t deck_type;	/**< the type of deck used */
	deck_t *deck;		/**< the deck being used */

	/* misc. game data */
	char *name;		/**< the name of the game */
	char *rules_url;	/**< the URL of where to read the game's rules */

	/* misc. state data */
	int initted;		/**< has the game been initialized? */
	player_t host;		/**< the host of the table; cannot be an AI */

	/* state data */
	server_state_t state;	/**< the current state of the game */
	server_state_t saved_state;	/**< any time while waiting, the state we _would_ be in if we weren't waiting */

	/* these next few are general game-specific options that are used by
	   the game-independent code */
	int must_overtrump;	/**< if this is set, then a player must trump/overtrump if possible (i.e. La Pocha) */
	int must_break_trump;	/**< if this is set, then trump can't be lead until it's been broken (i.e. Spades) */
	int target_score;	/**< after someone reaches this score, the game is over (0=unused) */
	int last_trick;		/**< should the last trick be sent to all the players? */
	int last_hand;		/**< should the last hand be sent to all the players? */
	int cumulative_scores;	/**< should the cumulative score be sent to all the players? */
	int bid_history;	/**< should a complete history of the hand's bidding be shown? */

	/* Table options */
	int open_hands;		/**< are we playing with open hands? */

	/* State-tracking data: dealing */
	int hand_num;		/**< the number of the current hand (counting
				   from ?) */
	int hand_size;		/**< the size of the hand */
	int max_hand_length;	/**< the largest possible hand */
	player_t dealer;	/**< who dealt this hand (starts at random,
				   then rotates) */

	/* State-tracking data: bidding */
	int bid_count;		/**< how many bids there have been this bidding
				   cycle */
	int bid_total;		/**< how many bids there will be this bidding
				   cycle */
	int bid_rounds;		/**< number of *rounds* of bidding */
	int max_bid_rounds;	/**< the maximum number of bid rounds we can
				   hold (increased at need) */
	player_t prev_bid;	/**< who made the most recent bid? */
	player_t next_bid;	/**< who is going to be bidding next? */

	/* State-tracking data: playing */
	card_t lead_card;	/**< the card that was lead this trick */
	char trump;		/**< the suit of trump; 0-3 or other for none */
	int play_count;		/**< how many plays there have been this trick */
	int play_total;		/**< how many plays there will be this trick */
	player_t next_play;	/**< current/next player */
	player_t curr_play;	/**< current player, tracked automatically by req_play */
	/* Note: the difference between these two is subtle, but important.
	   next_play is used to track the player whose hand is being played
	   from.  curr_play is used automatically to remember who is supposed 
	   to be playing now.  In the case of bridge, next_play will go
	   around the table (0, 1, 2, 3) for each play.  However, when the
	   dummy is playing curr_play will point to the declarer, since
	   they're the one we request the play from. */
	seat_t play_seat;	/**< the seat being played from */

	/* State-tracking data: tricks */
	player_t winner;	/**< who won last trick */
	player_t leader;	/**< who leads next trick/lead this trick */
	int trick_count;	/**< how many tricks have been played this hand */
	int trick_total;	/**< how many tricks there will be this hand */
	int trump_broken;	/**< has trump been broken this hand?  See must_break_trump, above. */

	/* table data: players */
	player_t num_players;	/**< the number of players in the game */
	int player_count;	/**< the number of human players who have joined, in total */
	struct game_player_t *players;	/**< data for each player, allocated in game_init */

	/* table data: seats */
	seat_t num_seats;	/**< the number of "seats" in the table (which includes fake non-players */
	struct game_seat_t *seats;	/**< data for each seat, allocated in game_init */

	/* Global messages */
	global_message_list_t *message_head;	/**< global message list head */
	global_message_list_t *message_tail;	/**< global message list tail */

	/* Game-specific data */
	void *specific;		/**< any extra game-specific data */

};

/** convert an "absolute" seat number s to the
 * "relative" seat number used by player p */
#define CONVERT_SEAT(s, p)					\
	(assert(game.players[(p)].seat >= 0),			\
	 ((s) - game.players[(p)].seat + game.num_seats) % game.num_seats)
#define UNCONVERT_SEAT(s_rel, p)				\
	(assert(game.players[(p)].seat >= 0),			\
	 (game.players[(p)].seat + (s_rel)) % game.num_seats)


/* Game-independent functions */
void set_game_state(server_state_t state);
void save_game_state();
void restore_game_state();

void init_game(void);


void next_play(void);		/* make the next move */

/* handle player events -- used to all just be "update" */

/* these are GGZ communication events that we must handle */
void handle_launch_event(GGZdMod * ggz, GGZdModEvent event, void *data);
void handle_join_event(GGZdMod * ggz, GGZdModEvent event, void *data);
void handle_leave_event(GGZdMod * ggz, GGZdModEvent event, void *data);
void handle_player_event(GGZdMod * ggz, GGZdModEvent event, void *data);

/* these are internal GGZCards events */
int handle_newgame_event(player_t p);
int handle_play_event(card_t card);
int handle_bid_event(player_t p, bid_t bid);

/* initialization functions */
void init_ggzcards(GGZdMod * ggz, int which);	/* pass in the name of the
						   game */
void set_num_seats(int num_seats);
void assign_seat(seat_t s, player_t p);	/* player #p sits in seat #s */
void empty_seat(seat_t s, char *name);	/* seat s is empty; give it a label */

/* the game structure */
extern struct game_t game;


/* accessor functions */
/* I made these when switching over to a new ggzdmod.  I don't think it's
   safe to access the data directly. */
const char *get_seat_name(seat_t s);
GGZSeatType get_seat_status(seat_t s);

#define get_player_name(p) (ggzdmod_get_seat(game.ggz, p).name)
#define get_player_status(p) (ggzdmod_get_seat(game.ggz, p).type)
#define get_player_socket(p) (ggzdmod_get_seat(game.ggz, p).fd)

/* Support functions.  Should go into a different file. */
void fatal_error(const char *message);

#endif /* __COMMON_H__ */
