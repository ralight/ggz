/*
 * File: game.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards game module
 * Date: 06/29/2000
 * Desc: Game-dependent game functions
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

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <easysock.h>

#include "ggz.h"
#include "cards.h"
#include "game.h"
#include "games.h"
#include "protocols.h"

/* TODO: these should be low, clubs, diamonds, ..., high, but that won't fit in the client window */
char* suaro_suit_names[6] = {"lo", "C", "D", "H", "S", "hi"};
char* bridge_suit_names[5] = {"C", "D", "H", "S", "NT"};

/* helper functions */
static char** alloc_string_array(int, int);
/* static void free_string_array(char**, int); */


/* JDS: these are just helper functions which should be moved to another
 * file */

void* alloc(int size)
{
	void* ret = malloc(size);
	if (ret == NULL) {
		/* TODO... */
		ggz_debug("SERVER error: failed malloc.");
		exit (-1);
	}
	memset(ret, 0, size); /* zero it all */
	return ret;
}	

/* alloc_char_array, free_char_array
 *   this allocates/frees an array of strings
 *   it really shouldn't go here...
 */
static char** alloc_string_array(int num, int len)
{
	int i;
	char** bids;
	char* bids2;
	i = num * sizeof(char*);
	bids = (char**)alloc(i);

	i = len * num * sizeof(char);
	bids2 = (char*)alloc(i);

	for(i = 0; i < num; i++) {
		bids[i] = bids2;
		bids2 += len;
	}

	return bids;
}
/*
static void free_string_array(char** bids)
{
	int i;
	free(bids[0]);
	free(bids);
}
*/


/*
 * JDS: these functions are game-specific or contain game-specific code
 */

/* will be used to determine the game type from command-line parameter  */
int game_get_gametype(char* text)
{
	if (!strcmp(text, "bridge"))
		return GGZ_GAME_BRIDGE;

	if (!strcmp(text, "suaro"))
		return GGZ_GAME_SUARO;

	if (!strcmp(text, "lapocha"))
		return GGZ_GAME_LAPOCHA;

	if (!strcmp(text, "spades"))
		return GGZ_GAME_SPADES;

	if (!strcmp(text, "hearts"))
		return GGZ_GAME_HEARTS;

	/* NOTE: we may not yet be connected to the ggz server, in which case this won't work. */
	ggz_debug("Unknown game for '%s'.", text);
	return GGZ_GAME_UNKNOWN;
}

/* as soon as the game type is determined, the game should be initialized.  This is called
 * by game-independent code, but may be called in one of several places (e.g. launch, connection, newgame) */
/* TODO: it may be necessary to uninitialize a game before initializing a new one */
void game_init_game()
{
	player_t p;
	seat_t s;
	int proper_players = 4; /* the correct number of players */

	if (game.initted || game.which_game == GGZ_GAME_UNKNOWN) {
		ggz_debug("SERVER BUG: game_init_game called on unknown or previously initialized game.");
		return;
	}

	/* First, allocate the game */
	switch (game.which_game) {
		case GGZ_GAME_LAPOCHA:
			game.specific = alloc(sizeof(lapocha_game_t));
			break;
		case GGZ_GAME_SUARO:
			game.specific = alloc(sizeof(suaro_game_t));
			break;
		case GGZ_GAME_SPADES:
			game.specific = alloc(sizeof(spades_game_t));
			break;
		case GGZ_GAME_HEARTS:
			game.specific = alloc(sizeof(hearts_game_t));
			break;
		case GGZ_GAME_BRIDGE:
			game.specific = alloc(sizeof(bridge_game_t));
			break;
		default:
			ggz_debug("game_alloc_game not implemented for game %d.", game.which_game);
			exit(-1);
	}

	cards_create_deck(game.which_game);

	/* TODO: see TODO by other game.seats allocation */
	if (game.num_seats != 4) {
		game.num_seats = 4; /* default value */
		game.seats = (struct game_seat_t *)alloc(game.num_seats * sizeof(struct game_seat_t));
	}

	/* second round of game-specific initialization */
	switch (game.which_game) {
		case GGZ_GAME_SUARO:
			{
			static struct ggz_seat_t ggz[2] = { {GGZ_SEAT_NONE, "Kitty", -1},
							    {GGZ_SEAT_NONE, "Key Card", -1} };
			proper_players = 2;
			game.seats[0].ggz = &ggz_seats[0];
			game.players[0].seat = 0;
			game.seats[2].ggz = &ggz_seats[1];
			game.players[1].seat = 2;
			game.seats[1].ggz = &ggz[0];
			game.seats[3].ggz = &ggz[1];
			/* most possible bids for suaro: 6 suits * 5 numbers + pass + double = 32
			 * for shotgun suaro, that becomes 62 possible bids
			 * longest possible bid: 9 diamonds = 11
			 * for shotgun suaro that becomes K 9 diamonds = 13 */
			game.max_bid_choices = 62;
			game.max_bid_length = 13;
			game.max_hand_length = 9;
			game.target_score = 50;
			SUARO.shotgun = 1;	/* shotgun suaro */
			SUARO.declarer = -1;
			game.name = "Suaro";
			}
			break;
		case GGZ_GAME_LAPOCHA:
			proper_players = 4;
			for(p = 0; p < game.num_players; p++) {
				s = p;
				game.players[p].seat = s;
				game.seats[s].ggz = &ggz_seats[p];
			}
			game.max_bid_choices = 12;
			game.max_bid_length = 4;
			game.max_hand_length = 10;
			game.must_overtrump = 1;	/* in La Pocha, you *must* overtrump if you can */
			game.name = "La Pocha";
			break;
		case GGZ_GAME_BRIDGE:
			proper_players = 4;
			for(p = 0; p < game.num_players; p++) {
				s = p;
				game.players[p].seat = s;
				game.seats[s].ggz = &ggz_seats[p];
			}
			/* most possible bids for bridge: 7 * 5 suit bids + (re)double + pass = 37
			 * longest possible bid: "redouble" = 9 */
			game.max_bid_choices = 37;
			game.max_bid_length = 9;
			game.max_hand_length = 13;
			game.name = "Bridge";

			/* TODO: for now we won't use bridge scoring */
			BRIDGE.declarer = -1;
			break;
		case GGZ_GAME_SPADES:
			proper_players = 4;
			for(p = 0; p < game.num_players; p++) {
				s = p;
				game.players[p].seat = s;
				game.seats[s].ggz = &ggz_seats[p];
			}
			/* most possible bids for spades: 0-13 + Nil = 15
			 * longest possible bid: "Nil" = length 4 */
			game.max_bid_choices = 15;
			game.max_bid_length = 4;
			game.max_hand_length = 13;
			game.must_break_trump = 1;	/* in spades, you can't lead trump until it's broken */
			game.target_score = 500;	/* TODO: override with options */
			GSPADES.nil_value = 100;
			game.trump = SPADES;
			game.name = "Spades";
			break;
		case GGZ_GAME_HEARTS:
			game.trump = -1; /* no trump in hearts */
			proper_players = 4; /* TODO: doesn't have to be 4 */
			for(p = 0; p < game.num_players; p++) {
				s = p;
				game.players[p].seat = s;
				game.seats[s].ggz = &ggz_seats[p];
			}
			/* hearts has no bidding */
			game.max_bid_choices = 0;
			game.max_bid_length = 0;
			game.max_hand_length = 13;
			game.target_score = 100;
			game.name = "Hearts";
			break;
		default:  	
			ggz_debug("SERVER BUG: game_launch not implemented for game %d.", game.which_game);
			game.name = "(Unknown)";
	}

	set_global_message("game", game.name);
	send_global_message_toall("game");

	/* allocate hands */
	for (s=0; s<game.num_seats; s++) {
		game.seats[s].hand.cards = (card_t *)alloc(game.max_hand_length * sizeof(card_t));
	}

	/* allocate bidding arrays */
	game.bid_texts = alloc_string_array(game.max_bid_choices, game.max_bid_length);
	game.bid_choices = (bid_t*)alloc(game.max_bid_choices * sizeof(bid_t));

	set_global_message("", "");
	for (s = 0; s < game.num_seats; s++)
		game.seats[s].message[0] = 0;
	if (game.bid_texts == NULL || game.bid_choices == NULL) {
		ggz_debug("SERVER BUG: game.bid_texts not allocated.");
		exit(-1);
	}
	if (game.num_players != proper_players) {
		ggz_debug("SERVER BUG: wrong number of players for game %d: %d instead of %d.", game.which_game, game.num_players, proper_players);
		exit(-1);
	}

	game.initted = 1;
}

/* game_get_options
 *   this is very clunky; this must request options from the client
 *   also, everything in it is closely tied to game_handle_options()
 */
int game_get_options()
{
	int fd;
	fd = ggz_seats[game.host].fd;
	ggz_debug("Entering game_get_options.");
	if (fd == -1) {
		ggz_debug("SERVER BUG: nonexistent host.");
		return -1;
	}
	switch (game.which_game) {
		case GGZ_GAME_SUARO:
			/* four options for now:
			 *   shotgun -> boolean
			 *   unlimited redoubling -> boolean
			 *   persistent doubles -> boolean
			 *   target score -> 25, 50, 100, 200, 500, 1000
			 */
			game.num_options = 4;
			if (es_write_int(fd, WH_REQ_OPTIONS) < 0 ||
				es_write_int(fd, game.num_options) < 0 || /* number of options */
				es_write_int(fd, 1) < 0 || /* first option: 1 choice */
				es_write_int(fd, 1) < 0 || /* first option: default is 1 */
				es_write_string(fd, "shotgun") < 0 || /* the first option is "shotgun" */
				es_write_int(fd, 1) < 0 || /* second option: 1 choice */
				es_write_int(fd, 0) < 0 || /* second option: default is 0 */
				es_write_string(fd, "unlimited redoubling") < 0 ||
				es_write_int(fd, 1) < 0 || /* third option: 1 choice */
				es_write_int(fd, 0) < 0 || /* third option: default is 0 */
				es_write_string(fd, "persistent doubles") < 0 ||
				es_write_int(fd, 6) < 0 || /* fourth option: 6 choices */
				es_write_int(fd, 1) < 0 || /* fourth option: default is 1 (on a scale from 0-2) */
				es_write_string(fd, "Game to 25") < 0 ||
				es_write_string(fd, "Game to 50") < 0 ||
				es_write_string(fd, "Game to 100") < 0 ||
				es_write_string(fd, "Game to 200") < 0 ||
				es_write_string(fd, "Game to 500") < 0 ||
				es_write_string(fd, "Game to 1000") < 0
			   )
				return -1;
			break;
		case GGZ_GAME_SPADES:
			/* two options for now:
			 *   target score: 100, 250, 500, 1000
			 *   nil value: 50, 100
			 */
			game.num_options = 2;
			if (es_write_int(fd, WH_REQ_OPTIONS) < 0 ||
				es_write_int(fd, game.num_options) < 0 || /* number of options */
				es_write_int(fd, 2) < 0 || /* first option: 2 choices */
				es_write_int(fd, 1) < 0 || /* first option: default is 1 */
				es_write_string(fd, "Nil is worth 50") < 0 ||
				es_write_string(fd, "Nil is worth 100") < 0 ||
				es_write_int(fd, 4) < 0 || /* second option: 2 choices */
				es_write_int(fd, 2) < 0 || /* second option: default is 2 */
				es_write_string(fd, "Game to 100") < 0 ||
				es_write_string(fd, "Game to 250") < 0 ||
				es_write_string(fd, "Game to 500") < 0 ||
				es_write_string(fd, "Game to 1000") < 0
			   )
				return -1;
			break;
		default:
			game.options_initted = 1; /* no options */
			break;
	}	
	return 0;
}

/* game_handle_options
 *   handles options being sent from the client
 *   a value of -1 will be passed in as an option to indicate an error */
void game_handle_options(int *options)
{
	ggz_debug("Entering game_handle_options.");
	switch (game.which_game) {
		case GGZ_GAME_SUARO:
			if (options[0] >= 0) SUARO.shotgun = options[0];
			if (options[1] >= 0) SUARO.unlimited_redoubling = options[1];
			if (options[2] >= 0) SUARO.persistent_doubles = options[2];
			switch (options[3]) {
				/* this highlights a problem with this protocol.  Only discrete entries are possible.
				 * it would be better if the client could just enter a number */
				case 0: game.target_score = 25; break;
				case 1: game.target_score = 50; break;
				case 2: game.target_score = 100; break;
				case 3: game.target_score = 200; break;
				case 4: game.target_score = 500; break;
				case 5: game.target_score = 1000; break;
				default: break;
			}
			game.options_initted = 1; /* we could do another round of option requests, if we wanted */
			break;
		case GGZ_GAME_SPADES:
			switch (options[0]) {
				case 0: GSPADES.nil_value = 50;  break;
				case 1: GSPADES.nil_value = 100; break;
				default: break;
			}
			switch (options[1]) {
				case 0: game.target_score = 100; break;
				case 1: game.target_score = 250; break;
				case 2: game.target_score = 500; break;
				case 3: game.target_score = 1000; break;
				default: break;
			}
			game.options_initted = 1;
			break;
		default:
			ggz_debug("SERVER/CLIENT bug: game_handle_options called incorrectly.");
			break;
	}
}

/* game_start_game
 *   Called at the beginning of a game!
 */
void game_start_game(void)
{
	player_t p;

	/* TODO: initialize the game; right now we just assume everything's
	 * zero which won't be true the second time around. */
	for (p=0; p<game.num_players; p++) {
		game.players[p].score = 0;
	}

	ggz_debug("Game start completed successfully.");
}

/* game_handle_gameover
 *   The game is over; end out game-over message
 *   game-specific code to determine the winner(s) and
 *   call send_gameover */
int game_handle_gameover(void)
{
	player_t p;
	int hi_score = -9999;
	player_t winners[game.num_players];
	int winner_cnt = 0;

	switch (game.which_game) {
		case GGZ_GAME_HEARTS:
			/* in hearts, the low score wins */
			{
			int lo_score = 100;	
			for (p=0; p<game.num_players; p++) {
				if (game.players[p].score < lo_score) {
					winner_cnt = 1;
					winners[0] = p;
					lo_score = game.players[p].score;
				} else if (game.players[p].score == lo_score) {
					winners[winner_cnt] = p;
					winner_cnt++;
				}
			}
			}
			break;
		case GGZ_GAME_LAPOCHA:
			/* in La Pocha, anyone who has 0 at the end automatically wins */
			for (p=0; p<game.num_players; p++) {
				if(game.players[p].score == 0) {
					winners[winner_cnt] = p;
					winner_cnt++;
				}
			}
			if (winner_cnt > 0) break;
			/* else fall through */
		case GGZ_GAME_BRIDGE: /* TODO */
		case GGZ_GAME_SUARO:
		case GGZ_GAME_SPADES:
		default:
			/* in the default case, just take the highest score(s)
			 * this should automatically handle the case of teams! */
			for (p=0; p<game.num_players; p++) {
				if (game.players[p].score > hi_score) {
					winner_cnt = 1;
					winners[0] = p;
					hi_score = game.players[p].score;
				} else if (game.players[p].score == hi_score) {
					winners[winner_cnt] = p;
					winner_cnt++;
				}
			}
	}
	return send_gameover(winner_cnt, winners);
}

/* game_start_bidding
 *   called at the very start of bidding
 */
void game_start_bidding()
{
	switch (game.which_game) {
		case GGZ_GAME_LAPOCHA:
			/* all 4 players bid once, but the first bid determines the trump */
			game.bid_total = 5;
			LAPOCHA.bid_sum = 0;
			break;
		case GGZ_GAME_SUARO:
			{
			char suit;
			game.bid_total = -1; /* no set total */

			/* key card determines first bidder */
			suit = game.seats[3].hand.cards[0].suit;
			game.next_bid = (suit == CLUBS || suit == SPADES) ? 0 : 1;

			SUARO.pass_count = 0;
			SUARO.bonus = 1;
			}
			break;
		case GGZ_GAME_BRIDGE:
			game.next_bid = game.dealer; /* dealer bids first */
			game.bid_total = -1; /* no set total */
			BRIDGE.pass_count = 0;
			BRIDGE.bonus = 1;
			break;
		case GGZ_GAME_HEARTS:
			/* there is no bidding phase */
			set_game_state( WH_STATE_FIRST_TRICK );
			break;
		case GGZ_GAME_SPADES:
		default:
			/* all players bid once */
			game.bid_total = game.num_players;
			game.next_bid = (game.dealer + 1) % game.num_players;
	}
}

/* game_get_bid()
 *   asks for bid from the client/AI
 *   AI can be inserted here; just call update with a WH_EVENT_BID
 */
/* TODO: verify that it will work with and without bots */
int game_get_bid()
{
	int status = 0, index=0;
	bid_t bid;
	bid.bid = 0;

	switch (game.which_game) {
		case GGZ_GAME_SPADES:
			{
			int i;
			for (i = 0; i <= game.hand_size; i++) {
				bid.sbid.val = i;
				game.bid_choices[i] = bid;
				game_get_bid_text(game.bid_texts[i], game.max_bid_length, game.bid_choices[i]);
			}
			
			/* "Nil" bid */
			bid.bid = 0;
			bid.sbid.spec = SPADES_NIL;
			game.bid_choices[i] = bid;
			game_get_bid_text(game.bid_texts[i], game.max_bid_length, game.bid_choices[i]);

			/* TODO: other specialty bids */

			status = req_bid(game.next_bid, game.hand_size+2, game.bid_texts);
			}
			break;
		case GGZ_GAME_LAPOCHA:
			if (game.bid_count == 0) { /* determine the trump suit */
				/* handled just like a bid */
				if(game.hand_size != 10) {
					bid.bid = (long)cards_deal_card().suit;
					update(WH_EVENT_BID, (void*)&bid);
				} else
					status = req_bid(game.dealer, 4, suit_names);
			} else { /* get a player's numberical bid */
				int i, num=0;
				for (i = 0; i <= game.hand_size; i++) {
					/* the dealer can't make the sum of the bids equal the hand size;
					 * somebody's got to go down */
					if (game.next_bid == game.dealer &&
					    LAPOCHA.bid_sum + i == game.hand_size) {
						ggz_debug("Disallowing a bid of %d because that makes the bid sum of %d to equal the hand size of %d.",
							i, LAPOCHA.bid_sum, game.hand_size);
						continue;
					}
					game.bid_choices[num].bid = i;
					game_get_bid_text(game.bid_texts[num], game.max_bid_length, game.bid_choices[num]);
					num++;
				}

				status = req_bid(game.next_bid, num, game.bid_texts);
			}
			break;
		case GGZ_GAME_BRIDGE:
			/* this is based closely on the Suaro code, below */

			/* make a list of regular bids */
			for (bid.sbid.val = 1; bid.sbid.val <= 7; bid.sbid.val++) {
				for (bid.sbid.suit = CLUBS; bid.sbid.suit <= BRIDGE_NOTRUMP; bid.sbid.suit++) {
					if (bid.sbid.val < BRIDGE.contract) continue;
					if (bid.sbid.val == BRIDGE.contract && bid.sbid.suit <= BRIDGE.contract_suit) continue;
					snprintf(game.bid_texts[index], game.max_bid_length,
					"%d %s", bid.sbid.val, bridge_suit_names[(int)bid.sbid.suit]);
					game.bid_choices[index] = bid;
					index++;
				}
			}

			/* make "double" or "redouble" bid */
			if (BRIDGE.contract != 0
				/* TODO: one double *per team* */
			    && BRIDGE.bonus < 4) {
				bid.bid = 0;
				bid.sbid.spec = BRIDGE_DOUBLE;
				game.bid_choices[index] = bid;
				snprintf(game.bid_texts[index], game.max_bid_length,
					BRIDGE.bonus == 1 ? "double" : "redouble");
				index++;
			}

			/* make "pass" bid */
			bid.bid = 0;
			bid.sbid.spec = BRIDGE_PASS;
			game.bid_choices[index] = bid;
			snprintf(game.bid_texts[index], game.max_bid_length, "pass");
			index++;

			status = req_bid(game.next_bid, index, game.bid_texts);
			break;
		case GGZ_GAME_SUARO:
			/* in suaro, a bid consists of a number and a suit. */

			/* make a list of regular bids */
			for(bid.sbid.val = 5; bid.sbid.val <= 9; bid.sbid.val++) {
				for(bid.sbid.suit = SUARO_LOW; bid.sbid.suit <= SUARO_HIGH; bid.sbid.suit++) {
					if (bid.sbid.val < SUARO.contract) continue;
					if (bid.sbid.val == SUARO.contract && bid.sbid.suit <= SUARO.contract_suit) continue;
					snprintf(game.bid_texts[index], game.max_bid_length,
						"%d %s", bid.sbid.val, suaro_suit_names[(int)bid.sbid.suit]);
					game.bid_choices[index] = bid;
					index++;
					if (SUARO.shotgun) {
						/* in "shotgun" suaro, you are allowed to bid on the kitty just like on your hand! */
						bid_t kbid = bid;
						kbid.sbid.spec = SUARO_KITTY;
						snprintf(game.bid_texts[index], game.max_bid_length,
							"K %d %s", kbid.sbid.val, suaro_suit_names[(int)kbid.sbid.suit]);
						game.bid_choices[index] = kbid;
						index++;
					}
				}
			}

			/* make "double" or "redouble" bid */
			if ( SUARO.contract > 0
			     && (SUARO.bonus < 4 || SUARO.unlimited_redoubling)) {
				/* unless unlimited doubling is specifically allowed,
				 * only double and redouble are possible */
				bid.bid = 0;
				bid.sbid.spec = SUARO_DOUBLE;
				game.bid_choices[index] = bid;
				snprintf(game.bid_texts[index], game.max_bid_length,
					 SUARO.bonus == 1 ? "double" : "redouble");
				index++;
			}

			/* make "pass" bid */
			bid.bid = 0;
			bid.sbid.spec = SUARO_PASS;
			game.bid_choices[index] = bid;
			snprintf(game.bid_texts[index], game.max_bid_length, "pass");
			index++;

			status = req_bid(game.next_bid, index, game.bid_texts);
			break;
		default:
			ggz_debug("SERVER BUG: game_get_bid called for unimplemented game.");
			status = -1;
	}

	return status;
}

/* Handle incoming bid from player */
int game_handle_bid(int bid_index)
{
	bid_t bid;
	bid.bid = -1;

	ggz_debug("Entering game_handle_bid(%d).", bid_index);

	switch (game.which_game) {
		case GGZ_GAME_BRIDGE:
			/* closely based on the Suaro code, below */
			bid = game.bid_choices[bid_index];

			ggz_debug("The bid chosen is %d %s %d.", bid.sbid.val, bridge_suit_names[(int)bid.sbid.suit], bid.sbid.spec);
			
			if (bid.sbid.spec == BRIDGE_PASS) {
				BRIDGE.pass_count++;
			} else if (bid.sbid.spec == BRIDGE_DOUBLE) {
				BRIDGE.pass_count = 1;
				BRIDGE.bonus *= 2;
			} else {
				BRIDGE.declarer = game.next_bid;
				BRIDGE.dummy = (BRIDGE.declarer + 2) % 4;
				BRIDGE.pass_count = 1;
				BRIDGE.contract = bid.sbid.val;
				BRIDGE.contract_suit = bid.sbid.suit;
				ggz_debug("Setting bridge contract to %d %s.", BRIDGE.contract, bridge_suit_names[BRIDGE.contract_suit]);
				if (bid.sbid.suit < BRIDGE_NOTRUMP)
					game.trump = bid.sbid.suit;
				else
					game.trump = -1;
			}
			if (BRIDGE.contract)
				set_global_message("", "%d %s x%d", BRIDGE.contract,
					bridge_suit_names[(int)BRIDGE.contract_suit], BRIDGE.bonus);
			else
				set_global_message("", "no contract");
			send_global_message_toall("");
			break;
		case GGZ_GAME_SUARO:
			bid = game.bid_choices[bid_index];

			if (bid.sbid.spec == SUARO_PASS) {
				SUARO.pass_count++;
			} else if (bid.sbid.spec == SUARO_DOUBLE) {
				SUARO.pass_count = 1; /* one more pass will end it */
				SUARO.bonus *= 2;
			} else {
				SUARO.declarer = game.next_bid;
				SUARO.pass_count = 1; /* one more pass will end it */
				if (!SUARO.persistent_doubles)
					SUARO.bonus = 1; /* reset any doubles */
				SUARO.contract = bid.sbid.val;
				SUARO.kitty = (bid.sbid.spec == SUARO_KITTY);
				SUARO.contract_suit = bid.sbid.suit;
				if (bid.sbid.suit > SUARO_LOW && bid.sbid.suit < SUARO_HIGH)
					game.trump = bid.sbid.suit - 1; /* a minor hack */
				else
					game.trump = -1;
			}
			if (SUARO.contract)
				set_global_message("", "%s%d %s x%d",
					SUARO.kitty ? "K " : "",
					SUARO.contract, suaro_suit_names[(int)SUARO.contract_suit], SUARO.bonus);
			else
				set_global_message("", "no contract");
			send_global_message_toall("");
			break;
		case GGZ_GAME_SPADES:
			break; /* no special handling necessary */
		case GGZ_GAME_LAPOCHA:
			if (game.bid_count == 0) {
				game.trump = bid_index;

				set_global_message("", "Trump: %s.", suit_names[(int)game.trump % 4]);
				send_global_message_toall("");
			} else {
				bid = game.bid_choices[bid_index];
				LAPOCHA.bid_sum += bid.bid;
			}
			break;	
		default:
			ggz_debug("SERVER Not Implemented: game_handle_bid.");
 			break;
	}

	game_set_player_message(game.next_bid);
	
	return 0;	
}

/* sets up for the next bid
 * note that game.bid_count has already been incremented */
void game_next_bid()
{
	ggz_debug("Determining next bid.");
	switch (game.which_game) {
		case GGZ_GAME_BRIDGE:
			/* closely based on Suaro code, below */
			if (BRIDGE.pass_count == 4) {
				/* done bidding */
				if (BRIDGE.contract == 0) {
					ggz_debug("Four passes; redealing hand.");
					set_global_message("", "redeal");
					send_global_message_toall("");
					set_game_state( WH_STATE_NEXT_HAND ); /* redeal hand */
				} else {
					ggz_debug("Three passes; bidding is over.");
					game.bid_total = game.bid_count;
					/* contract was determined in game_handle_bid */
				}
			} else {
				if (game.bid_count == 0)
					game.next_bid = game.dealer;
				else
					game.next_bid = (game.next_bid + 1) % game.num_players;
			}
			break;
		case GGZ_GAME_SUARO:
			if (SUARO.pass_count == 2) {
				/* done bidding */
				if (SUARO.contract == 0) {
					ggz_debug("Two passes; redealing hand.");
					set_global_message("", "redeal");
					send_global_message_toall("");
					set_game_state( WH_STATE_NEXT_HAND ); /* redeal hand */
				} else {
					ggz_debug("A pass; bidding is over.");
					game.bid_total = game.bid_count;
					/* contract was determined in game_handle_bid */
				}
			} else
				goto normal_order;
			break;
		case GGZ_GAME_LAPOCHA:
			if (game.bid_count == 1) {
				game.next_bid = (game.dealer + 1) % game.num_players;
				break;
			}
			goto normal_order;
		case GGZ_GAME_SPADES:
		default:
normal_order:
			if (game.bid_count == 0)
				game.next_bid = (game.dealer + 1) % game.num_players;
			else
				game.next_bid = (game.next_bid + 1) % game.num_players;
	}
	
}

/* called between the bidding and playing sequences */
void game_start_playing(void)
{
	player_t p;

	switch (game.which_game) {
		case GGZ_GAME_LAPOCHA:
			game.leader = (game.dealer + 1) % game.num_players;
			set_global_message("", "%d %s x%d", BRIDGE.contract,
				bridge_suit_names[(int)BRIDGE.contract_suit], BRIDGE.bonus);
			send_global_message_toall("");
			break;
		case GGZ_GAME_BRIDGE:
			/* declarer is set in game_handle_bid */
			game.leader = (BRIDGE.declarer + 1) % game.num_players;
			game.trick_total = game.hand_size;
			game.play_total = game.num_players;
			break;
		case GGZ_GAME_SUARO:
			/* declarer is set in game_handle_bid */
			game.leader = 1 - SUARO.declarer;
			set_global_message("", "%s%d %s x%d",
				SUARO.kitty ? "K " : "",
				SUARO.contract, suaro_suit_names[(int)SUARO.contract_suit], SUARO.bonus);
			send_global_message_toall("");
			if (SUARO.kitty) {
				/* if it's a kitty bid, the declarer takes up the kitty and
				 * lays their own hand down (face up) in its place */
				card_t* temp;
				seat_t s1, s2;
				s1 = 1; /* kitty hand */
				s2 = game.players[SUARO.declarer].seat;
				/* this "swapping" only works because the cards in the hands
				 * are specifically allocated by malloc.  Also note that there
				 * are always 9 cards in both hands. */
				temp = game.seats[s1].hand.cards;
				game.seats[s1].hand.cards = game.seats[s2].hand.cards;
				game.seats[s2].hand.cards = temp;
				send_hand(SUARO.declarer, s2, 1); /* reveal the new hand to the player */
				for(p = 0; p<game.num_players; p++)
					/* reveal the kitty to everyone */
					send_hand(p, s1, 1);
			}
			break;
		case GGZ_GAME_HEARTS:
			/* we track the points that will be earned this hand. */
			for (p=0; p<game.num_players; p++)
				GHEARTS.points_on_hand[p] = 0;
		case GGZ_GAME_SPADES:
			/* fall through */
		default:
			game.leader = (game.dealer + 1) % game.num_players;
			break;
	}
}

/* verify that the play is legal; return NULL if it is an an error message otherwise
 * the error message should be statically declared! */
char* game_verify_play(int card_index)
{
	card_t card = game.seats[game.play_seat].hand.cards[card_index], c;
	seat_t s = game.play_seat;
	int cnt;

	/* TODO: hearts must be handled differently, since
	 * you can't lead *hearts* until *points* have been broken */

	/* the leader has his own restrictions */
	if (game.next_play == game.leader) {
		/* if must_break_trump is set, you can't lead trump until
		 * it's been broken.  This is the case in spades, for instance. */
		/* otherwise the leader can lead anything */
		if (!game.must_break_trump) return NULL;
		if (card.suit != game.trump) return NULL;
		if (game.trump_broken) return NULL;
		if (cards_suit_in_hand(&game.seats[s].hand, game.trump) == game.seats[s].hand.hand_size)
			/* their entire hand is trump! */
			return NULL;
		return "Trump has not yet been broken.";
	}

	/* following suit is always okay */
	if (card.suit == game.lead_card.suit) return NULL;

	/* not following suit is never allowed */
	if ( (cnt = cards_suit_in_hand(&game.seats[s].hand, game.lead_card.suit)) ) {
		ggz_debug("Player %d, playing from seat %d, played %s when they have %d of %s.",
			  game.next_play, s, suit_names[(int)card.suit], cnt, suit_names[(int)game.lead_card.suit]);
		return "You must follow suit.";
	}

	/* if must_overtrump is set, then you must overtrump to win if you can */
	if (game.must_overtrump && game.trump >= 0 && game.trump < 4) {
		/* The player doesn't have led suit.
		 * He MUST overtrump
		 * if he has a trump card higher than
		 * any trump card on the table. */
		char hi_trump, hi_trump_played;
		player_t p2;
		hi_trump = cards_highest_in_suit(&game.seats[s].hand,
						 game.trump);
		hi_trump_played = 0;
		for (p2=0; p2<game.num_players; p2++) {
			c = game.players[p2].table;
			if(c.suit == game.trump
			   && c.face > hi_trump_played)
				hi_trump_played = c.face;
		}

		/* the play is invalid if they had a higher trump than any on the table
		 * AND they didn't play a higher trump than any on the table */
		if (hi_trump > hi_trump_played
		    && !(card.suit == game.trump && card.face > hi_trump_played))
			return "You must play a higher trump than any already played.";
	}

	/* I guess the play was okay... */
	return NULL;	
}

/* sets up for the next play
 * note that game.play_count has already been incremented */
void game_next_play()
{
	game.next_play = (game.next_play + 1) % game.num_players;
}

/* this gets a play.  It most likely just requests one from the player, but
 * AI can be inserted to call update on a WH_EVENT_PLAY */
void game_get_play(player_t p)
{
	switch (game.which_game) {
		case GGZ_GAME_BRIDGE:
			if (p == BRIDGE.dummy) {
				/* the declarer plays the dummy's hand */
				req_play(BRIDGE.declarer, game.players[p].seat);
				break;
			}
			/* else fall through */
		default:
			/* in almost all cases, we just want the player to play from their own hand */
			req_play(p, game.players[p].seat);
			break;
	}
}

/* this handles a play.  Just about everything is taken care of by the
 * game-independent code; all that needs to be done here is anything
 * game-specific. */
void game_handle_play(card_t c)
{
	switch (game.which_game) {
		case GGZ_GAME_BRIDGE:
			if (game.play_count == 0 && game.trick_count == 0) {
				/* after the first play of the hand, we reveal
				 * the dummy's hand to everyone */
				player_t p;
				seat_t dummy_seat = game.players[BRIDGE.dummy].seat;
				for (p=0; p<game.num_players; p++) {
					if (p == BRIDGE.dummy) continue;
					send_hand(p, dummy_seat, 1);
				}
			}
			break;
		default:
			/* nothing needs to be done... */
			break;
	}
}

/* game_test_for_gameover
 *   called at the beginning of a new hand to determine if the game is over
 */
int game_test_for_gameover()
{
	player_t p;
	switch (game.which_game) {
		case GGZ_GAME_LAPOCHA:
			return (game.hand_num == 29);
		case GGZ_GAME_BRIDGE:
			ggz_debug("FIXME: bridge scoring not implemented.");
			return 0;
		case GGZ_GAME_SPADES:
		case GGZ_GAME_HEARTS:
		case GGZ_GAME_SUARO:
		default:
			/* in the default case, it's just a race toward a target score */
			for (p = 0; p < game.num_players; p++)
				if (game.players[p].score >= game.target_score)
					return 1;
			return 0;
	}

	return 0;
}


/* The number of cards dealt each hand */
static int lap_card_count[] = { 1, 1, 1, 1,
			    2, 3, 4, 5, 6, 7, 8, 9,
			    10, 10, 10, 10,
			    9, 8, 7, 6, 5, 4, 3, 2,
			    1, 1, 1, 1 };

/* Generate and send out a new hand */
int game_deal_hand(void)
{
	seat_t s;
	int result=0;

	ggz_debug("Dealing hand %d.", game.hand_num);

	switch (game.which_game) {
		case GGZ_GAME_LAPOCHA:
			/* in la pocha, there are a predetermined number of cards
			 * each hand, as defined by lap_card_count above */
			game.hand_size = lap_card_count[game.hand_num];
			game.trump = -1; /* must be determined later */
			goto regular_deal;
		case GGZ_GAME_SUARO:
			/* in suaro, players 0 and 1 (seats 0 and 2) get 9 cards each.
			 * the kitty (seat 1) also gets 9 cards, and the face card
			 * (seat 3) is just one card. */
			game.hand_size = 9;
			for(s = 0; s < 3; s++)
				cards_deal_hand(game.hand_size, &game.seats[s].hand);
			cards_deal_hand(1, &game.seats[3].hand);
			break;		
		case GGZ_GAME_SPADES:
		case GGZ_GAME_BRIDGE:
		case GGZ_GAME_HEARTS:
		default:
			game.hand_size = 52 / game.num_players;
regular_deal:
			for (s = 0; s < game.num_seats; s++)
				cards_deal_hand(game.hand_size, &game.seats[s].hand);
	}

	ggz_debug("Dealt hand with result %d.", result);

	return result;
}


/* game_send_hand
 *   Show a player a hand.
 */
int game_send_hand(player_t p, seat_t s)
{
	switch (game.which_game) {
		case GGZ_GAME_SUARO:
			/* each player can see their own hand plus the
			 * key card */
			return send_hand(p, s,
				game.players[p].seat == s || s == 3);
		case GGZ_GAME_BRIDGE:
			/* TODO: we explicitly send out the dummy hand, but a player who
			 * joins late won't see it.  We have the same problem with Suaro. */
			/* fall through */
		default:
			return send_hand(p, s, game.players[p].seat == s);
	}
	return -1;
}

/* game_get_bid_text
 *   places text for the bid into the buffer.  Returns the length of the text (from snprintf).
 */
int game_get_bid_text(char* buf, int buf_len, bid_t bid)
{
	switch (game.which_game) {
		case GGZ_GAME_SUARO:
			if (bid.sbid.spec == SUARO_PASS) return snprintf(buf, buf_len, "Pass");
			if (bid.sbid.spec == SUARO_DOUBLE) return snprintf(buf, buf_len, "Double"); /* TODO: differentiate redouble */
			return snprintf(buf, buf_len, "%s%d %s", (bid.sbid.spec == SUARO_KITTY) ? "K " : "", bid.sbid.val, suaro_suit_names[(int)bid.sbid.suit]);
		case GGZ_GAME_BRIDGE:
			if (bid.sbid.spec == BRIDGE_PASS) return snprintf(buf, buf_len, "Pass");
			if (bid.sbid.spec == BRIDGE_DOUBLE) return snprintf(buf, buf_len, "Double"); /* TODO: differentiate redouble */
			return snprintf(buf, buf_len, "%d %s", bid.sbid.val, bridge_suit_names[(int)bid.sbid.suit]);
		case GGZ_GAME_SPADES:
			if (bid.sbid.spec == SPADES_NIL) return snprintf(buf, buf_len, "Nil");
			return snprintf(buf, buf_len, "%d", (int)bid.sbid.val);
		case GGZ_GAME_LAPOCHA:
			return snprintf(buf, buf_len, "%d", (int)bid.bid);
		case GGZ_GAME_HEARTS:
		default:
			return snprintf(buf, buf_len, "%s", "");
	}
}

void game_set_player_message(player_t p)
{
	seat_t s = game.players[p].seat;
	char* message = game.seats[s].message;
	int len = 0;

	/* This function is tricky.  The problem is that we're trying to assemble a single player string out of
	 * multiple units of data - score, bid, tricks, etc.  The solution here is to integrate these all into one
	 * function (this one).  The problem is that you have to check each unit of data to see if it should be
	 * shown - for instance, tricks should only be shown while the hand is being played.  Another solution
	 * would be to update things separately.  The disadvantage there is that the code is all spread out, and
	 * it's difficult to deal with multiple units of data */

	/* Note that this way depends on this function being called at the proper times -
	 * i.e. every time the player is affected, and on some game state changes.  Much
	 * of this is handled by the game-independent code */

	/* anyway, it's really ugly, but I don't see a better way... */

	switch (game.which_game) {
		case GGZ_GAME_BRIDGE:
			if (p == BRIDGE.declarer)
				len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "declarer\n");
			/* TODO */
			goto normal_message;
		case GGZ_GAME_SUARO:
			if (p == SUARO.declarer)
				len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "declarer\n");
			/* TODO: declarer really shouldn't be at the top */
			goto normal_message;
		case GGZ_GAME_LAPOCHA:
			if (game.state != WH_STATE_NEXT_BID && game.state != WH_STATE_WAIT_FOR_BID) {
				len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Contract: %d\n", (int)game.players[p].bid.bid);
			}
			goto normal_message;
		case GGZ_GAME_SPADES:
			len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Score: %d (%d)\n", game.players[p].score, GSPADES.bags[p%2]);
			if (game.state != WH_STATE_NEXT_BID && game.state != WH_STATE_WAIT_FOR_BID) {
				/* we show both the individual and team contract */
				char bid_text[game.max_bid_length];
				int contract = game.players[p].bid.sbid.val + game.players[(p+2)%4].bid.sbid.val;
				game_get_bid_text(bid_text, game.max_bid_length, game.players[p].bid);
				if (*bid_text) len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Bid: %s (%d)\n", bid_text, contract);
			}
			if (game.state == WH_STATE_WAIT_FOR_PLAY || game.state == WH_STATE_NEXT_TRICK || game.state == WH_STATE_NEXT_PLAY) {
				len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Tricks: %d (%d)\n",
					game.players[p].tricks, game.players[p].tricks + game.players[(p+2)%4].tricks);

			}
			goto bid_message_only;
		case GGZ_GAME_HEARTS:
		default:
normal_message:
			len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Score: %d\n", game.players[p].score);
			if (game.state == WH_STATE_WAIT_FOR_PLAY || game.state == WH_STATE_NEXT_TRICK || game.state == WH_STATE_NEXT_PLAY)
				len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Tricks: %d\n", game.players[p].tricks);
bid_message_only:
			if (game.state == WH_STATE_NEXT_BID || game.state == WH_STATE_WAIT_FOR_BID) {
				char bid_text[game.max_bid_length];
				game_get_bid_text(bid_text, game.max_bid_length, game.players[p].bid);
				if (*bid_text) len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Bid: %s\n", bid_text);
			}
			break;
	}

	send_player_message_toall(s);
}

/* Figure who won the trick
 * game.winner will be announced as the winner */
void game_end_trick(void)
{
	player_t hi_player = game.leader, p, lo_player = game.leader;
	card_t hi_card = game.lead_card, lo_card = game.lead_card;

	ggz_debug("Scoring trick.");

	/* default method of winning tricks: the winning card is the highest
	 * card of the suit lead, or the highest trump if there is trump */
	for (p=0; p<game.num_players; p++) {
		card_t card = game.players[p].table;
		if ( (card.suit == game.trump && (hi_card.suit != game.trump || hi_card.face < card.face))
		   || (card.suit == hi_card.suit && card.face > hi_card.face) ) {
			hi_card = card;
			hi_player = p;
		}
		if (card.suit == lo_card.suit && card.face < lo_card.face) {
			/* tracking the low card is unnecessary in most cases,
			 * but it saves us trouble in a few cases (like "low" bids in Suaro) */
			lo_card = card;
			lo_player = p;
		}
	}


	switch (game.which_game) {
		case GGZ_GAME_SUARO:
			if (SUARO.contract_suit == SUARO_LOW) {
				/* low card wins */
				hi_player = lo_player;
				hi_card = lo_card;
			}			
			break;
		case GGZ_GAME_HEARTS:
			for(p=0; p<game.num_players; p++) {
				card_t card = game.players[p].table;
				if (card.suit == HEARTS)
					GHEARTS.points_on_hand[hi_player]++;
				if (card.suit == SPADES && card.face == QUEEN)
					GHEARTS.points_on_hand[hi_player] += 13;
			}
			break;
		case GGZ_GAME_BRIDGE:
		case GGZ_GAME_SPADES:
			/* update teammate's info as well */
			game_set_player_message((hi_player+2)%4);
			break;
		case GGZ_GAME_LAPOCHA:		
		default:
			/* no additional scoring is necessary */
			break;
	}

	game.players[hi_player].tricks++;
	game.leader = game.winner = hi_player;

	game_set_player_message(hi_player);
}


/* Calculate scores for this hand and announce */
void game_end_hand(void)
{
	player_t p;

	switch (game.which_game) {
		case GGZ_GAME_LAPOCHA:
			for(p=0; p<game.num_players; p++) {
				ggz_debug("Player %d got %d tricks on a bid of %d", p,
					  game.players[p].tricks, (int)game.players[p].bid.bid);
				if(game.players[p].tricks == game.players[p].bid.bid)
					game.players[p].score += 10 + (5 * game.players[p].bid.bid);
				else
					game.players[p].score -= 5 * game.players[p].bid.bid;
			}
			break;
		case GGZ_GAME_BRIDGE:
			{
			int points_above, points_below, tricks;
			int winning_team, team;
			int tricks_above, tricks_below;
			tricks = game.players[BRIDGE.declarer].tricks + game.players[BRIDGE.dummy].tricks;
			if (tricks >= BRIDGE.contract) {
				winning_team = BRIDGE.declarer % 2;
				tricks_below = BRIDGE.contract;
				tricks_above = tricks - BRIDGE.contract;

			} else {
				winning_team = (BRIDGE.declarer + 1) % 2;
				tricks_below = 0;
				tricks_above = BRIDGE.contract - tricks;
			}
			switch (BRIDGE.contract_suit) {
				case CLUBS:
				case DIAMONDS:
					points_below = 20 * tricks_below;
					points_above = 20 * tricks_above;
					break;
				case HEARTS:
				case SPADES:
					points_below = 30 * tricks_below;
					points_above = 30 * tricks_above;
					break;
				case BRIDGE_NOTRUMP:
				default:
					points_below = 30 * tricks_below + (tricks_below > 0) ? 10 : 0;
					points_above = 30 * tricks_above;
					break;
			}
			points_below *= BRIDGE.bonus;
			points_above *= BRIDGE.bonus;
			BRIDGE.points_above_line[winning_team] += points_above;
			BRIDGE.points_below_line[winning_team] += points_below;
			
			if (BRIDGE.points_below_line[winning_team] >= 100) {
				/* 500 point bonus for winning a game */
				BRIDGE.points_above_line[winning_team] += 500;
				for (team=0; team<2; team++) {
					BRIDGE.points_above_line[team] += BRIDGE.points_below_line[team];
					BRIDGE.points_below_line[team] = 0;
				}
			}
			/* TODO: vulnerable, etc. */
			}

			BRIDGE.declarer = BRIDGE.dummy = -1;
			BRIDGE.contract = 0;
			break;
		case GGZ_GAME_SUARO:
			{
			int points, tricks;
			player_t winner;
			tricks = game.players[SUARO.declarer].tricks;
			if (tricks >= SUARO.contract) {
				int overtricks = tricks - SUARO.contract;
				winner = SUARO.declarer;
				/* you get the value of the contract MINUS
				 * the number of overtricks == 2 * contract - tricks */
				points = (SUARO.contract - overtricks) * SUARO.bonus;
				ggz_debug("Player %d made their bid of %d, plus %d overtricks for %d points.",
					  winner, SUARO.contract, overtricks, points);
			} else {
				winner = 1-SUARO.declarer;
				/* for setting, you just get the value of the contract */
				points = SUARO.contract * SUARO.bonus;
				ggz_debug("Player %d set the bid of %d, earning %d points.",
					  winner, SUARO.contract, points);
			}
			game.players[winner].score += points;
			SUARO.declarer = -1;
			SUARO.contract = 0;
			}
			break;
		case GGZ_GAME_SPADES:
			for (p=0; p<2; p++) {
				int tricks, bid, score;
				bid = game.players[p].bid.sbid.val + game.players[p+2].bid.sbid.val;
				tricks = game.players[p].tricks + game.players[p+2].tricks;
				if (tricks >= bid) {
					int bags = tricks-bid;
					score = 10 * bid + bags;
					GSPADES.bags[p] += bags;
					if (GSPADES.bags[p] >= 10) {
						/* you lose 100 points for 10 overtricks */
						GSPADES.bags[p] -= 10;
						score -= 100;
					}
				} else
					score = -10 * bid;
				ggz_debug("Team %d bid %d, took %d, earned %d.", (int)p, bid, tricks, score);
				game.players[p].score += score;
				game.players[p+2].score += score;
			}
			for (p=0; p<4; p++) {
				if (game.players[p].bid.sbid.spec == SPADES_NIL) {
					int score = (game.players[p].tricks == 0 ? GSPADES.nil_value : -GSPADES.nil_value);
					ggz_debug("Player %d earned %d for going nil.", (int)p, score);
					game.players[p].score += score;
					game.players[(p+2)%4].score += score;
				}
			}
			break;
		case GGZ_GAME_HEARTS:
			for (p=0; p<game.num_players; p++) {
				int points = GHEARTS.points_on_hand[p];
				int score = (points == 26 ? -26 : points);
				/* if you take all 26 points you "shoot the moon" and earn -26 instead.
				 * TODO: option of giving everyone else 26. */
				game.players[p].score += score;
			}
			break;
		default:
			ggz_debug("SERVER not implemented: game_end_hand for game %d.", game.which_game);
			break;
	}                                                         	
}

/* returns a boolean */
int game_valid_game(int which_game)
{
	switch (which_game) {
		case GGZ_GAME_SUARO:
			return (game.num_players == 2);
		case GGZ_GAME_ROOK:
		case GGZ_GAME_EUCHRE:
			return 0;	/* not yet supported */
		case GGZ_GAME_BRIDGE:
		case GGZ_GAME_LAPOCHA:
		case GGZ_GAME_SPADES:
			return (game.num_players == 4);
		case GGZ_GAME_HEARTS:
			return (game.num_players > 2 && game.num_players <= 7);	/* 3-7 players */
		default:
			ggz_debug("SERVER BUG: game_valid_gams: unknown game %d.", which_game);
			return 0;
	}
}


