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
#include "message.h"

struct game_function_pointers game_funcs = {
	game_init_game,
	game_get_options,
	game_handle_options,
	game_set_player_message,
	game_get_bid_text,
	game_start_bidding,
	game_get_bid,
	game_handle_bid,
	game_next_bid,
	game_start_playing,
	game_verify_play,
	game_next_play,
	game_get_play,
	game_handle_play,
	game_deal_hand,
	game_end_trick,
	game_end_hand,
	game_start_game,
	game_test_for_gameover,
	game_handle_gameover,
	game_map_card,
	game_compare_cards,
	game_send_hand
};

/* game.funcs->map_card
 *   newly implemented; it should cause one card to behave
 *   as another in just about all situations */
card_t game_map_card(card_t c)
{
	switch (game.which_game) {
		case GGZ_GAME_EUCHRE:
			if (c.face == JACK && c.suit == 3-game.trump) {
				c.suit = game.trump;
				c.face = ACE_HIGH+1;
			} else if (c.face == JACK && c.suit == game.trump)
				c.face = ACE_HIGH+2;
		case GGZ_GAME_SKAT:
			/* TODO */
		default:
			break;
	}
	return c;
}


/* game_compare_cards
 *   Returns -1, 0, or 1 if c1 is lower, equal, or higher than c2.
 *   This function is used for the automatic sorting of hands by
 *   cards_sort_hand.
 */
int game_compare_cards(const void *c1, const void *c2)
{
	register card_t card1 = game.funcs->map_card( *(card_t *)c1 );
	register card_t card2 = game.funcs->map_card( *(card_t *)c2 );
	switch (game.which_game) {
		default:
			if (card1.suit < card2.suit) return -1;
			if (card1.suit > card2.suit) return 1;
			if (card1.face < card2.face) return -1;
			if (card1.face > card2.face) return 1;
			return 0; /* ignore decks for now */
	}
}

/* game_init_game
 *   This function initializes the game soon after GGZCards is launched.
 *   This is tricky, because it really has to set up _everything_ about the
 *   game, including allocating any extra data, setting any game options or
 *   global options, and setting up all of the GGZ information so that
 *   seats and players can both be used intelligently.
 */
void game_init_game()
{
	player_t p;
	seat_t s;

	/* second round of game-specific initialization */
	switch (game.which_game) {
		case GGZ_GAME_EUCHRE:
			game.specific = alloc(sizeof(euchre_game_t));
			set_num_seats(4);
			for(p = 0; p < game.num_players; p++) {
				s = p;
				game.players[p].seat = s;
				game.seats[s].ggz = &ggz_seats[p];
			}
			game.deck_type = GGZ_DECK_EUCHRE;
			game.max_bid_choices = 5;
			game.max_bid_length = 20; /* TODO */
			game.max_hand_length = 5;
			game.target_score = 10;
			game.name = "Euchre";
			EUCHRE.maker = -1;
			game.trump = -1;
			break;
		case GGZ_GAME_SKAT:
			game.specific = alloc(sizeof(skat_game_t));
			set_num_seats(game.num_players);
			for(p = 0; p < game.num_players; p++) {
				s = p;
				game.players[p].seat = s;
				game.seats[s].ggz = &ggz_seats[p];
			}
			/* TODO */
			game.max_bid_choices = 10;
			game.max_bid_length = 10;
			game.target_score = 100;
			game.name = "Skat";
			break;
		case GGZ_GAME_LAPOCHA:
			game.specific = alloc(sizeof(lapocha_game_t));
			set_num_seats(4);
			for(p = 0; p < game.num_players; p++) {
				s = p;
				game.players[p].seat = s;
				game.seats[s].ggz = &ggz_seats[p];
			}
			game.deck_type = GGZ_DECK_LAPOCHA;
			game.max_bid_choices = 12;
			game.max_bid_length = 4;
			game.max_hand_length = 10;
			game.must_overtrump = 1;	/* in La Pocha, you *must* overtrump if you can */
			game.name = "La Pocha";
			break;
		default:  	
			ggz_debug("SERVER BUG: game_launch not implemented for game %d.", game.which_game);
			game.name = "(Unknown)";
	}
}

/* game_get_options
 *   This function manually requests options from the client; the response
 *   is handled by game_handle_options.  Options are optional; it can
 *   be left as-is for a game that has no options.
 */
int game_get_options(int fd)
{
	game.options_initted = 1; /* no options */
	return 0;
}

/* game_handle_options
 *   This handles options being sent from the client.  A value
 *   of -1 will be passed in as an option to indicate an error.
 *   It corresponds very closely to game_get_options, above;
 *   and can be left as-is for games that have no options.
 */
void game_handle_options(int *options)
{
	switch (game.which_game) {
		default:
			ggz_debug("SERVER/CLIENT bug: game_handle_options called incorrectly.");
			break;
	}
}

/* game_start_game
 *   Called at the beginning of a game, it initializes any necessary data.
 *   This is pretty empty right now, but we don't yet play multiple games
 *   so it's not necessary yet anyway.
 */
void game_start_game(void)
{
	player_t p;

	/* TODO: initialize the game; right now we just assume everything's
	 * zero which won't be true the second time around. */
	for (p=0; p<game.num_players; p++) {
		game.players[p].score = 0;
	}

	switch (game.which_game) {
		default:
			/* nothing else to do... */
	}
}

/* game_handle_gameover
 *   The game is over and we should send out game-over message.
 *   This function determines who has won and calls send_gameover.
 */
int game_handle_gameover(void)
{
	player_t p;
	int hi_score = -9999;
	player_t winners[game.num_players];
	int winner_cnt = 0;

	switch (game.which_game) {
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
		case GGZ_GAME_EUCHRE:
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
 *   This is called at the start of bidding, shortly after
 *   the hand has been dealt.  It corresponds closely to
 *   the other bidding functions.
 *   The way bidding works is this:
 *     - We will enter the "bidding" phase right before this function is called.
 *     - This function can change the phase; for instance you can skip bidding
 *       by jumping right to the first trick (e.g. hearts).
 *     - After this, we'll just keep requesting bids until we're done.
 *     - For each bid, game_get_bid is called to request the bid from the client.
 *       Once the bid is returned, game_hand_bid is called to handle it.
 *       game_next_bid is called after this to prepare for the next bid.
 *     - There are two game values: game.bid_count and game.bid_total.  bid_count
 *       tracks the number of bids that have happened.  bid_total is used to
 *       check the total number of bids that _should_ have happened; if bid_count
 *       ever equals bid_total then the bidding will be automatically ended.
 *       (note: I'm thinking of getting rid of bid_total since it's only used
 *       by spades.)
 *     - Aside from this, your own game data should be used to
 *       track what's going on with the bidding.
 */
void game_start_bidding()
{
	switch (game.which_game) {
		case GGZ_GAME_LAPOCHA:
			/* all 4 players bid once, but the first bid determines the trump */
			game.bid_total = 5;
			LAPOCHA.bid_sum = 0;
			break;
		case GGZ_GAME_EUCHRE:
			game.bid_total = 8; /* twice around, at most */
			game.next_bid = (game.dealer + 1) % game.num_players;
			break;
		default:
			/* all players bid once */
			game.bid_total = game.num_players;
			game.next_bid = (game.dealer + 1) % game.num_players;
	}
}

/* game_get_bid()
 *   asks for bid from the client/AI.
 *   AI can be inserted here; just call handle_bid_event;
 *   however, this has never been tested.  It needs to work pretty
 *   closely with the other bidding functions.
 */
/* TODO: verify that it will work with and without bots */
int game_get_bid()
{
	int status = 0;
	bid_t bid;
	bid.bid = 0;

	switch (game.which_game) {
		case GGZ_GAME_EUCHRE:
			if (game.bid_count < 4) {
				/* first four bids: either "pass" or "take" */
				bid.sbid.spec = EUCHRE_TAKE;
				game.bid_choices[0] = bid;
				bid.sbid.spec = EUCHRE_PASS;
				game.bid_choices[1] = bid;
				status = req_bid(game.next_bid, 2, NULL);
			} else {
				char suit;
				bid.sbid.spec = EUCHRE_TAKE_SUIT;
				for (suit=0; suit<4; suit++) {
					bid.sbid.suit = suit;
					game.bid_choices[(int)suit] = bid;
				}
				bid.bid = 0;
				bid.sbid.spec = EUCHRE_PASS;
				game.bid_choices[4] = bid;
				status = req_bid(game.next_bid, 5, NULL);
			}
			/* TODO: dealer's last bid */
			break;
		case GGZ_GAME_LAPOCHA:
			if (game.bid_count == 0) { /* determine the trump suit */
				/* handled just like a bid */
				if(game.hand_size != 10) {
					bid.bid = (long)cards_deal_card().suit;
					handle_bid_event(bid); /* TODO: Does this work? */
				} else
					status = req_bid(game.dealer, 4, suit_names);
			} else { /* get a player's numerical bid */
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
					num++;
				}

				status = req_bid(game.next_bid, num, NULL);
			}
			break;
		default:
			ggz_debug("SERVER BUG: game_get_bid called for unimplemented game.");
			status = -1;
	}

	return status;
}

/* game_handle_bid
 *   Handle incoming bid from player.  Note that the player's bid (i.e. game.players[p].bid)
 *   will already have been set automatically; all we need to do is any additional
 *   game-specific stuff.
 */
int game_handle_bid(bid_t bid)
{
	switch (game.which_game) {
		case GGZ_GAME_EUCHRE:
			if ( bid.sbid.spec == EUCHRE_TAKE ) {
				EUCHRE.maker = game.next_bid;
				game.trump = EUCHRE.up_card.suit;
			} else if ( bid.sbid.spec == EUCHRE_TAKE_SUIT ) {
				EUCHRE.maker = game.next_bid;
				game.trump = bid.sbid.suit;
			}
			/* bidding is ended automatically by game_next_bid */
			break;
		case GGZ_GAME_LAPOCHA:
			if (game.bid_count == 0) {
				game.trump = bid.bid;
				set_global_message("", "Trump is %s.", suit_names[(int)game.trump % 4]);
			} else
				LAPOCHA.bid_sum += bid.bid;
			break;	
		default:
 			break; /* no special handling necessary */
	}

	/* the bid message is set automatically */
	
	return 0;	
}

/* game_next_bid
 *   sets up for the next bid
 *   note that game.bid_count has already been incremented, and will now
 *   equal the number of the current bid.  game.next_bid
 *   should equal the player who just bid, and should be changed to
 *   the player who bids next.
 */
void game_next_bid()
{
	switch (game.which_game) {
		case GGZ_GAME_EUCHRE:
			if (EUCHRE.maker >= 0)
				game.bid_total = game.bid_count;
			else if (game.bid_count == 8) {
				set_global_message("", "s", "Everyone passed; redealing.");
				set_game_state( WH_STATE_NEXT_HAND );
			} else
				goto normal_order;
			break;
		case GGZ_GAME_LAPOCHA:
			if (game.bid_count == 1) {
				game.next_bid = (game.dealer + 1) % game.num_players;
				break;
			}
			goto normal_order;
		default:
normal_order:
			if (game.bid_count == 0)
				game.next_bid = (game.dealer + 1) % game.num_players;
			else
				game.next_bid = (game.next_bid + 1) % game.num_players;
	}
	
}

/* game_start_playing
 *   called between the bidding and playing sequences.  A lot of stuff is done
 *   automatically at this point, all we have to do is any game-specific stuff.
 *   This means figuring out who leads, writing out any contract messages, etc.
 */
void game_start_playing(void)
{
	player_t p;
	seat_t s;

	game.trick_total = game.hand_size;
	game.play_total = game.num_players;

	switch (game.which_game) {
		case GGZ_GAME_LAPOCHA:
			game.leader = (game.dealer + 1) % game.num_players;
			break;
		case GGZ_GAME_EUCHRE:
			/* maker is set in game_handle_bid */
			set_global_message("", "%s is the maker in %s.", ggz_seats[EUCHRE.maker].name, suit_names[(int)game.trump]);
			game.leader = (game.dealer + 1) % game.num_players;
			/* resort/resend hand - this should probably be a function in itself... */
			for(s=0; s<game.num_seats; s++) {
				cards_sort_hand( &game.seats[ s ].hand );
				for (p=0; p<game.num_players; p++)
					game.funcs->send_hand(p, s);
			}
			break;
		default:
			game.leader = (game.dealer + 1) % game.num_players;
			break;
	}
}

/* game_verify_play
 *   Here we verify that the play is legal; returning NULL if it is and an
 *   error message otherwise.  The error message must be statically declared!!!
 *   Note that we've already checked that the play is legal _in general_, here we
 *   only check that it's legal _for this game_.  Except for games that have
 *   special rules (outside of those covered by game.must_overtrump and
 *   game.must_break_trump), no changes should be necessary.
 */
char* game_verify_play(card_t card)
{
	card_t c;
	seat_t s = game.play_seat;
	int cnt;

	card = game.funcs->map_card(card);

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
	c = game.funcs->map_card( game.lead_card );
	if ( (cnt = cards_suit_in_hand(&game.seats[s].hand, c.suit)) ) {
		ggz_debug("Player %d/%s, playing from seat %d/%s, played %s when they have %d of %s.",
			  game.next_play, ggz_seats[game.next_play].name, s, game.seats[s].ggz->name, suit_names[(int)card.suit], cnt, suit_names[(int)game.lead_card.suit]);
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
			c = game.seats[ game.players[p2].seat ].table;
			c = game.funcs->map_card( c );
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

/* game_next_play
 *   sets up for the next play
 *   note that game.play_count has already been incremented.
 *   As with bidding, we track the play_count and play_total.  For
 *   most games, you can just say game.play_total = 4 up above
 *   so that we'll automatically get 4 plays on each hand.
 */
void game_next_play()
{
	game.next_play = (game.next_play + 1) % game.num_players;
}

/* game_get_play
 *   this gets a play.  It most likely just requests one from the player, but
 *   AI can be inserted to call handle_play_event.  We also handle
 *   game-specific stuff here (e.g. playing from the dummy hand in Bridge).
 */
void game_get_play(player_t p)
{
	/* in almost all cases, we just want the player to play from their own hand */
	req_play(p, game.players[p].seat);
}

/* game_handle_play
 *   this handles a play.  Just about everything is taken care of by the
 *   game-independent code; all that needs to be done here is anything
 *   game-specific (e.g. revealing the dummy hand after the first lead in
 *   Bridge).
 */
void game_handle_play(card_t c)
{
	/* nothing needs to be done... */
}

/* game_test_for_gameover
 *   called at the beginning of a new hand to determine if the game is over.
 *   Return 1 for gameover, 0 otherwise.
 */
int game_test_for_gameover()
{
	player_t p;
	switch (game.which_game) {
		case GGZ_GAME_LAPOCHA:
			return (game.hand_num == 29);
		case GGZ_GAME_EUCHRE:
		default:
			/* in the default case, it's just a race toward a target score */
			for (p = 0; p < game.num_players; p++)
				if (game.players[p].score >= game.target_score)
					return 1;
			return 0;
	}

	return 0;
}


/* game_deal_hand
 *   Deal a new hand.
 */
int game_deal_hand(void)
{
	seat_t s;
	int result=0;

	switch (game.which_game) {
		case GGZ_GAME_LAPOCHA:
			{
			/* The number of cards dealt each hand; it's
			 * easier just to write it out than use any kind of function */
			int lap_card_count[] = { 1, 1, 1, 1,
					2, 3, 4, 5, 6, 7, 8, 9,
					10, 10, 10, 10,
					9, 8, 7, 6, 5, 4, 3, 2,
					1, 1, 1, 1 };
			/* in la pocha, there are a predetermined number of cards
			 * each hand, as defined by lap_card_count above */
			game.hand_size = lap_card_count[game.hand_num];
			game.trump = -1; /* must be determined later */ /* TODO: shouldn't go here */
			}
			goto regular_deal;
		case GGZ_GAME_EUCHRE:
			/* in Euchre, players 0-3 (seats 0, 1, 3, 4) get 5 cards each.
			 * the up-card (seat 5) gets one card, and the kitty (seat 2)
			 * gets the other 3. */
			cards_deal_hand(1, &game.seats[0].hand);
			EUCHRE.up_card = game.seats[0].hand.cards[0];
			set_global_message("", "The up-card is the %s of %s.",
				face_names[(int)EUCHRE.up_card.face], suit_names[(int)EUCHRE.up_card.suit]);
			game.hand_size = 5;
			goto regular_deal;
		default:
			game.hand_size = 52 / game.num_players;
regular_deal:
			/* in a regular deal, we just deal out hand_size cards to everyone */
			for (s = 0; s < game.num_seats; s++)
				cards_deal_hand(game.hand_size, &game.seats[s].hand);
	}

	return result;
}


/* game_send_hand
 *   Show a player a hand.  This has to determine whether the hand is going to
 *   be revealed to the player or not.  It's called automatically after dealing
 *   the hand, but may be called at other times as well.
 */
int game_send_hand(player_t p, seat_t s)
{
	switch (game.which_game) {
		case GGZ_GAME_EUCHRE:
			/* reveal the up-card */
			return send_hand(p, s, game.players[p].seat == s || s == 5);
		default:
			/* in most cases, we want to reveal the hand only to the player
			 * who owns it. */
			return send_hand(p, s, game.players[p].seat == s);
	}
	return -1;
}

/* game_get_bid_text
 *   places text for the bid into the buffer.  Returns the length of the text (from snprintf).
 */
int game_get_bid_text(char* buf, int buf_len, bid_t bid)
{
	/* TODO: in case of an overflow, the result from snprintf probably isn't what we want to return. */
	switch (game.which_game) {
		case GGZ_GAME_EUCHRE:
			if (bid.sbid.spec == EUCHRE_PASS) return snprintf(buf, buf_len, "Pass");
			if (bid.sbid.spec == EUCHRE_TAKE) return snprintf(buf, buf_len, "Take");
			if (bid.sbid.spec == EUCHRE_TAKE_SUIT) return snprintf(buf, buf_len, "Take at %s", suit_names[(int)bid.sbid.suit]);
			break;
		case GGZ_GAME_LAPOCHA:
			return snprintf(buf, buf_len, "%d", (int)bid.bid);
		default:
			/* nothing... */
	}
	return snprintf(buf, buf_len, "%s", "");
}

/* game_set_player_message
 *   sets the player message for a given player.
 */
void game_set_player_message(player_t p)
{
	seat_t s = game.players[p].seat;
	char* message = game.seats[s].message;
	int len = 0;

	ggz_debug("Setting player %d/%s's message.", p, ggz_seats[p].name);

	/* This function is tricky.  The problem is that we're trying to assemble a single player string out of
	 * multiple units of data - score, bid, tricks, etc.  The solution here is to integrate these all into one
	 * function (this one).  The problem is that you have to check each unit of data to see if it should be
	 * shown - for instance, tricks should only be shown while the hand is being played.  Another solution
	 * would be to update things separately.  The disadvantage there is that the code is all spread out, and
	 * it's difficult to deal with multiple units of data */

	/* Note that this way depends on this function being called at the proper times -
	 * i.e. every time the player is affected, and on some game state changes.  Much
	 * of this is handled by the game-independent code */

	/* did I mention this was really ugly?  It could be much worse... */
#define REGULAR_SCORE_MESSAGE	len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Score: %d\n", game.players[p].score)
#define REGULAR_TRICKS_MESSAGE	if (game.state == WH_STATE_WAIT_FOR_PLAY || game.state == WH_STATE_NEXT_TRICK || game.state == WH_STATE_NEXT_PLAY) \
					len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Tricks: %d\n", game.players[p].tricks)
#define REGULAR_BID_MESSAGE	if (game.state == WH_STATE_NEXT_BID || game.state == WH_STATE_WAIT_FOR_BID) { \
					char bid_text[game.max_bid_length]; \
					game.funcs->get_bid_text(bid_text, game.max_bid_length, game.players[p].bid); \
					if (*bid_text) len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Bid: %s\n", bid_text); \
				}
#define REGULAR_BIDDING_MESSAGE	if (game.state == WH_STATE_WAIT_FOR_BID && p == game.next_bid) \
					len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Bidding...") /* "Waiting for bid" won't fit */	
#define REGULAR_PLAYING_MESSAGE	if (game.state == WH_STATE_WAIT_FOR_PLAY && p == game.curr_play) \
					len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Playing...") /* "Waiting for play" won't fit */
#define REGULAR_ACTION_MESSAGES REGULAR_BIDDING_MESSAGE; REGULAR_PLAYING_MESSAGE

	/* anyway, it's really ugly, but I don't see a better way... */

	switch (game.which_game) {
		case GGZ_GAME_EUCHRE:
			REGULAR_SCORE_MESSAGE;
			if (game.state == WH_STATE_FIRST_BID || game.state == WH_STATE_NEXT_BID || game.state == WH_STATE_WAIT_FOR_BID) {
				if (p == game.dealer)
					len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "dealer\n");
			} else
				if (p == EUCHRE.maker)
					len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "maker\n");
			if (game.state == WH_STATE_WAIT_FOR_PLAY || game.state == WH_STATE_NEXT_TRICK || game.state == WH_STATE_NEXT_PLAY)
				len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Tricks: %d\n", game.players[p].tricks + game.players[(p+2)%4].tricks);
			REGULAR_ACTION_MESSAGES;
			break;
		case GGZ_GAME_LAPOCHA:
			REGULAR_SCORE_MESSAGE;
			if (p == game.dealer)
				len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "dealer\n");
			if (game.state >= WH_STATE_FIRST_TRICK && game.state <= WH_STATE_WAIT_FOR_PLAY) {
				len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Contract: %d\n", (int)game.players[p].bid.bid);
			}
			REGULAR_TRICKS_MESSAGE;
			REGULAR_BID_MESSAGE;
			REGULAR_ACTION_MESSAGES;
			break;
		default:
			len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Score: %d\n", game.players[p].score);
			if (game.state == WH_STATE_WAIT_FOR_PLAY || game.state == WH_STATE_NEXT_TRICK || game.state == WH_STATE_NEXT_PLAY)
					len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Tricks: %d\n", game.players[p].tricks);
			if (game.state == WH_STATE_NEXT_BID || game.state == WH_STATE_WAIT_FOR_BID) {
					char bid_text[game.max_bid_length];
					game.funcs->get_bid_text(bid_text, game.max_bid_length, game.players[p].bid);
					if (*bid_text) len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Bid: %s\n", bid_text);
				}
			if (game.state == WH_STATE_WAIT_FOR_BID && p == game.next_bid)
					len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Bidding...");
			if (game.state == WH_STATE_WAIT_FOR_PLAY && p == game.curr_play) \
					len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Playing...");
			break;
	}

#undef REGULAR_SCORE_MESSAGE
#undef REGULAR_TRICKS_MESSAGE
#undef REGULAR_BID_MESSAGE
#undef REGULAR_BIDDING_MESSAGE
#undef REGULAR_PLAYING_MESSAGE
}

/* game_end_trick
 *   Figure who won the trick.
 *   game.winner will be announced as the winner; game.leader
 *   will lead the next trick.
 *   Some games (none that I know) may wish to do other tricky stuff here;
 *   for most games just setting game.trick_total = game.hand_size up above will
 *   result in all the cards being played out.
 */
void game_end_trick(void)
{
	player_t hi_player = game.leader, p, lo_player = game.leader;
	card_t hi_card = game.lead_card, lo_card = game.lead_card;

	/* default method of winning tricks: the winning card is the highest
	 * card of the suit lead, or the highest trump if there is trump */
	for (p=0; p<game.num_players; p++) {
		card_t card = game.seats[ game.players[p].seat ].table;
		card = game.funcs->map_card( card );
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
		case GGZ_GAME_EUCHRE:	
			/* TODO: handle out-of-order cards */
			break;
		case GGZ_GAME_LAPOCHA:
		default:
			/* no additional scoring is necessary */
			break;
	}

	game.players[hi_player].tricks++;
	game.leader = game.winner = hi_player;

	set_player_message(hi_player);

	/* update teammate's info, if necessary */
	switch (game.which_game) {
		case GGZ_GAME_EUCHRE:
			/* update teammate's info as well */
			set_player_message((game.winner+2)%4);
			break;
		case GGZ_GAME_LAPOCHA:
		default:
			break;
	}
}


/* game_end_hand
 *   Calculate scores for this hand and announce.
 */
void game_end_hand(void)
{
	player_t p;

	switch (game.which_game) {
		case GGZ_GAME_LAPOCHA:
			for(p=0; p<game.num_players; p++) {
				ggz_debug("Player %d/%s got %d tricks on a bid of %d", p, ggz_seats[p].name,
					  game.players[p].tricks, (int)game.players[p].bid.bid);
				if(game.players[p].tricks == game.players[p].bid.bid)
					game.players[p].score += 10 + (5 * game.players[p].bid.bid);
				else
					game.players[p].score -= 5 * game.players[p].bid.bid;
			}
			break;
		case GGZ_GAME_EUCHRE:
			{
			int tricks, winning_team;
			tricks = game.players[EUCHRE.maker].tricks + game.players[(EUCHRE.maker+2)%4].tricks;
			if (tricks >= 3)
				winning_team = EUCHRE.maker % 2;
			else
				winning_team = (EUCHRE.maker + 1) % 2;
			/* TODO: point values other than 1 */
			game.players[winning_team].score += 1;
			game.players[winning_team+2].score += 1;
			EUCHRE.maker = -1;
			game.trump = -1;
			}
			break;
		default:
			ggz_debug("SERVER not implemented: game_end_hand for game %d.", game.which_game);
			break;
	}                                                         	
}


