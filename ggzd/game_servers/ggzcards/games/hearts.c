/* 
 * File: games/hearts.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for Hearts
 * $Id: hearts.c 2823 2001-12-09 08:16:26Z jdorje $
 *
 * Copyright (C) 2001 Brent Hendricks.
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

#include <stdlib.h>

#include "../common.h"

#include "hearts.h"

static int hearts_is_valid_game(void);
static void hearts_init_game(void);
static void hearts_get_options(void);
static int hearts_handle_option(char *option, int value);
static char *hearts_get_option_text(char *buf, int bufsz, char *option,
				    int value);
static int hearts_handle_gameover(void);
static void hearts_start_bidding(void);
static void hearts_start_playing(void);
static char *hearts_verify_play(card_t card);
static void hearts_end_trick(void);
static void hearts_end_hand(void);

struct game_function_pointers hearts_funcs = {
	hearts_is_valid_game,
	hearts_init_game,
	hearts_get_options,
	hearts_handle_option,
	hearts_get_option_text,
	game_set_player_message,
	game_get_bid_text,
	hearts_start_bidding,
	game_get_bid,
	game_handle_bid,
	game_next_bid,
	hearts_start_playing,
	hearts_verify_play,
	game_next_play,
	game_get_play,
	game_handle_play,
	game_deal_hand,
	hearts_end_trick,
	hearts_end_hand,
	game_start_game,
	game_test_for_gameover,
	hearts_handle_gameover,
	game_map_card,
	game_compare_cards,
	game_send_hand
};


static int hearts_is_valid_game(void)
{
	return (game.num_players >= 3 && game.num_players <= 6);
}

static void hearts_init_game(void)
{
	seat_t s;

	game.specific = ggz_malloc(sizeof(hearts_game_t));
	set_num_seats(game.num_players);
	for (s = 0; s < game.num_seats; s++)
		assign_seat(s, s);	/* one player per seat */

	game.trump = -1;	/* no trump in hearts */
	game.target_score = 100;

	GHEARTS.no_blood = 1;
}

static void hearts_get_options(void)
{
	add_option("jack_diamonds", 1, 0, "The jack of diamonds counts -10");
	add_option("no_blood", 1, 1, "No blood on the first trick");
	add_option("num_decks", 1, 0, "Play with two decks");
	game_get_options();
}

static int hearts_handle_option(char *option, int value)
{
	if (!strcmp("jack_diamonds", option))
		GHEARTS.jack_diamonds = value;
	else if (!strcmp("no_blood", option))
		GHEARTS.no_blood = value;
	else if (!strcmp("num_decks", option)) {
		/* HACK - the deck will already have been made at this point,
		   so we need to destroy and recreate it. */
		seat_t s;
		GHEARTS.num_decks = value + 1;
		game.deck_type =
			(value == 0) ? GGZ_DECK_FULL : GGZ_DECK_DOUBLE;
		cards_destroy_deck();
		cards_create_deck(game.deck_type);

		/* allocate hands */
		game.max_hand_length = cards_deck_size() / game.num_players;
		for (s = 0; s < game.num_seats; s++) {
			if (game.seats[s].hand.cards)
				ggz_free(game.seats[s].hand.cards);
			game.seats[s].hand.cards =
				ggz_malloc(game.max_hand_length *
					   sizeof(card_t));
		}
	} else
		return game_handle_option(option, value);
	return 0;
}

static char *hearts_get_option_text(char *buf, int bufsz, char *option,
				    int value)
{
	if (!strcmp("jack_diamonds", option))
		snprintf(buf, bufsz,
			 "The jack of diamonds rule is %sbeing used.",
			 value ? "" : "not ");
	else if (!strcmp("no_blood", option))
		snprintf(buf, bufsz, "%s is allowed on the first trick.",
			 value ? "No blood" : "Blood");
	else if (!strcmp("num_decks", option)) {
		if (value > 0)
			snprintf(buf, bufsz, "%d decks are being used.",
				 value + 1);
	} else
		return game_get_option_text(buf, bufsz, option, value);
	return buf;
}

static int hearts_handle_gameover(void)
{
	player_t p;
	player_t winners[game.num_players];
	int winner_cnt = 0;


	/* in hearts, the low score wins */
	int lo_score = 100;
	for (p = 0; p < game.num_players; p++) {
		if (game.players[p].score < lo_score) {
			winner_cnt = 1;
			winners[0] = p;
			lo_score = game.players[p].score;
		} else if (game.players[p].score == lo_score) {
			winners[winner_cnt] = p;
			winner_cnt++;
		}
	}
	return send_gameover(winner_cnt, winners);
}

static void hearts_start_bidding(void)
{
	/* there is no bidding phase */
	set_game_state(STATE_FIRST_TRICK);
}

static void hearts_start_playing(void)
{
	player_t p;
	char face;

	game.trick_total = game.hand_size;
	game.play_total = game.num_players;

	/* we track the points that will be earned this hand. */
	for (p = 0; p < game.num_players; p++)
		GHEARTS.points_on_hand[p] = 0;

	/* in hearts, the lowest club leads first */
	game.leader = -1;

	/* this is initialized to -1 because under some systems, it may be
	   possible for nobody to win the jack */
	GHEARTS.jack_winner = -1;

	/* TODO: what if we're playing with multiple decks? */
	for (face = 2; face <= ACE_HIGH; face++) {
		for (p = 0; p < game.num_players; p++) {
			/* TODO: this only works because the cards are sorted 
			   this way */
			card_t card =
				game.seats[game.players[p].seat].hand.
				cards[0];
			if (card.suit == CLUBS && card.face == face)
				game.leader = p;
		}
		if (game.leader != -1) {
			GHEARTS.lead_card_face = face;
			break;
		}
	}

	if (game.leader == -1) {
		ggzdmod_log(game.ggz,
			    "ERROR: SERVER BUG: " "nobody has a club.");
		game.leader = (game.dealer + 1) % game.num_players;
	}
}

static char *hearts_verify_play(card_t card)
{
	seat_t s = game.play_seat;

	/* in hearts, we have two additional rules about leading: the low
	   club must lead the first trick, and hearts cannot be lead until
	   broken. */

	if (game.next_play == game.leader) {
		/* the low club must lead on the first trick */
		if (game.trick_count == 0 && game.play_count == 0 &&
		    (card.suit != CLUBS
		     || card.face != GHEARTS.lead_card_face))
			return "You must lead the low club.";
		/* you can't lead *hearts* until they're broken */
		/* TODO: integrate this with must_break_trump */
		if (card.suit != HEARTS)
			return NULL;
		if (game.trump_broken)
			return NULL;
		if (cards_suit_in_hand(&game.seats[s].hand, HEARTS) ==
		    game.seats[s].hand.hand_size)
			/* their entire hand is hearts */
			return NULL;
		return "Hearts have not yet been broken.";
	}

	/* also in hearts, the "no blood on the first trick" rule means you
	   can't play points onto the first trick (unless that's all you
	   have) */
	if (GHEARTS.no_blood &&
	    game.trick_count == 0 &&
	    (card.suit == HEARTS
	     || (card.suit == SPADES && card.face == QUEEN))) {
		int i;
		card_t c;
		for (i = 0; i < game.seats[game.play_seat].hand.hand_size;
		     i++) {
			c = game.seats[game.play_seat].hand.cards[i];
			if (c.suit == HEARTS)
				continue;
			if (c.suit == SPADES && c.face == QUEEN)
				continue;
			return "No blood on the first trick!";
		}
	}

	return game_verify_play(card);
}

static void hearts_end_trick(void)
{
	player_t p;
	game_end_trick();
	for (p = 0; p < game.num_players; p++) {
		card_t card = game.seats[game.players[p].seat].table;
		int points = 0;
		if (card.suit == HEARTS)
			points = 1;
		if (card.suit == SPADES && card.face == QUEEN)
			points = 13;
		/* as an optional rule, the Jack of Diamonds is worth -10 */
		if (GHEARTS.jack_diamonds && card.suit == DIAMONDS
		    && card.face == JACK)
			GHEARTS.jack_winner = game.winner;

		if (points > 0) {
			/* in hearts, it's not really "trump broken" but
			   rather "points broken". However, it works about
			   the same way. */
			game.trump_broken = 1;
		}
		GHEARTS.points_on_hand[game.winner] += points;
	}
}

static void hearts_end_hand(void)
{
	player_t p;
	int max = 0;
	char buf[1024];

	for (p = 0; p < game.num_players; p++) {
		int points = GHEARTS.points_on_hand[p];
		/* if you take all 26 points you "shoot the moon" and earn
		   -26 instead. TODO: option of giving everyone else 26.  It
		   could be handled as a bid... */
		int score = (points == 26 ? -26 : points);
		int fullscore = score;

		if (GHEARTS.jack_winner == p)
			fullscore -= 10;
		game.players[p].score += fullscore;

		if (score == -26) {
			snprintf(buf, sizeof(buf), "%s shot the moon.",
				 ggzd_get_player_name(p));
			max = 26;
		} else if (fullscore > max) {
			/* only the maximum player's score is written; this
			   is less than ideal. */
			max = fullscore;
			snprintf(buf, sizeof(buf), "%s took %d points.",
				 ggzd_get_player_name(p), fullscore);
		}
	}
	set_global_message("", "%s", buf);
}
