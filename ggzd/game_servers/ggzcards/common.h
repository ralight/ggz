/* 
 * File: common.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: Functions and data common to all games
 * $Id: common.h 6892 2005-01-25 04:09:21Z jdorje $
 *
 * This file contains code that controls the flow of a general
 * trick-taking game.  Game states, event handling, etc. are all
 * handled within this code.  For specialty handling we call a
 * game-specific function from game.c or games/<game>.c.
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-130
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <assert.h>

#include <ggz.h>		/* libggz */
#include "ggzdmod.h"

#include "ai.h"
#include "deck.h"
#include "games.h"
#include "protocol.h"
#include "types.h"

/* Debugging/logging types. */
#define DBG_BID "bid"		/* bid system messages */
#define DBG_PLAY "play"		/* play system messages */
#define DBG_AI "ai"		/* AI messages */
#define DBG_GAME "game"		/* game-specific messages */
#define DBG_NET "net"		/* net-related messages */
#define DBG_CLIENT "client"	/* client problems */
#define DBG_MISC "misc"		/* everything else... */

/* GGZCards server game states */
typedef enum {
	STATE_PRELAUNCH,	/**< before the launch happens */
	STATE_NOTPLAYING,	/**< no game started */
	STATE_NEXT_HAND,	/**< ready to create a new hand */
	STATE_WAIT_FOR_BID,	/**< waiting for a player to respond to a bid */
	STATE_WAIT_FOR_PLAY,	/**< waiting for a player to respond to a play */
	STATE_FIRST_BID,	/**< about to have the first bid */
	STATE_NEXT_BID,		/**< asking for new bid */
	STATE_FIRST_TRICK,	/**< about to have the first trick of a hand */
	STATE_NEXT_TRICK,	/**< time for the next trick */
	STATE_NEXT_PLAY,	/**< asking for a new play */
} server_state_t;

/* Data structure for generic trick-taking card game */
typedef struct {
	/* GGZ data state object */
	GGZdMod *ggz;

	/* Game meta-data */
	game_data_t *data;	/**< Game-specific data */
	ai_module_t *ai_module;	/**< the type of AI we're using */

	deck_type_t deck_type;	/**< the type of deck used */
	deck_t *deck;		/**< the deck being used */

	/* misc. state data */
	bool initted;		/**< has the game been initialized? */
	player_t host;		/**< the host of the table; cannot be an AI */

	/* state data */
	server_state_t state;	/**< the current state of the game */

	/* these next few are general game-specific options that are used by
	   the game-independent code */
	bool must_overtrump;	/**< if this is set, then a player must trump/overtrump if possible (i.e. La Pocha) */
	bool must_break_trump;	/**< if this is set, then trump can't be lead until it's been broken (i.e. Spades) */
#define MAX_TARGET_SCORE 1000000
	int target_score;	/**< after someone reaches this score, the game is over (0=unused) */
	bool last_trick;	/**< should the last trick be sent to all the players? */
	bool last_hand;		/**< should the last hand be sent to all the players? */
	bool cumulative_scores;	/**< should the cumulative score be sent to all the players? */
	bool bid_history;	/**< should a complete history of the hand's bidding be shown? */
	bool rated;		/**< should we track player stats? */

	/* Table options */
	bool open_hands;	/**< are we playing with open hands? */
	bool blind_spectators;	/**< are hands hidden from spectators? */

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

	/* State-tracking data: tricks */
	player_t winner;	/**< who won last trick */
	player_t leader;	/**< who leads next trick/lead this trick */
	int trick_count;	/**< how many tricks have been played this hand */
	int trick_total;	/**< how many tricks there will be this hand */
	bool trump_broken;	/**< has trump been broken this hand?  See must_break_trump, above. */

	/* table data: players */
	int num_players;	/**< the number of players in the game */
	int player_count;	/**< the number of human players who have joined, in total */
	struct game_player_t *players;	/**< data for each player, allocated in game_init */

	/* table data: seats */
	int num_seats;	/**< the number of "seats" in the table (which includes fake non-players */
	struct game_seat_t *seats;	/**< data for each seat, allocated in game_init */
	
	/* table data: teams */
	int num_teams;

	/* Global messages */
	global_message_list_t *message_head;	/**< global message list head */
	global_message_list_t *message_tail;	/**< global message list tail */

	/* Game-specific data */
	void *specific;		/**< any extra game-specific data */

} game_t;


/* State manipulation functions */
const char *get_state_name(server_state_t state);
void set_game_state(server_state_t state);

/* these are GGZ communication events that we must handle */
void handle_ggz_state_event(GGZdMod * ggz,
			    GGZdModEvent event, const void *data);
void handle_ggz_seat_event(GGZdMod *ggz, 
			   GGZdModEvent event, const void *data);
void handle_ggz_spectator_seat_event(GGZdMod *ggz,
				     GGZdModEvent event, const void *data);
void handle_ggz_player_data_event(GGZdMod * ggz,
				  GGZdModEvent event, const void *data);
void handle_ggz_spectator_data_event(GGZdMod *ggz,
				     GGZdModEvent event, const void *data);

/* Handle a player/spectator data event. */
void handle_player_data_event(player_t p);

/* these are internal GGZCards events */
void handle_newgame_event(player_t p);
void handle_trick_event(player_t winner);
void handle_gameover_event(int winner_cnt, player_t * winners);
void handle_neterror_event(player_t p);

/* Handlers for general client messages.  Other handlers may be in
   their specific files. */
void handle_client_language(player_t p, const char* lang);
void handle_client_newgame(player_t p);
void handle_client_sync(player_t p);

/* General high-level functions */
void init_ggzcards(GGZdMod * ggz, game_data_t *game_data);
bool try_to_start_game(void);
void init_game(void);
void next_move(void);		/* make the next move, if all seats are full */
void send_sync(player_t p);
void broadcast_sync(void);
void send_hand(const player_t p, const seat_t s,
               bool show_fronts, bool show_backs);

/* Seat manipulation functions */
void set_num_seats(int num_seats);
void assign_seat(seat_t s, player_t p);	/* player #p sits in seat #s */
void empty_seat(seat_t s, char *name);	/* seat s is empty; give it a label */

/* the game structure */
extern game_t game;


/* accessor functions */
/* I made these when switching over to a new ggzdmod.  I don't think it's
   safe to access the data directly. */
const char *get_seat_name(seat_t s);
GGZSeatType get_seat_status(seat_t s);

const char* get_player_name(player_t p);
GGZSeatType get_player_status(player_t p);
int get_player_socket(int p);

bool seats_full(void);
bool seats_empty(void);

#define SPECTATOR_TO_PLAYER(spectator) (-1 - (spectator))
#define PLAYER_TO_SPECTATOR(player) (-1 - (player))

#define PLAYER_TO_SEAT(p) \
	((p) >= 0 && (p) < game.num_players ? game.players[p].seat : -1)
#define SEAT_TO_PLAYER(s) \
	(assert((s) >= 0 && (s) < game.num_seats), game.seats[s].player)

/* Support functions.  Should go into a different file. */
void fatal_error(const char *message);

/* Preliminary i18n macros */
#define N_(string) (string)

#define allplayers_iterate(p)                               \
{                                                           \
	player_t p;                                         \
	for (p = -ggzdmod_get_max_num_spectators(game.ggz); \
	     p < game.num_players;                          \
	     p++)

#define allplayers_iterate_end }

#define players_iterate(p)                                  \
{                                                           \
	player_t p;                                         \
		for (p = 0; p < game.num_players; p++)

#define players_iterate_end }

#define IS_REAL_PLAYER(p) ((p) >= 0)

#define IS_SPECTATOR(p) ((p) < 0)

#endif /* __COMMON_H__ */

