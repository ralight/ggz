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

#include "cards.h"
#include "games.h"
#include "protocol.h"

#ifndef __COMMON_H__
#define __COMMON_H__

/* GGZCards server game states */
typedef enum {
	WH_STATE_PRELAUNCH,		/* before the launch happens */
	WH_STATE_NOTPLAYING,		/* no game started */
	WH_STATE_WAITFORPLAYERS,	/* waiting for players */
	WH_STATE_NEXT_HAND,		/* creating a new hand */
	WH_STATE_FIRST_BID,		/* about to have the first bid */
	WH_STATE_NEXT_BID,		/* asking for new bid */
	WH_STATE_WAIT_FOR_BID,		/* waiting for a bid */
	WH_STATE_FIRST_TRICK,		/* about to have the first trick of a hand */
	WH_STATE_NEXT_TRICK,		/* time for the next trick */
	WH_STATE_NEXT_PLAY,		/* asking for a new play */
	WH_STATE_WAIT_FOR_PLAY		/* waiting for a play */
} server_state_t;

/* GGZCards game events */
typedef enum {
	WH_EVENT_LAUNCH,		
	WH_EVENT_NEWGAME,	
	WH_EVENT_JOIN,		
	WH_EVENT_LEAVE,		
	WH_EVENT_BID,		
	WH_EVENT_PLAY		
} server_event_t;


/* in different games, bids may have different meanings.
 *   we'll just use this arbitrary data structure for it */
typedef union bid_t {
	/* this assumes a "long" is at least 32-bit. */
	long bid;
	struct special_bid_struct {
		/* this can be used for many different games
		 * that have unusual but similar bidding.
		 * Different games may use it differently. */
		char val;	/* the value of the bid */
		char suit;	/* the suit of the bid (generally trump) */
		char spec;	/* specialty bids (defined per-game) */
	} sbid;
} bid_t;

/* all players have seats, but every seat doesn't necessarily have a player.
 * some seats may be dummies or kitties */
/* the tricky thing is that GGZ knows only about _players_ while the
 * client game knows only about _seats_ */
#define MAX_MESSAGE_LENGTH 100
struct game_seat_t {
	hand_t hand;
	card_t table;
	struct ggz_seat_t * ggz; /* ggz seat data; probably points to something in ggz_seats */
	char message[MAX_MESSAGE_LENGTH];
};
typedef int seat_t; /* just to make things clearer */

struct game_player_t {
	int score;
	bid_t bid;
	int tricks;
	int seat; /* the number of the player's seat */
	int ready;
};
typedef int player_t; /* just to make things clearer */

/* Data structure for generic trick-taking card game */
struct wh_game_t {
	game_type_t which_game;	/* the game; currently defined in cards.h */
	deck_type_t deck_type;	/* the type of deck used, as defined in cards.h */
	char* name;		/* the name of the game */

	int initted;		/* has the game been initialized? */
	int num_options;	/* the number of options the game has */
	int options_initted;	/* have the options been initialized? */
	player_t host;		/* the host of the table; cannot be an AI */ /* TODO: currently it's always player 0 */

	server_state_t state;		/* the current state of the game (see WH_STATE, above) */
	server_state_t saved_state;	/* any time while waiting, the state we _would_ be in if we weren't waiting */

	/* these next few are general game-specific options that are used by the game-independent code */
	int must_overtrump;	/* if this is set, then a player must trump/overtrump if possible (i.e. La Pocha) */
	int must_break_trump;	/* if this is set, then trump can't be lead until it's been broken (i.e. spades) */
	int target_score;	/* after someone reaches this score, the game is over (if used) */
	int last_trick;		/* should the last trick be sent to all the players? */
	int last_hand;		/* should the last hand be sent to all the players? */

	card_t lead_card;	/* the card that was lead this trick */
	char trump;		/* the suit of trump; 0-3 or other for none*/

	int hand_num;		/* the number of the current hand (counting from ?) */
	int hand_size;		/* the size of the hand */
	int max_hand_length;	/* the largest possible hand */
	player_t dealer;	/* who dealt this hand (starts at random, then rotates) */

	int bid_count;		/* how many bids there have been this bidding cycle */
	int bid_total;		/* how many bids there will be this bidding cycle */
	player_t next_bid;	/* current/next bidder */
	int num_bid_choices;		/* the number of bid choices */
	int max_bid_choices;	/* the maximum number of choices there may be == sizeof(bid_texts) == sizeof(bid_choices) */
	int max_bid_length;	/* the longest possible bid (text-wise) */
	bid_t* bid_choices;	/* the bid choices -- must be allocated in game_launch or declared static*/
	char** bid_texts;	/* the texts for the player to choose from -- allocated in game_launch  or declared static */
	char** bid_text_ref;	/* a pointer to the currently-used bid texts */		

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
	struct game_player_t *players;	/* data for each player, allocated in game_init */

	seat_t num_seats;		/* the number of "seats" in the table (which includes fake non-players */
	struct game_seat_t *seats;	/* data for each seat, allocated in game_init */

	char* messages[256];	/* global messages */		

	void* specific;		/* any extra game-specific data */
		
};

/* convert an "absolute" seat number s to the
 * "relative" seat number used by player p */
#define CONVERT_SEAT(s, p) (((s) - game.players[(p)].seat + game.num_seats) % game.num_seats)


/* Game-independent functions */
extern void set_game_state(server_state_t state);
extern void save_game_state();
extern void restore_game_state();
extern void send_player_message(seat_t, player_t);
extern void send_player_message_toall(seat_t);
extern void set_all_player_messages();

extern void send_global_message(char*, player_t);
extern void send_global_message_toall(char*);
extern void send_all_global_messages(player_t p);
extern void set_global_message(char*, char*, ...);
extern char* get_global_message(char*);

extern int send_player_list(player_t);
extern int send_play(card_t card, seat_t);
extern int send_table(player_t);
extern int send_sync(player_t);
extern int send_sync_all();
extern int send_gameover(int, player_t*);
extern int req_bid(player_t, int, char**);
extern int req_play(player_t, seat_t);
extern int rec_play(player_t, int *);
extern void send_badplay(player_t, char*);
extern int send_hand(player_t, seat_t, int);
extern int req_newgame(player_t);
extern int send_newgame();

extern int rec_options(int, int*);

extern void next_play(void);				/* make the next move */
extern int update(server_event_t event, void *data);	/* receive a player move */

extern void init_game(int which); /* pass in the name of the game */
extern int handle_ggz(int, int *);
extern int handle_player(player_t);

extern void set_num_seats(int num_seats);

/* the game structure */
extern struct wh_game_t game;




/* random helper function */
extern void* alloc(int);
extern char** alloc_string_array(int, int);

#endif /* __COMMON_H__ */

