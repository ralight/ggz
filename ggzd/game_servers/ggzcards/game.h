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
#include "games.h"

/* ---------- LA POCHA ---------- */

/* la pocha specific data */
#define LAPOCHA ( *(lapocha_game_t *)(game.specific) )
typedef struct lapocha_game_t {
	int bid_sum;		/* what the sum of the bids is */
} lapocha_game_t;

/* ---------- HEARTS ---------- */

#define GHEARTS ( *(hearts_game_t *)(game.specific) )
typedef struct hearts_game_t {
	int points_on_hand[10];	/* the points each player has taken this hand.  Works for up to 10 players. */
	char lead_card_face;	/* the card that leads first.  It's a club.  Two-deck issues aren't dealt with. */
} hearts_game_t;

/* ---------- BRIDGE ---------- */

/* special bids */
#define BRIDGE_PASS	1
#define BRIDGE_DOUBLE	2
#define BRIDGE_REDOUBLE	3

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

/* ---------- EUCHRE ---------- */

/* special bids */
#define EUCHRE_PASS 1
#define EUCHRE_TAKE 2
#define EUCHRE_TAKE_SUIT 3

#define EUCHRE ( *(euchre_game_t *)(game.specific) )
typedef struct euchre_game_t {
	player_t maker;		/* just like the declarer */
	int dealer_gets_card;	/* does the dealer get the up-card? */
	card_t up_card;		/* the "up-card" */
	int alone;		/* is the dealer going alone? */
	int suit;		/* the suit of trump (TODO: is this necessary?) */
} euchre_game_t;

/* ---------- EUCHRE ---------- */

typedef struct skat_game_t {
	
} skat_game_t;

/* TODO: make a more generalized macro instead of LAPOCHA, etc. */



/* Game-specific functions */

extern struct game_function_pointers game_funcs;


extern void game_init_game();			
extern int game_get_options();			
extern void game_handle_options();	

extern void game_set_player_message(player_t);

extern int game_get_bid_text(char*, int, bid_t);
extern void game_start_bidding();		
extern int game_get_bid();		
extern int game_handle_bid(bid_t);		
extern void game_next_bid();		

extern void game_start_playing();		
extern char* game_verify_play(card_t);	
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
extern int game_compare_cards(const void *, const void *);
extern int game_send_hand(player_t, seat_t);


