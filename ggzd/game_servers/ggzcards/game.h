/*
 * File: game.h
 * Author: Rich Gade, Jason Short
 * Project: GGZCards game module
 * Date: 06/29/2000
 * Desc: Game functions
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

#include <stdio.h> /* just for debugging */

#include "cards.h"
#include "common.h"

/* ---------- LA POCHA ---------- */

/* la pocha specific data */
#define LAPOCHA ( *(lapocha_game_t *)(game.specific) )
typedef struct lapocha_game_t {
	int bid_sum;		/* what the sum of the bids is */
} lapocha_game_t;

/* ---------- SPADES ---------- */

/* special bids */
#define SPADES_NIL 1

#define GSPADES ( *(spades_game_t *)(game.specific) )
typedef struct spades_game_t {
	/* options */
	int nil_value;		/* generally 50 or 100 */
	int minimum_team_bid;	/* the minimum bid by one team */

	int bags[2];
} spades_game_t;

/* ---------- HEARTS ---------- */

#define GHEARTS ( *(hearts_game_t *)(game.specific) )
typedef struct hearts_game_t {
	int points_on_hand[10];	/* the points each player has taken this hand.  Works for up to 10 players. */
	char lead_card_face;	/* the card that leads first.  It's a club.  Two-deck issues aren't dealt with. */
} hearts_game_t;

/* ---------- BRIDGE ---------- */

/* special bids */
#define BRIDGE_PASS 1
#define BRIDGE_DOUBLE 2

/* special suits */
	/* clubs-spades = 0-3 = regular values */
#define BRIDGE_NOTRUMP 4

#define BRIDGE ( *(bridge_game_t *)(game.specific) )
typedef struct bridge_game_t {
	int pass_count;		/* number of passes in a row */

	/* we ignore the regular player_t score field altogether */
	int points_above_line[2];
	int points_below_line[2];

	/* contract information */
	int contract;		/* value of the contract */
	int contract_suit;	/* suit of the contract; 0-4 */
	int bonus;		/* 1=regular; 2=doubled; 4=redoubled */
	player_t declarer;	/* player with the contract */
	player_t dummy;		/* dummy player; the declarer's partner */

	int dummy_revealed;
} bridge_game_t;

/* ---------- SUARO ---------- */

/* special bids */
#define SUARO_PASS 1
#define SUARO_DOUBLE 2
#define SUARO_KITTY 3

/* special suits */
#define SUARO_LOW 0
	/* clubs-spades = 1-4 = regular values + 1 */
#define SUARO_HIGH 5

#define SUARO ( *(suaro_game_t *)(game.specific) )
typedef struct suaro_game_t {
	/* options */
	int shotgun;		/* are we playing shotgun suaro? */
	int unlimited_redoubling;	/* can doubling continue indefinitely? */
	int persistent_doubles;	/* a double isn't negated by another bid */

	int pass_count;		/* number of passes in a row */

	/* contract information */
	int contract;		/* value of the contract */
	int kitty;		/* 0=>no kitty; 1=>kitty.  Only applicable in shotgun suaro. */
	int contract_suit;	/* 0=>low, 5=>high, as above */
	int bonus;		/* 1 = regular; 2 = doubled; 4 = redoubled; etc. */
	player_t declarer;	/* player with the contract */

	int kitty_revealed;
} suaro_game_t;

/* ---------- EUCHRE ---------- */

/* special bids */
#define EUCHRE_PASS 1
#define EUCHRE_TAKE 2
#define EUCHRE_TAKE_SUIT 3

#define EUCHRE ( *(euchre_game_t *)(game.specific) )
typedef struct euchre_game_t {
	player_t maker;		/* just like the declarer */
	int alone;		/* is the dealer going alone? */
	int suit;		/* the suit of trump (TODO: is this necessary?) */
} euchre_game_t;

/* ---------- EUCHRE ---------- */

typedef struct skat_game_t {
	
} skat_game_t;

/* TODO: make a more generalized macro instead of LAPOCHA, etc. */



/* Game-specific functions */

extern int game_send_hand(int, int);		/* send a hand to a player */

extern void game_set_player_message(player_t); /* determine and send the player message */

extern int game_get_bid_text(char*, int, bid_t);	/* determines the textual string for the bid */
extern void game_start_bidding();	/* updates data for the first bid */
extern int game_get_bid();		/* gets a bid from next player */
extern int game_handle_bid(int);	/* handles a bid from current bidder */
extern void game_next_bid();		/* updates data for the next bid */
extern void game_start_playing();	/* updates data after the last bid/before the playing starts */

extern char* game_verify_play(int card_index);	/* verifies the play is legal */
extern void game_next_play();			/* sets up for next play */
extern void game_get_play(player_t);		/* retreives a play */
extern void game_handle_play(card_t);		/* handle a play */

extern int game_deal_hand(void);		/* deal next hand */
extern void game_end_trick(void);		/* end-of-trick calculations */
extern void game_end_hand(void);		/* end-of-hand calculations */

extern void game_init_game();			/* initialize the game data */
extern int game_get_options();			/* determine/request options */
extern void game_handle_options();		/* handle options from player */
extern void game_start_game();			/* start a game */
extern int game_test_for_gameover();		/* returns TRUE iff gameover */
extern int game_handle_gameover();		/* handle a gameover */

extern int game_valid_game(int);		/* is the game valid? */


/* random helper function */
extern void* alloc(int);
