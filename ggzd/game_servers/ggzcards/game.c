/* 
 * File: game.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Server
 * Date: 06/29/2000
 * Desc: default game functions
 * $Id: game.c 3484 2002-02-27 05:40:53Z jdorje $
 *
 * This file was originally taken from La Pocha by Rich Gade.  It now
 * contains the default game functions; that is, the set of game functions
 * that may be used as defaults.  Each game (in games/) will define its
 * own functions as well.
 *
 * Copyright (C) 2000-2002 Brent Hendricks.
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
#  include <config.h>			/* Site-specific config */
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "game.h"
#include "message.h"
#include "net.h"
#include "options.h"
#include "play.h"

static void bad_game(char *func)
{
	assert(FALSE);
	ggzdmod_log(game.ggz, "ERROR: SERVER BUG: "
	            "%s not implemented for game %s.",
	            func, game.which_game);
}


/* is_valid_game returns true if the game is valid under the current game
   conditions (i.e. number of players); false otherwise */
int game_is_valid_game(void)
{
	return 0;
}


/* This function initializes the game soon after GGZCards is launched.  This
   is tricky, because it really has to set up _everything_ about the game,
   including allocating any extra data, setting any game options or global
   options, and setting up all of the GGZ information so that seats and
   players can both be used intelligently. */
void game_init_game(void)
{
	bad_game("init_game");
}


/* This function manually requests options from the client; the response is
   handled by game_handle_options.  Options are optional; it can be left
   as-is for a game that has no options. */
void game_get_options(void)
{
	add_option("open_hands", 1, 0, "Play with open hands");
}


/* This handles options being sent from the client.  A value of -1 will be
   passed in as an option to indicate an error.  It corresponds very closely
   to game_get_options, above; and can be left as-is for games that have no
   options. */
int game_handle_option(char *option, int value)
{
	if (!strcmp("open_hands", option))
		game.open_hands = value;
	else
		return -1;
	return 0;
}


/* This function finds descriptive text for the current setting of an option. 
 */
char *game_get_option_text(char *buf, int bufsz, char *option, int value)
{
	if (!strcmp("open_hands", option))
		if (value)
			snprintf(buf, bufsz, "Playing with open hands.");
		else
			*buf = 0;
	else
		return NULL;
	return buf;
}


/* Sets the player message for a given player. */
void game_set_player_message(player_t p)
{
	/* This function is tricky.  The problem is that we're trying to
	   assemble a single player string out of multiple units of data -
	   score, bid, tricks, etc.  The solution here is to integrate these
	   all into one function (this one).  You have to check each unit of
	   data to see if it should be shown - for instance, tricks should
	   only be shown while the hand is being played.  Another solution
	   would be to update things separately.  The disadvantage there is
	   that the code is all spread out, and it's difficult to deal with
	   multiple units of data */

	/* Note that this way depends on this function being called at the
	   proper times - every time the player is affected, and on some game
	   state changes.  Much of this is handled by the game-independent
	   code */

	/* in this "example" function, we just put in several default pieces
	   of information.  See one of the games for a more lively example */

	clear_player_message(game.players[p].seat);
	add_player_rating_message(p);
	add_player_score_message(p);
	add_player_tricks_message(p);
	add_player_bid_message(p);
	add_player_action_message(p);
}


/* Places text for the bid into the buffer.  Returns the length of the text. */
int game_get_bid_text(char *buf, size_t buf_len, bid_t bid)
{
	return snprintf(buf, buf_len, "%s", "");
}


/* This is called at the start of bidding, shortly after the hand has been
   dealt.  It corresponds closely to the other bidding functions.  The way
   bidding works is this: - We will enter the "bidding" phase right before
   this function is called.  - This function can change the phase; for
   instance you can skip bidding by jumping right to the first trick (e.g.
   hearts).  - After this, we'll just keep requesting bids until we're done.
   - For each bid, game_get_bid is called to request the bid from the client.
   Once the bid is returned, game_hand_bid is called to handle it.
   game_next_bid is called after this to prepare for the next bid.  - There
   are two game values: game.bid_count and game.bid_total. bid_count tracks
   the number of bids that have happened.  bid_total is used to check the
   total number of bids that _should_ have happened; if bid_count ever equals
   bid_total then the bidding will be automatically ended.  (note: I'm
   thinking of getting rid of bid_total since it's only used by spades.) -
   Aside from this, your own game data should be used to track what's going on 
   with the bidding. */
void game_start_bidding(void)
{
	/* by default, all players bid once */
	game.bid_total = game.num_players;
	game.next_bid = (game.dealer + 1) % game.num_players;
}


/* Asks for bid from the client/AI.  AI can be inserted here; just call
   handle_bid_event; however, this has never been tested.  It needs to work
   pretty closely with the other bidding functions. */
int game_get_bid(void)
{
	bad_game("get_bid");
	return -1;
}


/* Handle incoming bid from player.  Note that the player's bid
   (game.players[p].bid) will already have been set automatically; all we
   need to do is any additional game-specific stuff. */
void game_handle_bid(player_t p, bid_t bid)
{
	/* no special handling necessary */
	/* the bid message is set automatically */
}


/* Sets up for the next bid.  game.bid_count has already been incremented,
   and will now equal the number of the current bid. game.next_bid should
   equal the player who just bid, and should be changed to the player who
   bids next.  In games that allow more than one person to bid at once,
   game.next_bid may not be accurate! */
void game_next_bid(void)
{
	if (game.bid_count == 0)
		game.next_bid = (game.dealer + 1) % game.num_players;
	else
		game.next_bid = (game.next_bid + 1) % game.num_players;
}


/* Called between the bidding and playing sequences.  A lot of stuff is done
   automatically at this point, all we have to do is any game-specific stuff.
   This means figuring out who leads, writing out any contract messages, etc. */
void game_start_playing(void)
{
	game.trick_total = game.hand_size;
	game.play_total = game.num_players;

	game.leader = (game.dealer + 1) % game.num_players;
}


/* Here we verify that the play is legal; returning NULL if it is and an
   error message otherwise.  The error message must be statically declared!!!
   Note that we've already checked that the play is legal _in general_, here
   we only check that it's legal _for this game_. Except for games that have
   special rules (outside of those covered by game.must_overtrump and
   game.must_break_trump), no changes should be necessary. */
char *game_verify_play(player_t p, card_t card)
{
	card_t c;
	seat_t s = game.players[p].play_seat;
	int cnt;

	card = game.funcs->map_card(card);

	/* the leader has his own restrictions */
	if (game.play_count == 0) {
		/* if must_break_trump is set, you can't lead trump until
		   it's been broken.  This is the case in spades, for
		   instance. */
		/* otherwise the leader can lead anything */
		if (!game.must_break_trump)
			return NULL;
		if (card.suit != game.trump)
			return NULL;
		if (game.trump_broken)
			return NULL;
		if (cards_suit_in_hand(&game.seats[s].hand, game.trump) ==
		    game.seats[s].hand.hand_size)
			/* their entire hand is trump! */
			return NULL;
		return "Trump has not yet been broken.";
	}

	/* following suit is always okay */
	if (card.suit == game.lead_card.suit)
		return NULL;

	/* not following suit is never allowed */
	c = game.funcs->map_card(game.lead_card);
	if ((cnt = cards_suit_in_hand(&game.seats[s].hand, c.suit)))
		return "You must follow suit.";

	/* if must_overtrump is set, then you must overtrump to win if you
	   can */
	if (game.must_overtrump && game.trump >= 0 && game.trump < 4) {
		/* The player doesn't have led suit. He MUST overtrump if he
		   has a trump card higher than any trump card on the table. */
		char hi_trump, hi_trump_played;
		player_t p2;
		hi_trump = cards_highest_in_suit(&game.seats[s].hand,
						 game.trump);
		hi_trump_played = 0;
		for (p2 = 0; p2 < game.num_players; p2++) {
			c = game.seats[game.players[p2].seat].table;
			c = game.funcs->map_card(c);
			if (c.suit == game.trump && c.face > hi_trump_played)
				hi_trump_played = c.face;
		}

		/* the play is invalid if they had a higher trump than any on
		   the table AND they didn't play a higher trump than any on
		   the table */
		if (hi_trump > hi_trump_played
		    && !(card.suit == game.trump
			 && card.face > hi_trump_played))
			return "You must play a higher trump than any already played.";
	}

	/* I guess the play was okay... */
	return NULL;
}


/* Sets up for the next play note that game.play_count has already been
   incremented.  As with bidding, we track the play_count and play_total. For
   most games, you can just say game.play_total = 4 up above so that we'll
   automatically get 4 plays on each hand. */
void game_next_play(void)
{
	game.next_play = (game.next_play + 1) % game.num_players;
}


/* This gets a play.  It most likely just requests one from the player, but
   AI can be inserted to call handle_play_event.  We also handle
   game-specific stuff here (e.g. playing from the dummy hand in Bridge). */
void game_get_play(player_t p)
{
	/* in almost all cases, we just want the player to play from their
	   own hand */
	req_play(p, game.players[p].seat);
}


/* This handles a play.  Just about everything is taken care of by the
   game-independent code; all that needs to be done here is anything
   game-specific (e.g. revealing the dummy hand after the first lead in
   Bridge). */
void game_handle_play(card_t c)
{
	/* nothing needs to be done... */
}


/* Deal a new hand. */
int game_deal_hand(void)
{
	seat_t s;

	game.hand_size = cards_deck_size(game.deck) / game.num_players;
	/* in a regular deal, we just deal out hand_size cards to everyone */
	for (s = 0; s < game.num_seats; s++)
		cards_deal_hand(game.deck, game.hand_size,
				&game.seats[s].hand);
	return 0;
}


/* Figure who won the trick.  game.winner will be announced as the winner;
   game.leader will lead the next trick.  Some games (none that I know) may
   wish to do other tricky stuff here; for most games just setting
   game.trick_total = game.hand_size up above will result in all the cards
   being played out. */
void game_end_trick(void)
{
	player_t hi_player = game.leader, p_r;
	card_t hi_card = game.funcs->map_card(game.lead_card);

	/* default method of winning tricks: the winning card is the highest
	   card of the suit lead, or the highest trump if there is trump */
	for (p_r = 1; p_r < game.num_players; p_r++) {
		player_t p = (game.leader + p_r) % game.num_players;
		card_t card = game.seats[game.players[p].seat].table;
		card = game.funcs->map_card(card);
		if ((card.suit == game.trump
		     && (hi_card.suit != game.trump
			 || hi_card.face < card.face))
		    || (card.suit == hi_card.suit
			&& card.face > hi_card.face)) {
			hi_card = card;
			hi_player = p;
		}
	}

	game.players[hi_player].tricks++;
	game.leader = game.winner = hi_player;

	set_player_message(hi_player);
}


/* Calculate scores for this hand and announce. */
void game_end_hand(void)
{
	bad_game("end_hand");
}


/* Called at the beginning of a game, it initializes any necessary data. This 
   is pretty empty right now, but we don't yet play multiple games so it's
   not necessary yet anyway. */
void game_start_game(void)
{
	player_t p;

	/* TODO: initialize the game; right now we just assume everything's
	   zero which won't be true the second time around. */
	for (p = 0; p < game.num_players; p++)
		game.players[p].score = 0;
}


/* Called at the beginning of a new hand to determine if the game is over.
   Return 1 for gameover, 0 otherwise. */
int game_test_for_gameover(void)
{
	player_t p;

	/* in the default case, it's just a race toward a target score */
	for (p = 0; p < game.num_players; p++)
		if (game.players[p].score >= game.target_score)
			return 1;
	return 0;
}

/* Causes one card to behave as another in just about all situations of the
   game. */
card_t game_map_card(card_t c)
{
	return c;
}


/* The game is over and we should send out game-over message.  This function
   determines who has won and calls send_gameover. */
int game_handle_gameover(void)
{
	player_t p;
	int hi_score = -9999;
	player_t winners[game.num_players];
	int winner_cnt = 0;

	/* in the default case, just take the highest score(s) this should
	   automatically handle the case of teams! */
	for (p = 0; p < game.num_players; p++) {
		if (game.players[p].score > hi_score) {
			winner_cnt = 1;
			winners[0] = p;
			hi_score = game.players[p].score;
		} else if (game.players[p].score == hi_score) {
			winners[winner_cnt] = p;
			winner_cnt++;
		}
	}

	return send_gameover(winner_cnt, winners);
}


/* Returns -1, 0, or 1 if c1 is lower, equal, or higher than c2.  This
   function is used for the automatic sorting of hands by cards_sort_hand. */
int game_compare_cards(card_t card1, card_t card2)
{
	if (card1.suit < card2.suit)
		return -1;
	if (card1.suit > card2.suit)
		return 1;
	if (card1.face < card2.face)
		return -1;
	if (card1.face > card2.face)
		return 1;
	return 0;		/* ignore decks for now */
}


/* Show a player a hand.  This has to determine whether the hand is going to
   be revealed to the player or not.  It's called automatically after dealing 
   the hand, but may be called at other times as well. */
int game_send_hand(player_t p, seat_t s)
{
	/* in most cases, we want to reveal the hand only to the player who
	   owns it. */
	return send_hand(p, s, game.players[p].seat == s);
}
