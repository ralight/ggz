/* 
 * File: client.c
 * Author: Jason Short
 * Project: GGZCards Client-Common
 * Date: 07/22/2001 (as common.c)
 * Desc: Backend to GGZCards Client-Common
 * $Id: client.c 8443 2006-08-01 16:40:16Z jdorje $
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
#  include <config.h>
#endif

#ifndef AI_CLIENT
#  define GUI_CLIENT
#endif

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>	/* for time() */
#include <unistd.h>	/* for close() */

#include <ggz.h>
#ifdef GUI_CLIENT
# include <ggzmod.h>
#endif

#include "net_common.h"
#include "protocol.h"
#include "shared.h"

#include "client.h"

static void handle_server_connect(int server_fd);

static void handle_message_global(void);

static void handle_text_message(void);
static void handle_player_message(void);
static void handle_cardlist_message(void);
static void handle_game_message(void);

static struct {
	GGZDataIO *dio;
#ifdef GUI_CLIENT
	GGZMod *ggzmod;
#endif				/* GUI_CLIENT */
	int max_hand_size;
} game_internal = {
	0,
#ifdef GUI_CLIENT
	    NULL,
#endif
0};

struct ggzcards_game_t ggzcards = { 0 };

static void handle_req_play(void);

#ifdef GUI_CLIENT
GGZMod *client_get_ggzmod(void)
{
	return game_internal.ggzmod;
}


static void handle_ggzmod_server(GGZMod * ggzmod, GGZModEvent e,
				 const void *data)
{
	const int *fd = data;

	ggzmod_set_state(ggzmod, GGZMOD_STATE_PLAYING);
	handle_server_connect(*fd);
}
#endif

static void handle_server_connect(int server_fd)
{
	game_internal.dio = ggz_dio_new(server_fd);

	/* HACK: Auto flush cuts down on packets being split up in transit. */
	ggz_dio_set_auto_flush(game_internal.dio, TRUE);

	client_send_language(getenv("LANG"));

	game_alert_server(game_internal.dio);	/* ?? */
}

int client_initialize(void)
{
	/* A word on memory management: the client-common code uses ggz for
	   internal memory management; i.e. ggz_malloc+ggz_free. Anything
	   allocated with ggz_dio will be allocated with malloc and must be
	   freed with free.  The table code (which is gui-specific) may use
	   whatever memory management routines it wants (currently the GTK
	   client uses g_malloc and g_free). This may be unnecessarily
	   complicated, but remember that the internal client-common
	   variables are always kept separate from the GUI variables, so
	   there should be no confusion there. And all of the
	   ggz_dio-allocated variables are labelled. */

	srand((unsigned)time(NULL));

	game_internal.max_hand_size = 0;

	ggzcards.state = STATE_INIT;
	ggz_debug(DBG_CLIENT, "Client initialized.");

#ifdef GUI_CLIENT
	game_internal.ggzmod = ggzmod_new(GGZMOD_GAME);
	ggzmod_set_handler(game_internal.ggzmod,
			   GGZMOD_EVENT_SERVER, &handle_ggzmod_server);
	ggzmod_connect(game_internal.ggzmod);
#else /* AI_CLIENT */
	handle_server_connect(3);
#endif

#ifdef GUI_CLIENT
	return ggzmod_get_fd(game_internal.ggzmod);
#else
	return -1;
#endif
}


void client_quit(void)
{
	int p;

	/* Disconnect */
#ifdef GUI_CLIENT
	if (ggzmod_disconnect(game_internal.ggzmod) < 0)
#else /* AI_CLIENT */
	if (close(ggz_dio_get_socket(game_internal.dio)) < 0)
#endif
		ggz_error_msg_exit("Couldn't disconnect from ggz.");
	ggz_dio_free(game_internal.dio);

	/* Free data */
	for (p = 0; p < ggzcards.num_players; p++) {
		if (ggzcards.players[p].hand.cards)
			ggz_free(ggzcards.players[p].hand.cards);
		if (ggzcards.players[p].name)
			ggz_free(ggzcards.players[p].name);

	}
	if (ggzcards.players != NULL)
		ggz_free(ggzcards.players);
	ggzcards.players = NULL;

	ggz_debug(DBG_CLIENT, "Client disconnected.");
}


GGZDataIO *client_get_dio(void)
{
	return game_internal.dio;
}


static const char *get_state_name(client_state_t state)
{
	/* A switch statement can be used so that if the ordering changes
	   it'll still work. */
	switch (state) {
	case STATE_INIT:
		return "INIT";
	case STATE_WAIT:
		return "WAIT";
	case STATE_PLAY:
		return "PLAY";
	case STATE_BID:
		return "BID";
	case STATE_DONE:
		return "DONE";
	case STATE_OPTIONS:
		return "OPTIONS";
	}
	return "[unknown state]";
}


static void set_game_state(client_state_t state)
{
	if (state == ggzcards.state) {
		ggz_error_msg("Staying in state %d.", ggzcards.state);
	} else {
		ggz_debug(DBG_CLIENT, "Changing state from %s to %s.",
			  get_state_name(ggzcards.state),
			  get_state_name(state));
		ggzcards.state = state;
	}
}

static void handle_text_message(void)
{
	char *message, *mark;

	ggz_dio_get_string_alloc(game_internal.dio, &mark);
	ggz_dio_get_string_alloc(game_internal.dio, &message);

	game_set_text_message(mark, message);
	ggz_free(message);	/* allocated by ggz_dio */
	ggz_free(mark);	/* allocated by ggz_dio */
}

static void handle_cardlist_message(void)
{
	int p, i;
	card_t **cardlist =
	    ggz_malloc(ggzcards.num_players * sizeof(*cardlist));
	int *lengths = ggz_malloc(ggzcards.num_players * sizeof(*lengths));
	char *mark;

	if (!cardlist || !lengths)
		abort();

	ggz_dio_get_string_alloc(game_internal.dio, &mark);

	for (p = 0; p < ggzcards.num_players; p++) {
		ggz_dio_get_int(game_internal.dio, &lengths[p]);
		if (lengths[p] > 0)
			cardlist[p] = ggz_malloc(lengths[p]
						 * sizeof(**cardlist));
		for (i = 0; i < lengths[p]; i++)
			read_card(game_internal.dio, &cardlist[p][i]);
	}

	game_set_cardlist_message(mark, lengths, cardlist);

	for (p = 0; p < ggzcards.num_players; p++)
		if (lengths[p] > 0)
			ggz_free(cardlist[p]);
	ggz_free(cardlist);
	ggz_free(lengths);
	ggz_free(mark);	/* allocated by ggz_dio */
}

/* A message_player message tells you one "player message", which is
   displayed by the client. */
static void handle_player_message(void)
{
	int p;
	char *message;

	read_seat(game_internal.dio, &p);
	ggz_dio_get_string_alloc(game_internal.dio, &message);

	assert(p >= 0 && p < ggzcards.num_players);

	game_set_player_message(p, message);

	ggz_free(message);	/* allocated by ggz_dio */
}

/* This handles a game-specific message.  We pass the game all the
   information about the message and let them read it in from the server
   themselves.  If they don't we do it manually just to get it out of the
   way. */
/* One alternative would be to read the data in ourselves (the server would
   have to send the data format, etc) and allow the game to query us about
   what data we read.  This would be safer but a lot more work.  Or, if we
   used an XML protocol, things could just sort-of take care of themselves
   because we'd just skip over the tag automatically if it wasn't handled (I
   think). */
static void handle_game_message(void)
{
	int size;
	char *game;

	ggz_dio_get_string_alloc(game_internal.dio, &game);
	ggz_dio_get_int(game_internal.dio, &size);	/* Obsolete. */

	/* Note: "size" refers to the size of the data block, not including
	   the headers above. */
	ggz_debug(DBG_CLIENT,
		  "Received game message of size %d for game %s.", size,
		  game);

	game_handle_game_message(game_internal.dio, game);

	ggz_free(game);	/* allocated by ggz_dio */
}

/* a message_global message tells you one "global message", which is
   displayed by the client. */
static void handle_message_global(void)
{
	int opcode;
	game_message_t op;

	read_opcode(game_internal.dio, &opcode);

	op = opcode;

	ggz_debug(DBG_CLIENT, "Received %s global message opcode.",
		  get_game_message_name(op));

	switch (op) {
	case GAME_MESSAGE_TEXT:
		handle_text_message();
		break;
	case GAME_MESSAGE_CARDLIST:
		handle_cardlist_message();
		break;
	case GAME_MESSAGE_GAME:
		handle_game_message();
		break;
	case GAME_MESSAGE_PLAYER:
		handle_player_message();
		break;
	}
}


static void handle_msg_newgame(void)
{
	int cardset;
	cardset_type_t cardset_type;

	ggz_dio_get_int(game_internal.dio, &cardset);

	cardset_type = cardset;

	assert(cardset_type != UNKNOWN_CARDSET);
	set_cardset_type(cardset_type);
	game_alert_newgame(cardset_type);
}


/* A gameover message tells you the game is over, and who won. */
static void handle_msg_gameover(void)
{
	int num_winners, i, *winners = NULL;

	ggz_dio_get_int(game_internal.dio, &num_winners);
	assert(num_winners >= 0 && num_winners <= ggzcards.num_players);

	if (num_winners > 0)
		winners = ggz_malloc(num_winners * sizeof(*winners));

	for (i = 0; i < num_winners; i++)
		read_seat(game_internal.dio, &winners[i]);

	game_handle_gameover(num_winners, winners);

	set_game_state(STATE_DONE);

	if (winners)
		ggz_free(winners);
}


/* A players message tells you all the players (well, seats really) at the
   table. */
static void handle_msg_players(void)
{
	int i, numplayers, different;
	int old_numplayers = ggzcards.num_players;

	/* It is possible to have 0 players.  At the begginning of a
	   "general" game, you don't know how many seats will be used yet so
	   the number of players is 0. */
	ggz_dio_get_int(game_internal.dio, &numplayers);

	assert(numplayers >= 0);

	/* we may need to allocate memory for the players */
	different = (ggzcards.num_players != numplayers);

	/* reallocate the players, if necessary */
	if (different) {
		int p;
		if (ggzcards.players) {
			for (p = 0; p < ggzcards.num_players; p++)
				if (ggzcards.players[p].hand.cards)
					ggz_free(ggzcards.players[p].hand.
						 cards);
			ggz_free(ggzcards.players);
		}
		ggz_debug(DBG_CLIENT,
			  "get_players: (re)allocating ggzcards.players.");
		ggzcards.players =
		    ggz_malloc(numplayers * sizeof(*ggzcards.players));
		for (p = 0; p < numplayers; p++) {
			/* At least for table_card, this initialization is
			   very important. */
			ggzcards.players[p].table_card = UNKNOWN_CARD;
			ggzcards.players[p].name = NULL;
			ggzcards.players[p].hand.hand_size = 0;
			ggzcards.players[p].hand.cards = NULL;
		}

		/* this forces reallocating later */
		game_internal.max_hand_size = 0;
	}

	/* TODO: support for changing the number of players */

	/* read in data about the players */
	for (i = 0; i < numplayers; i++) {
		int type, ggzseat, team;
		GGZSeatType old_type, new_type;
		char *old_name, *new_name;

		ggz_dio_get_int(game_internal.dio, &type);
		ggz_dio_get_string_alloc(game_internal.dio, &new_name);
		ggz_dio_get_int(game_internal.dio, &ggzseat);
		ggz_dio_get_int(game_internal.dio, &team);

		new_type = type;

		old_name = ggzcards.players[i].name;
		old_type = ggzcards.players[i].status;

		ggzcards.players[i].status = new_type;
		ggzcards.players[i].name = new_name;
		ggzcards.players[i].ggzseat = ggzseat;
		ggzcards.players[i].team = team;

		if (old_type != new_type
		    || !old_name || strcmp(old_name, new_name) != 0)
			game_alert_player(i, old_type, old_name);

		if (old_name) {
			/* allocated by ggz_dio */
			ggz_free(old_name);
		}
	}

	ggzcards.num_players = numplayers;

	/* Redesign the table, if necessary. */
	if (different) {
		assert(numplayers != old_numplayers);
		game_alert_num_players(numplayers, old_numplayers);
	}

	/* TODO: should we need to enter a waiting state if players leave? */
}

/* Possibly increase the maximum hand size we can sustain. */
static void increase_max_hand_size(int max_hand_size)
{
	int p;

	if (max_hand_size <= game_internal.max_hand_size)	/* no problem 
								 */
		return;

	ggz_debug(DBG_CLIENT,
		  "Expanding max_hand_size to allow for %d cards"
		  " (previously max was %d).", max_hand_size,
		  game_internal.max_hand_size);
	game_internal.max_hand_size = max_hand_size;

	/* Let the table know how big a hand might be. */
	game_alert_hand_size(game_internal.max_hand_size);

	for (p = 0; p < ggzcards.num_players; p++) {
		/* This reallocates the hand to be larger, but leaves the
		   unused cards uninitialized.  This should be acceptable. */
		ggzcards.players[p].hand.cards =
		    ggz_realloc(ggzcards.players[p].hand.cards,
				game_internal.max_hand_size *
				sizeof(*ggzcards.players[p].hand.cards));
		ggzcards.players[p].u_hand =
		    ggz_realloc(ggzcards.players[p].u_hand,
				game_internal.max_hand_size
				* sizeof(*ggzcards.players[p].u_hand));
	}
}

/* A hand message tells you all the cards in one player's hand. */
static void handle_msg_hand(void)
{
	int player, hand_size, i;
	hand_t *hand;

	assert(ggzcards.players);

	/* first read the player whose hand it is */
	read_seat(game_internal.dio, &player);

	assert(player >= 0 && player < ggzcards.num_players);

	/* Find out how many cards in this hand */
	ggz_dio_get_int(game_internal.dio, &hand_size);

	/* Reallocate hand structures, if necessary */
	increase_max_hand_size(hand_size);

	/* Read in all the card values.  It's important that we don't change
	   anything before here so that any functions we call from
	   increase_max_hand_size won't have inconsistent data. */
	hand = &ggzcards.players[player].hand;
	hand->hand_size = hand_size;
	ggzcards.players[player].u_hand_size = hand_size;
	for (i = 0; i < hand->hand_size; i++) {
		card_t card;
		read_card(game_internal.dio, &card);

		hand->cards[i] = card;

		ggzcards.players[player].u_hand[i].card = card;
		ggzcards.players[player].u_hand[i].is_valid = TRUE;
	}

	ggz_debug(DBG_CLIENT,
		  "Received hand message for player %d; %d cards.", player,
		  hand->hand_size);

	/* Finally, show the hand. */
	game_display_hand(player);
}


/* A bid request asks you to pick from a given list of bids. */
static void handle_req_bid(void)
{
	int i;
	int possible_bids;
	char **bid_texts;
	char **bid_descs;
	bid_t *bid_choices;

	if (ggzcards.state == STATE_BID) {
		/* The new bid request overrides the old one.  But this means
		   some messy cleanup is necessary. */
		ggz_debug(DBG_CLIENT,
			  "WARNING: new bid message overriding old one.");
	}

	/* Determine the number of bidding choices we have */
	ggz_dio_get_int(game_internal.dio, &possible_bids);

	bid_choices = ggz_malloc(possible_bids * sizeof(*bid_choices));
	bid_texts = ggz_malloc(possible_bids * sizeof(*bid_texts));
	bid_descs = ggz_malloc(possible_bids * sizeof(*bid_descs));

	/* Read in all of the bidding choices. */
	for (i = 0; i < possible_bids; i++) {
		read_bid(game_internal.dio, &bid_choices[i]);
		ggz_dio_get_string_alloc(game_internal.dio, &bid_texts[i]);
		ggz_dio_get_string_alloc(game_internal.dio, &bid_descs[i]);
	}

	/* Get the bid */
	set_game_state(STATE_BID);
	game_get_bid(possible_bids, bid_choices, bid_texts, bid_descs);

	/* Clean up */
	for (i = 0; i < possible_bids; i++) {
		ggz_free(bid_texts[i]);	/* allocated by ggz_dio */
		ggz_free(bid_descs[i]);	/* allocated by ggz_dio */
	}
	ggz_free(bid_choices);
	ggz_free(bid_texts);
	ggz_free(bid_descs);
}


static void handle_msg_bid(void)
{
	bid_t bid;
	int bidder;

	read_seat(game_internal.dio, &bidder);
	read_bid(game_internal.dio, &bid);

	game_alert_bid(bidder, bid);
}


/* A play request asks you to play a card from any hand (most likely your
   own). */
static void handle_req_play(void)
{
	int num_valid_cards, i;
	card_t *valid_cards;

	/* Determine which hand we're supposed to be playing from. */
	read_seat(game_internal.dio, &ggzcards.play_hand);
	ggz_dio_get_int(game_internal.dio, &num_valid_cards);

	valid_cards = ggz_malloc(num_valid_cards * sizeof(*valid_cards));
	for (i = 0; i < num_valid_cards; i++)
		read_card(game_internal.dio, &valid_cards[i]);

	assert(ggzcards.play_hand >= 0
	       && ggzcards.play_hand < ggzcards.num_players);

	/* Get the play. */
	set_game_state(STATE_PLAY);
	game_get_play(ggzcards.play_hand, num_valid_cards, valid_cards);

	ggz_free(valid_cards);
}


/* A badplay message indicates an invalid play, and requests a new one. */
static void handle_msg_badplay(void)
{
	char *err_msg;

	/* Read the error message for the bad play. */
	ggz_dio_get_string_alloc(game_internal.dio, &err_msg);

	/* Get a new play. */
	set_game_state(STATE_PLAY);
	game_alert_badplay(err_msg);

	/* Clean up. */
	ggz_free(err_msg);	/* allocated by ggz_dio */
}


/* returns an index into the hand's card for a match to the given card */
static int match_card(card_t card, hand_t * hand)
{
	int tc, matches = -1, match = -1;
	/* Anything "unknown" will match, as will the card itself.  However,
	   we look for the greatest possible number of matches for the card. */
	/**
	 * Consider the following case:
	 * hand contains: (1, 2, 3) and (-1, -1, -1)
	 * card to match: (1, 2, 3)
	 * it matches (-1, -1, -1) so we go with that match.
	 * later, card (2, 3, 4) becomes unmatchable
	 * (1, 2, 3) was obviously the better match.
	 */
	/* TODO: It might be better to return -1 if there's more than one
	   possible match; that way we can get a re-sync and fix any
	   potential problems before they bother the player.  OTOH, then my
	   cool "squeeze" comment below would no longer apply! */
	for (tc = hand->hand_size - 1; tc >= 0; tc--) {
		/* TODO: look for a stronger match */
		card_t hcard = hand->cards[tc];
		int tc_matches = 0;

		if ((hcard.deck != -1 && hcard.deck != card.deck) ||
		    (hcard.suit != -1 && hcard.suit != card.suit) ||
		    (hcard.face != -1 && hcard.face != card.face))
			continue;

		/* we count the number of matches to make the best pick for
		   the match */
		if (hcard.deck != -1)
			tc_matches++;
		if (hcard.suit != -1)
			tc_matches++;
		if (hcard.face != -1)
			tc_matches++;

		/* If we have a greater number of matches, we should always
		   substitute.  If we have an equal number of matches, in
		   theory we should pick a match at random.  This algorithm
		   is an approximation of that; in the case where all matches
		   are equally good it will be a random pick. */
		if (tc_matches > matches
		    || (tc_matches == matches
			&& (rand() % (hand->hand_size + 1 - tc)) == 0)) {
			matches = tc_matches;
			match = tc;
		}
	}
	return match;
}


/* A play message tells of a play from a hand to the table. */
static void handle_msg_play(void)
{
	int p, c, tc, card_pos, card_pos_2;
	card_t card;
	hand_t *hand;

	/* Read the card being played. */
	read_seat(game_internal.dio, &p);
	read_card(game_internal.dio, &card);

	assert(p >= 0 && p < ggzcards.num_players);

	/* Reset the play_hand, just to be safe. */
	if (p == ggzcards.play_hand) {
		/* We assume that if the hand the card came from is the hand
		   we're playing from, that it's our play.  Thus there is
		   an implicit assumption that two players can't simultaneously
		   play from the same hand. */
		ggzcards.play_hand = -1;
	} else {
		/* Either we're not playing, or someone else played during
		   our play.  Both are possible. */
	}

	/* Place the card on the table.  Note, this contradicts what the
	   table code does, since that runs animation that may assume the
	   card has not yet been placed on the table. Nonetheless, this is
	   the correct thing to do - it is the table code that needs to be
	   fixed (to keep separate track of what cards are being _drawn_ on
	   the table, as opposed to what cards are _supposed to be_ on the
	   table which is what this is). */
	ggzcards.players[p].table_card = card;

	/* Find the hand the card is to be removed from. */
	assert(ggzcards.players);
	hand = &ggzcards.players[p].hand;
	assert(hand->cards);

	/* Find a matching card to remove. */
	card_pos = match_card(card, hand);

	/* Handle a bad match. */
	if (card_pos < 0 || card_pos >= hand->hand_size) {
		/* This is theoretically possible even without errors! In
		   fact, a clever server could _force_ us to pick wrong.
		   Figure out how and you'll be ready for a "squeeze" play!
		   Fortunately, it's easily solved. */
		ggz_debug(DBG_CLIENT,
			  "Whoa!  We can't find a match for the card.  That's strange.");
		(void)client_send_sync_request();
	}

	/* Remove the card.  This is a bit inefficient. It's also
	   tricky, so be careful not to go off-by-one and overrun
	   the buffer! */
	hand->hand_size--;
	for (c = card_pos; c < hand->hand_size; c++)
		hand->cards[c] = hand->cards[c + 1];
	hand->cards[hand->hand_size] = UNKNOWN_CARD;

	/* Remove the card just by marking its meta category to FALSE.
	   Note that we don't decrease hand_size in this case! */
	for (card_pos_2 = 0, tc = card_pos;; card_pos_2++) {
		if (!ggzcards.players[p].u_hand[card_pos_2].is_valid)
			continue;
		if (tc <= 0)
			break;
		tc--;
	}
	ggzcards.players[p].u_hand[card_pos_2].is_valid = FALSE;

	/* Update the graphics */
	game_alert_play(p, card, card_pos, card_pos_2);
}


/* A table message tells you all the cards on the table.  Each player only
   gets one card. */
static void handle_msg_table(void)
{
	int p;

	ggz_debug(DBG_CLIENT, "Handling table message.");

	assert(ggzcards.players);
	for (p = 0; p < ggzcards.num_players; p++) {
		card_t card;
		read_card(game_internal.dio, &card);
		ggzcards.players[p].table_card = card;
	}

	/* TODO: verify that the table cards have been removed from the hands 
	 */

	game_alert_table();
}


/* A trick message tells you about the end of a trick (and who won). */
static void handle_msg_trick(void)
{
	int winner, p;

	/* Read the trick winner */
	read_seat(game_internal.dio, &winner);

	assert(winner >= 0 && winner < ggzcards.num_players);

	/* Clear all cards off the table. */
	for (p = 0; p < ggzcards.num_players; p++)
		ggzcards.players[p].table_card = UNKNOWN_CARD;

	/* Update the graphics. */
	game_alert_trick(winner);
}


/* An options request asks you to pick a set of options.  Each "option" gives
   a list of choices so that you pick one choice for each option.  An option
   with only one choice is a special case: a boolean option. */
static void handle_req_options(void)
{
	int i, j;
	int option_cnt;		/* the number of options */
	char **types;		/* Types for each option. */
	char **descs;		/* Descriptive texts for the options. */
	int *choice_cnt;	/* The number of choices for each option */
	int *defaults;		/* What option choice is currently chosen for
				   each option */
	char ***option_choices;	/* The texts for each option choice of each
				   option */

	if (ggzcards.state == STATE_OPTIONS) {
		/* The new options request overrides the old one.  But this
		   means some messy cleanup is necessary. */
		ggz_debug(DBG_CLIENT,
			  "WARNING: new options request overriding old one.");
	}

	/* Read the number of options. */
	ggz_dio_get_int(game_internal.dio, &option_cnt);

	assert(option_cnt > 0);

	/* Allocate all data */
	types = ggz_malloc(option_cnt * sizeof(*types));
	descs = ggz_malloc(option_cnt * sizeof(*descs));
	choice_cnt = ggz_malloc(option_cnt * sizeof(*choice_cnt));
	defaults = ggz_malloc(option_cnt * sizeof(*defaults));
	option_choices = ggz_malloc(option_cnt * sizeof(*option_choices));

	/* Read all the options, their defaults, and the possible choices. */
	for (i = 0; i < option_cnt; i++) {
		ggz_dio_get_string_alloc(game_internal.dio, &types[i]);
		ggz_dio_get_string_alloc(game_internal.dio, &descs[i]);
		ggz_dio_get_int(game_internal.dio, &choice_cnt[i]);
		ggz_dio_get_int(game_internal.dio, &defaults[i]);
		option_choices[i] =
		    ggz_malloc(choice_cnt[i] * sizeof(**option_choices));
		for (j = 0; j < choice_cnt[i]; j++) {
			ggz_dio_get_string_alloc(game_internal.dio,
						 &option_choices[i][j]);
		}
	}

	/* Get the options. */
	set_game_state(STATE_OPTIONS);
	if (game_get_options(option_cnt, types, descs, choice_cnt,
			     defaults, option_choices) < 0) {
		(void)client_send_options(option_cnt, defaults);
	}

	/* Clean up. */
	for (i = 0; i < option_cnt; i++) {
		for (j = 0; j < choice_cnt[i]; j++) {
			/* allocated by ggz_dio */
			ggz_free(option_choices[i][j]);
		}
		ggz_free(option_choices[i]);
		ggz_free(types[i]);
		ggz_free(descs[i]);
	}
	ggz_free(defaults);
	ggz_free(option_choices);
	ggz_free(choice_cnt);
	ggz_free(descs);
	ggz_free(types);
}


/* The language lets the server translate messages for us. */
void client_send_language(const char *lang)
{
	if (!lang) {
		lang = "";
	}

	ggz_debug(DBG_CLIENT, "Sending language %s to the server.", lang);

	ggz_dio_packet_start(game_internal.dio);
	write_opcode(game_internal.dio, MSG_LANGUAGE);
	ggz_dio_put_string(game_internal.dio, lang);
	ggz_dio_packet_end(game_internal.dio);
}


/* A newgame message tells the server to start a new game. */
void client_send_newgame(void)
{
	ggz_dio_packet_start(game_internal.dio);
	write_opcode(game_internal.dio, RSP_NEWGAME);
	ggz_dio_packet_end(game_internal.dio);
}


/* A bid message tells the server our choice for a bid. */
void client_send_bid(int bid)
{
	set_game_state(STATE_WAIT);

	ggz_dio_packet_start(game_internal.dio);
	write_opcode(game_internal.dio, RSP_BID);
	ggz_dio_put_int(game_internal.dio, bid);
	ggz_dio_packet_end(game_internal.dio);
}


/* An options message tells the server our choices for options. */
void client_send_options(int option_cnt, int *options)
{
	int i;

	set_game_state(STATE_WAIT);

	ggz_dio_packet_start(game_internal.dio);
	write_opcode(game_internal.dio, RSP_OPTIONS);
	ggz_dio_put_int(game_internal.dio, option_cnt);
	for (i = 0; i < option_cnt; i++)
		ggz_dio_put_int(game_internal.dio, options[i]);
	ggz_dio_packet_end(game_internal.dio);
}


/* A play message tells the server our choice for a play. */
void client_send_play(card_t card)
{
	set_game_state(STATE_WAIT);

	ggz_dio_packet_start(game_internal.dio);
	write_opcode(game_internal.dio, RSP_PLAY);
	write_card(game_internal.dio, card);
	ggz_dio_packet_end(game_internal.dio);
}


/* A sync request asks for a sync from the server. */
void client_send_sync_request(void)
{
	ggz_debug(DBG_CLIENT, "Sending sync request to server.");

	ggz_dio_packet_start(game_internal.dio);
	write_opcode(game_internal.dio, REQ_SYNC);
	ggz_dio_packet_end(game_internal.dio);
}

#ifdef GUI_CLIENT
int client_handle_ggz(void)
{
	return ggzmod_dispatch(game_internal.ggzmod);
}
#endif

static void server_read_callback(GGZDataIO * dio, void *userdata)
{
	int opcode;
	server_msg_t op;

	/* Read the opcode */
	read_opcode(game_internal.dio, &opcode);

	op = opcode;

	ggz_debug(DBG_CLIENT, "Received %s opcode from the server.",
		  get_server_opcode_name(op));

	switch (op) {
	case REQ_NEWGAME:
		game_get_newgame();
		ggzcards.play_hand = -1;
		return;
	case MSG_NEWGAME:
		/* TODO: don't make "new game" until here */
		handle_msg_newgame();
		return;
	case MSG_GAMEOVER:
		handle_msg_gameover();
		return;
	case MSG_PLAYERS:
		handle_msg_players();
		return;
	case MSG_NEWHAND:
		game_alert_newhand();
		return;
	case MSG_HAND:
		handle_msg_hand();
		return;
	case REQ_BID:
		handle_req_bid();
		return;
	case MSG_BID:
		handle_msg_bid();
		return;
	case REQ_PLAY:
		handle_req_play();
		return;
	case MSG_BADPLAY:
		handle_msg_badplay();
		return;
	case MSG_PLAY:
		handle_msg_play();
		return;
	case MSG_TABLE:
		handle_msg_table();
		return;
	case MSG_TRICK:
		handle_msg_trick();
		return;
	case MESSAGE_GAME:
		handle_message_global();
		return;
	case REQ_OPTIONS:
		handle_req_options();
		return;
	}

	ggz_error_msg("Error handling message"
		      " from server (opcode %d).", op);
}

/* This function handles any input from the server. */
int client_handle_server(void)
{
	ggz_dio_set_read_callback(game_internal.dio,
				  server_read_callback, NULL);
	return ggz_dio_read_data(game_internal.dio);
}
