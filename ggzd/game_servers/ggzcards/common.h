/*
 * File: common.h
 * Author: Rich Gade, Jason Short
 * Project: GGZCards GGZ Server
 * Date: 06/20/2001
 * Desc: Functions and data common to all games
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

/* this will be replaced by ../libggzmod/whatever to use libggzdmod */
#include "../libggzmod/ggz_server.h"
#include "../libggzmod/ggz_protocols.h"

#include "ai.h"
#include "bid.h"
#include "cards.h"
#include "game.h"
#include "games.h"
#include "message.h"
#include "options.h"
#include "protocol.h"
#include "types.h"

#ifndef __COMMON_H__
#define __COMMON_H__

/* GGZCards server game states */
typedef enum
{
	WH_STATE_PRELAUNCH,	/* before the launch happens */
	WH_STATE_NOTPLAYING,	/* no game started */
	WH_STATE_WAITFORPLAYERS,	/* waiting for players */
	WH_STATE_NEXT_HAND,	/* creating a new hand */
	WH_STATE_FIRST_BID,	/* about to have the first bid */
	WH_STATE_NEXT_BID,	/* asking for new bid */
	WH_STATE_WAIT_FOR_BID,	/* waiting for a bid */
	WH_STATE_FIRST_TRICK,	/* about to have the first trick of a hand */
	WH_STATE_NEXT_TRICK,	/* time for the next trick */
	WH_STATE_NEXT_PLAY,	/* asking for a new play */
	WH_STATE_WAIT_FOR_PLAY	/* waiting for a play */
}
server_state_t;

/* Data structure for generic trick-taking card game */
struct wh_game_t
{
	game_type_t which_game;	/* the game; currently defined in cards.h */
	struct game_function_pointers *funcs;	/* game-specific functions */
	ai_type_t ai_type;	/* the type of AI we're using */
	deck_type_t deck_type;	/* the type of deck used, as defined in cards.h */
	char *name;		/* the name of the game */
	char *rules_url;	/* the URL of where to read the game's rules */

	int initted;		/* has the game been initialized? */
	player_t host;		/* the host of the table; cannot be an AI */

	server_state_t state;	/* the current state of the game (see WH_STATE, above) */
	server_state_t saved_state;	/* any time while waiting, the state we _would_ be in if we weren't waiting */

	/* these next few are general game-specific options that are used by the game-independent code */
	int must_overtrump;	/* if this is set, then a player must trump/overtrump if possible (i.e. La Pocha) */
	int must_break_trump;	/* if this is set, then trump can't be lead until it's been broken (i.e. spades) */
	int target_score;	/* after someone reaches this score, the game is over (if used) */
	int last_trick;		/* should the last trick be sent to all the players? */
	int last_hand;		/* should the last hand be sent to all the players? */
	int cumulative_scores;	/* should the cumulative score be sent to all the players? */
	int bid_history;	/* should a complete history of the hand's bidding be shown? */

	int open_hands;		/* are we playing with open hands? */

	card_t lead_card;	/* the card that was lead this trick */
	char trump;		/* the suit of trump; 0-3 or other for none */

	int hand_num;		/* the number of the current hand (counting from ?) */
	int hand_size;		/* the size of the hand */
	int max_hand_length;	/* the largest possible hand */
	player_t dealer;	/* who dealt this hand (starts at random, then rotates) */

	int bid_count;		/* how many bids there have been this bidding cycle */
	int bid_total;		/* how many bids there will be this bidding cycle */
	int bid_rounds;		/* number of *rounds* of bidding */
	int max_bid_rounds;	/* the maximum number of bid rounds we can hold (increased at need) */
	player_t prev_bid;	/* who made the _previous_ bid? */
	player_t next_bid;	/* current/next bidder */
	int num_bid_choices;	/* the number of bid choices */
	int max_bid_choices;	/* the maximum number of choices there may be == sizeof(bid_texts) == sizeof(bid_choices) */
	int max_bid_length;	/* the longest possible bid (text-wise) */
	bid_t *bid_choices;	/* the bid choices -- must be allocated in game_launch or declared static */
	char **bid_texts;	/* the texts for the player to choose from -- allocated in game_launch  or declared static */
	char **bid_text_ref;	/* a pointer to the currently-used bid texts */

	int play_count;		/* how many plays there have been this trick */
	int play_total;		/* how many plays there will be this trick */
	player_t next_play;	/* current/next player */
	player_t curr_play;	/* current player, tracked automatically by req_bid */
	/* Note: the difference between these two is subtle, but important.  next_play is used
	 * to track the player whose hand is being played from.  curr_play is used automatically
	 * to remember who is supposed to be playing now.  In the case of bridge, next_play
	 * will go around the table (0, 1, 2, 3) for each play.  However, when the dummy is
	 * playing curr_play will point to the declarer, since they're the one we request the play
	 * from. */
	seat_t play_seat;	/* the seat being played from */

	player_t winner;	/* who won last trick */
	player_t leader;	/* who leads next trick/lead this trick */

	int trick_count;	/* how many tricks have been played this hand */
	int trick_total;	/* how many tricks there will be this hand */
	int trump_broken;	/* has trump been broken this hand?  See must_break_trump, above. */

	player_t num_players;	/* the number of players in the game */
	int player_count;	/* the number of human players who have joined, in total */
	struct game_player_t *players;	/* data for each player, allocated in game_init */

	seat_t num_seats;	/* the number of "seats" in the table (which includes fake non-players */
	struct game_seat_t *seats;	/* data for each seat, allocated in game_init */

	global_message_list_t *message_head;	/* global messages */
	global_message_list_t *message_tail;

	void *specific;		/* any extra game-specific data */

};

/* convert an "absolute" seat number s to the
 * "relative" seat number used by player p */
#define CONVERT_SEAT(s, p) (((s) - game.players[(p)].seat + game.num_seats) % game.num_seats)


/* Game-independent functions */
extern void set_game_state(server_state_t state);
extern void save_game_state();
extern void restore_game_state();

extern int send_player_list(player_t);
extern int send_play(card_t card, seat_t);
extern int send_table(player_t);
extern int send_sync(player_t);
extern int send_sync_all();
extern int send_gameover(int, player_t *);
extern int req_play(player_t, seat_t);
extern int rec_play(player_t);
extern void send_badplay(player_t, char *);
extern int send_hand(player_t, seat_t, int);
extern int req_newgame(player_t);
extern int send_newgame();
extern void init_game();


extern void next_play(void);	/* make the next move */

/* handle player events -- used to all just be "update" */
extern int handle_launch_event();
extern int handle_join_event(player_t);
extern int handle_leave_event(player_t);
extern int handle_newgame_event(player_t);
extern int handle_play_event(card_t);
extern int handle_bid_event(bid_t);

extern void init_ggzcards(int which);	/* pass in the name of the game */
extern int handle_ggz(int, int *);
extern int handle_player(player_t);

extern void set_num_seats(int num_seats);

/* the game structure */
extern struct wh_game_t game;




/* random helper function */
extern void *alloc(int);
extern char **alloc_string_array(int, int);

#endif /* __COMMON_H__ */
