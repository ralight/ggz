/* 
 * File: games/hearts.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for Hearts
 * $Id: hearts.c 8561 2006-08-31 08:00:24Z jdorje $
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>	/* Site-specific config */
#endif

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "game.h"
#include "message.h"
#include "net.h"
#include "options.h"
#include "score.h"

#include "hearts.h"

#define MIN_HEARTS_PLAYERS 3
#define MAX_HEARTS_PLAYERS 6

#define GHEARTS ( *(hearts_game_t *)(game.specific) )
typedef struct hearts_game_t {
	/* the points each player has taken this hand. */
	int points_on_hand[MAX_HEARTS_PLAYERS];

	/* options */
	bool jack_diamonds;	/* is the jack-of-diamonds rule in effect? */
	bool no_blood;		/* no blood on the first trick */
	int num_decks;		/* the number of decks to use */

	player_t jack_winner;	/* who has won the jack of diamonds this
				   hand? */
	char lead_card_face;	/* the card that leads first.  It's a club.
				   Two-deck issues aren't dealt with. */
} hearts_game_t;

static bool hearts_is_valid_game(void);
static void hearts_init_game(void);
static void hearts_get_options(void);
static int hearts_handle_option(char *option, int value);
static char *hearts_get_option_text(char *buf, int bufsz, char *option,
				    int value);
static bool hearts_test_for_gameover(void);
static void hearts_handle_gameover(void);
static void hearts_start_bidding(void);
static void hearts_start_playing(void);
static char *hearts_verify_play(player_t p, card_t card);
static void hearts_end_trick(void);
static void hearts_end_hand(void);

game_data_t hearts_data = {
	"hearts",
	N_("Hearts"),
	"http://pagat.com/reverse/hearts.html",
	hearts_is_valid_game,
	hearts_init_game,
	hearts_get_options,
	hearts_handle_option,
	hearts_get_option_text,
	game_set_player_message,
	game_sync_player,
	game_get_bid_text,
	game_get_bid_desc,
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
	hearts_test_for_gameover,
	hearts_handle_gameover,
	game_map_card,
	game_compare_cards,
	game_send_hand
};


static bool hearts_is_valid_game(void)
{
	return game.num_players >= MIN_HEARTS_PLAYERS
	    && game.num_players <= MAX_HEARTS_PLAYERS;
}

static void hearts_init_game(void)
{
	seat_t s;

	game.specific = ggz_malloc(sizeof(hearts_game_t));
	set_num_seats(game.num_players);
	for (s = 0; s < game.num_seats; s++)
		assign_seat(s, s);	/* one player per seat */

	set_trump_suit(NO_SUIT);
	game.target_score = 100;
}

static void hearts_get_options(void)
{
	add_option("Hearts Options", "jack_diamonds",
		   "The jack of diamonds rule is that whoever wins the jack "
		   "of diamonds gets -10 points.",
		   1, FALSE, "Use the jack of diamonds rule");
	add_option("Hearts Options", "no_blood",
		   "If selected, nobody will be allowed to play point cards "
		   "on the first trick.",
		   1, TRUE, "No blood on the first trick");
	add_option("Hearts Options", "num_decks",
		   "How many decks should be used?",
		   1, FALSE, "Play with two decks");
	add_option("Hearts Options", "target_score",
		   "How many points until the game is over?",
		   5,
		   2,
		   "Game to 20", "Game to 50",
		   "Game to 100", "Game to 1000", "Unending game");
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
		destroy_deck(game.deck);
		game.deck = create_deck(game.deck_type);

		/* allocate hands */
		game.max_hand_length =
		    get_deck_size(game.deck) / game.num_players;
		for (s = 0; s < game.num_seats; s++) {
			if (game.seats[s].hand.cards)
				ggz_free(game.seats[s].hand.cards);
			game.seats[s].hand.cards =
			    ggz_malloc(game.max_hand_length *
				       sizeof(card_t));
		}
	} else if (!strcmp("target_score", option)) {
		switch (value) {
		case 0:
			game.target_score = 20;
			break;
		case 1:
			game.target_score = 50;
			break;
		case 2:
			game.target_score = 100;
			break;
		case 3:
			game.target_score = 1000;
			break;
		case 4:
			game.target_score = MAX_TARGET_SCORE;
			break;
		default:
			break;
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
	} else if (!strcmp(option, "target_score")) {
		if (game.target_score == MAX_TARGET_SCORE)
			snprintf(buf, bufsz, "The game will never end.");
		else
			snprintf(buf, bufsz,
				 "The game is being played to %d.",
				 game.target_score);
	} else
		return game_get_option_text(buf, bufsz, option, value);
	return buf;
}

static bool hearts_test_for_gameover(void)
{
	team_t t;
	int num_winners = 0, low_score = -1;

	/* We play until the highest-scoring person reaches the target score,
	   but we can never end in a tie. */
	if (!game_test_for_gameover())
		return FALSE;
	for (t = 0; t < game.num_teams; t++) {
		int score = get_team_score(t);

		if (low_score < 0 || score < low_score) {
			low_score = score;
			num_winners = 1;
		} else if (score == low_score) {
			num_winners++;
		}
	}

	return (num_winners == 1);
}

static void hearts_handle_gameover(void)
{
	player_t p, winners[game.num_players];
	int num_winners = 0, low_score = -1;
	int max_score = 0;

	/* In hearts, the low score wins.  Note that the low score
	   can be higher than the "target score", since we can never
	   end in a tie (we just play another hand). */
	for (p = 0; p < game.num_players; p++) {
		team_t t = game.players[p].team;
		int score = get_team_score(t);

		assert(t >= 0 && t < game.num_teams);
		max_score = MAX(score, max_score);
		if (low_score < 0 || score < low_score) {
			num_winners = 1;
			winners[0] = p;
			low_score = score;
		} else if (score == low_score) {
			winners[num_winners] = p;
			num_winners++;
		}
	}

	assert(num_winners == 1);
	assert(max_score >= game.target_score);
	handle_gameover_event(num_winners, winners);
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
			    game.seats[game.players[p].seat].hand.cards[0];
			if (card.suit == CLUBS && card.face == face)
				game.leader = p;
		}
		if (game.leader != -1) {
			GHEARTS.lead_card_face = face;
			break;
		}
	}

	if (game.leader == -1) {
		ggz_error_msg("Hearts: nobody has a club.");
		game.leader = (game.dealer + 1) % game.num_players;
	}
}

static char *hearts_verify_play(player_t p, card_t card)
{
	seat_t s = game.players[p].play_seat;

	/* in hearts, we have two additional rules about leading: the low
	   club must lead the first trick, and hearts cannot be lead until
	   broken. */

	if (game.next_play == game.leader) {
		/* the low club must lead on the first trick */
		if (game.play_count == 0 && game.trick_count == 0 &&
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

		/* Check to make sure they have a non-point card. */
		for (i = 0; i < game.seats[s].hand.hand_size; i++) {
			c = game.seats[s].hand.cards[i];
			if (c.suit == HEARTS)
				continue;
			if (c.suit == SPADES && c.face == QUEEN)
				continue;
			return "No blood on the first trick!";
		}
	}

	return game_verify_play(p, card);
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
			game.trump_broken = TRUE;
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
		assert(p == game.players[p].team);
		change_score(p, fullscore);

		if (score == -26) {
			snprintf(buf, sizeof(buf), "%s shot the moon.",
				 get_player_name(p));
			max = 26;
		} else if (fullscore > max) {
			/* only the maximum player's score is written; this
			   is less than ideal. */
			max = fullscore;
			snprintf(buf, sizeof(buf), "%s took %d points.",
				 get_player_name(p), fullscore);
		}
	}
	set_global_message("", "%s", buf);
}
