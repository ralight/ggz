/* 
 * File: common.c
 * Author: Jason Short
 * Project: GGZCards Client-Common
 * Date: 07/22/2001
 * Desc: Backend to GGZCards Client-Common
 * $Id: common.c 2868 2001-12-10 23:03:45Z jdorje $
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

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>		/* for close() */

#include <config.h>
#include <easysock.h>
#include <ggz.h>

#include "common.h"
#include "protocol.h"


static int handle_message_global();

static int handle_text_message();
static int handle_player_message();
static int handle_cardlist_message();
static int handle_game_message();

static int ggzfd = -1;
static int game_max_hand_size = 0;

struct ggzcards_game_t ggzcards = { 0 };


int client_initialize(void)
{
	/* A word on debugging: the client-common code uses ggz for internal
	   memory management; i.e. ggz_malloc+ggz_free. Anything allocated
	   with easysock will be allocated with malloc and must be freed with 
	   free.  The table code (which is gui-specific) may use whatever
	   memory management routines it wants (currently the GTK client uses 
	   g_malloc and g_free). This may be unnecessarily complicated, but
	   remember that the internal client-common variables are always kept 
	   separate from the GUI variables, so there should be no confusion
	   there. And all of the easysock-allocated variables are labelled. */
#ifdef DEBUG
	ggz_debug_enable("core");
#endif
	ggz_debug_enable("core-error");
	ggzfd = ggz_connect();
	if (ggzfd < 0) {
		ggz_error_msg_exit("Couldn't connect to ggz.");
	}
	ggzcards.state = STATE_INIT;
	ggz_debug("core", "Client initialized.");
	return ggzfd;
}


void client_quit(void)
{
	/* FIXME: is this the desired behavior? */
	if (ggz_disconnect() < 0)
		ggz_error_msg_exit("Couldn't disconnect from ggz.");
	if (ggzcards.players)
		ggz_free(ggzcards.players);
	ggz_debug("core", "Client disconnected.");
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
		ggz_debug("core-error", "ERROR: Staying in state %d.",
			  ggzcards.state);
	} else {
		ggz_debug("core", "Changing state from %s to %s.",
			  get_state_name(ggzcards.state),
			  get_state_name(state));
		ggzcards.state = state;
	}
}

static int handle_text_message(void)
{
	char *message, *mark;
	if (es_read_string_alloc(ggzfd, &mark) < 0 ||
	    es_read_string_alloc(ggzfd, &message) < 0)
		return -1;
	table_set_global_text_message(mark, message);
	free(message);		/* allocated by easysock */
	free(mark);		/* allocated by easysock */
	return 0;
}

static int handle_cardlist_message(void)
{
	int status = 0, p, i;
	card_t **cardlist =
		ggz_malloc(ggzcards.num_players * sizeof(*cardlist));
	int *lengths = ggz_malloc(ggzcards.num_players * sizeof(*lengths));
	char *mark;

	if (!cardlist || !lengths)
		abort();

	if (es_read_string_alloc(ggzfd, &mark) < 0)
		status = -1;

	for (p = 0; p < ggzcards.num_players; p++) {
		if (es_read_int(ggzfd, &lengths[p]))
			status = -1;
		cardlist[p] = ggz_malloc(lengths[p] * sizeof(**cardlist));
		for (i = 0; i < lengths[p]; i++)
			if (read_card(ggzfd, &cardlist[p][i]) < 0)
				status = -1;
	}

	if (status == 0)
		table_set_global_cardlist_message(mark, lengths, cardlist);

	for (p = 0; p < ggzcards.num_players; p++)
		ggz_free(cardlist[p]);
	ggz_free(cardlist);
	ggz_free(lengths);
	free(mark);		/* allocated by easysock */

	return status;
}

/* A message_player message tells you one "player message", which is
   displayed by the client. */
static int handle_player_message(void)
{
	int p;
	char *message;

	if (read_seat(ggzfd, &p) < 0 ||
	    es_read_string_alloc(ggzfd, &message) < 0)
		return -1;
	assert(p >= 0 && p < ggzcards.num_players);

	table_set_player_message(p, message);

	free(message);		/* allocated by easysock */

	return 0;
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
static int handle_game_message(void)
{
	int size, game, handled;
	char *block;

	if (es_read_int(ggzfd, &game) < 0 || es_read_int(ggzfd, &size) < 0)
		return -1;

	/* Note: "size" refers to the size of the data block, not including
	   the headers above. */

	handled = table_handle_game_message(ggzfd, game, size);
	if (handled < 0)
		return -1;
	assert(handled <= size);
	size -= handled;	/* this is how much was unread */

	if (size > 0) {
		/* We read the block just to get it out of the way. */
		block = ggz_malloc(size);
		if (es_readn(ggzfd, block, size) < 0)
			return -1;
		ggz_free(block);
	}

	return 0;
}

/* a message_global message tells you one "global message", which is
   displayed by the client. */
static int handle_message_global(void)
{
	int opcode, status = 0;
	game_message_t op;

	if (read_opcode(ggzfd, &opcode) < 0)
		return -1;

	op = opcode;

	ggz_debug("core", "Received global message opcode of type %d.", op);

	switch (op) {
	case GAME_MESSAGE_TEXT:
		status = handle_text_message();
		break;
	case GAME_MESSAGE_CARDLIST:
		status = handle_cardlist_message();
		break;
	case GAME_MESSAGE_GAME:
		status = handle_game_message();
		break;
	case GAME_MESSAGE_PLAYER:
		status = handle_player_message();
		break;
	}

	return status;
}


/* A gameover message tells you the game is over, and who won. */
static int handle_msg_gameover(void)
{
	int num_winners, i, *winners = NULL;

	if (es_read_int(ggzfd, &num_winners) < 0)
		return -1;
	assert(num_winners >= 0 && num_winners <= ggzcards.num_players);

	if (num_winners > 0)
		winners = ggz_malloc(num_winners * sizeof(*winners));

	for (i = 0; i < num_winners; i++)
		if (read_seat(ggzfd, &winners[i]) < 0)
			return -1;

	table_handle_gameover(num_winners, winners);

	set_game_state(STATE_DONE);

	if (winners)
		ggz_free(winners);

	return 0;
}


/* A players message tells you all the players (well, seats really) at the
   table. */
static int handle_msg_players(void)
{
	int i, p, numplayers, different;
	char *t_name;

	/* It is possible to have 0 players.  At the begginning of a
	   "general" game, you don't know how many seats will be used yet so
	   the number of players is 0. */
	if (es_read_int(ggzfd, &numplayers) < 0)
		return -1;
	assert(numplayers >= 0);

	/* we may need to allocate memory for the players */
	different = (ggzcards.num_players != numplayers);

	/* reallocate the players, if necessary */
	if (different) {
		if (ggzcards.players) {
			for (p = 0; p < ggzcards.num_players; p++)
				if (ggzcards.players[p].hand.card)
					ggz_free(ggzcards.players[p].hand.
						 card);
			ggz_free(ggzcards.players);
		}
		ggz_debug("core",
			  "get_players: (re)allocating ggzcards.players.");
		ggzcards.players =
			ggz_malloc(numplayers * sizeof(*ggzcards.players));
		game_max_hand_size = 0;	/* this forces reallocating later */
	}

	/* TODO: support for changing the number of players */

	/* read in data about the players */
	for (i = 0; i < numplayers; i++) {
		/* FIXME: "assign" is the ggz status.  It should be of type
		   GGZdModSeat. */
		int assign;
		if (es_read_int(ggzfd, &assign) < 0 ||
		    es_read_string_alloc(ggzfd, &t_name) < 0)
			return -1;

		table_alert_player(i, assign, t_name);

		/* this causes unnecessary memory fragmentation */
		if (ggzcards.players[i].name)
			free(ggzcards.players[i].name);	/* allocated by
							   easysock */
		ggzcards.players[i].status = assign;
		ggzcards.players[i].name = t_name;
	}

	ggzcards.num_players = numplayers;

	/* Redesign the table, if necessary. */
	if (different)
		table_setup();

	/* TODO: should we need to enter a waiting state if players leave? */

	return 0;
}

/* Possibly increase the maximum hand size we can sustain. */
static void increase_max_hand_size(int max_hand_size)
{
	int p;

	if (max_hand_size <= game_max_hand_size)	/* no problem */
		return;

	ggz_debug("core",
		  "Expanding max_hand_size to allow for %d cards"
		  " (previously max was %d).", max_hand_size,
		  game_max_hand_size);
	game_max_hand_size = max_hand_size;

	/* Let the table know how big a hand might be. */
	table_alert_hand_size(game_max_hand_size);

	for (p = 0; p < ggzcards.num_players; p++) {
#if 1
		/* TODO: figure out how this code could even fail at all. In
		   the meantime, I've disabled the call to free (realloc),
		   conceding the memory leak so that we don't have an
		   unexplained seg fault (which we would have if the realloc
		   method were used instead!!). */
		ggzcards.players[p].hand.card =
			ggz_realloc(ggzcards.players[p].hand.card,
				    game_max_hand_size *
				    sizeof(*ggzcards.players[p].hand.card));
#else
		ggzcards.players[p].hand.card =
			ggz_malloc(game_max_hand_size *
				   sizeof(*ggzcards.players[p].hand.card));
#endif
	}

	table_setup();		/* redesign table */
}

/* A hand message tells you all the cards in one player's hand. */
static int handle_msg_hand(void)
{
	int player, hand_size, i;
	struct hand_t *hand;

	assert(ggzcards.players);

	/* first read the player whose hand it is */
	if (read_seat(ggzfd, &player) < 0)
		return -1;
	assert(player >= 0 && player < ggzcards.num_players);

	/* Find out how many cards in this hand */
	if (es_read_int(ggzfd, &hand_size) < 0)
		return -1;

	/* Reallocate hand structures, if necessary */
	increase_max_hand_size(hand_size);

	/* Read in all the card values.  It's important that we don't change
	   anything before here so that any functions we call from
	   increase_max_hand_size won't have inconsistent data. */
	hand = &ggzcards.players[player].hand;
	hand->hand_size = hand_size;
	for (i = 0; i < hand->hand_size; i++)
		if (read_card(ggzfd, &hand->card[i]) < 0)
			return -1;

	/* Finally, show the hand. */
	table_display_hand(player);

	return 0;
}


/* A bid request asks you to pick from a given list of bids. */
static int handle_req_bid(void)
{
	int i;
	int possible_bids;
	char **bid_choices;

	if (ggzcards.state == STATE_BID) {
		/* The new bid request overrides the old one.  But this means
		   some messy cleanup is necessary. */
		ggz_debug("core",
			  "WARNING: new bid message overriding old one.");
	}

	/* Determine the number of bidding choices we have */
	if (es_read_int(ggzfd, &possible_bids) < 0)
		return -1;
	bid_choices = ggz_malloc(possible_bids * sizeof(*bid_choices));

	/* Read in all of the bidding choices. */
	for (i = 0; i < possible_bids; i++) {
		if (es_read_string_alloc(ggzfd, &bid_choices[i]) < 0)
			return -1;
	}

	/* Get the bid */
	set_game_state(STATE_BID);
	table_get_bid(possible_bids, bid_choices);

	/* Clean up */
	for (i = 0; i < possible_bids; i++)
		free(bid_choices[i]);	/* allocated by easysock */
	ggz_free(bid_choices);

	return 0;
}


/* A play request asks you to play a card from any hand (most likely your
   own). */
static int handle_req_play(void)
{
	/* Determine which hand we're supposed to be playing from. */
	if (read_seat(ggzfd, &ggzcards.play_hand) < 0)
		return -1;
	assert(ggzcards.play_hand >= 0
	       && ggzcards.play_hand < ggzcards.num_players);

	/* Get the play. */
	set_game_state(STATE_PLAY);
	table_get_play(ggzcards.play_hand);

	return 0;
}


/* A badplay message indicates an invalid play, and requests a new one. */
static int handle_msg_badplay(void)
{
	char *err_msg;

	/* Read the error message for the bad play. */
	if (es_read_string_alloc(ggzfd, &err_msg) < 0)
		return -1;

	/* Restore the cards the way they should be. */
	ggzcards.players[ggzcards.play_hand].table_card = UNKNOWN_CARD;

	/* Get a new play. */
	set_game_state(STATE_PLAY);
	table_alert_badplay(err_msg);

	/* Clean up. */
	free(err_msg);		/* allocated by easysock */

	return 0;
}


/* returns an index into the hand's card for a match to the given card */
static int match_card(card_t card, struct hand_t *hand)
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
		card_t hcard = hand->card[tc];
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

		if (tc_matches > matches) {
			matches = tc_matches;
			match = tc;
		}
	}
	return match;
}


/* A play message tells of a play from a hand to the table. */
static int handle_msg_play(void)
{
	int p, c, tc;
	card_t card;
	struct hand_t *hand;

	/* Read the card being played. */
	if (read_seat(ggzfd, &p) < 0 || read_card(ggzfd, &card) < 0)
		return -1;
	assert(p >= 0 && p < ggzcards.num_players);

	/* Find the hand the card is to be removed from. */
	assert(ggzcards.players);
	hand = &ggzcards.players[p].hand;
	assert(hand->card);

	/* Find a matching card to remove. */
	tc = match_card(card, hand);

	/* Handle a bad match. */
	if (tc < 0) {
		/* This is theoretically possible even without errors! In
		   fact, a clever server could _force_ us to pick wrong.
		   Figure out how and you'll be ready for a "squeeze" play!
		   Fortunately, it's easily solved. */
		ggz_debug("core",
			  "Whoa!  We can't find a match for the card.  That's strange.");
		(void) client_send_sync_request();
		return 0;
	}

	/* Remove the card.  This is a bit inefficient. */
	for (c = tc; c < hand->hand_size; c++)
		hand->card[c] = hand->card[c + 1];
	hand->card[hand->hand_size] = UNKNOWN_CARD;
	hand->hand_size--;

	/* Update the graphics */
	table_alert_play(p, card);

	return 0;
}


/* A table message tells you all the cards on the table.  Each player only
   gets one card. */
static int handle_msg_table(void)
{
	int p;

	ggz_debug("core", "Handling table message.");

	assert(ggzcards.players);
	for (p = 0; p < ggzcards.num_players; p++)
		if (read_card(ggzfd, &ggzcards.players[p].table_card) < 0)
			return -1;

	/* TODO: verify that the table cards have been removed from the hands 
	 */

	table_alert_table();

	return 0;
}


/* A trick message tells you about the end of a trick (and who won). */
static int handle_msg_trick(void)
{
	int p;

	/* Read the trick winner */
	if (read_seat(ggzfd, &p) < 0)
		return -1;
	assert(p >= 0 && p < ggzcards.num_players);

	/* Update the graphics. */
	table_alert_trick(p);

	return 0;
}


/* An options request asks you to pick a set of options.  Each "option" gives
   a list of choices so that you pick one choice for each option.  An option
   with only one choice is a special case: a boolean option. */
static int handle_req_options(void)
{
	int i, j;
	int option_cnt;		/* the number of options */
	int *choice_cnt;	/* The number of choices for each option */
	int *defaults;		/* What option choice is currently chosen for
				   each option */
	char ***option_choices;	/* The texts for each option choice of each
				   option */

	if (ggzcards.state == STATE_OPTIONS) {
		/* The new options request overrides the old one.  But this
		   means some messy cleanup is necessary. */
		ggz_debug("core",
			  "WARNING: new options request overriding old one.");
	}

	/* Read the number of options. */
	if (es_read_int(ggzfd, &option_cnt) < 0)
		return -1;
	assert(option_cnt > 0);

	/* Allocate all data */
	choice_cnt = ggz_malloc(option_cnt * sizeof(*choice_cnt));
	defaults = ggz_malloc(option_cnt * sizeof(*defaults));
	option_choices = ggz_malloc(option_cnt * sizeof(*option_choices));

	/* Read all the options, their defaults, and the possible choices. */
	for (i = 0; i < option_cnt; i++) {
		if (es_read_int(ggzfd, &choice_cnt[i]) < 0 ||
		    es_read_int(ggzfd, &defaults[i]) < 0)
			return -1;	/* read the default */
		option_choices[i] =
			ggz_malloc(choice_cnt[i] * sizeof(**option_choices));
		for (j = 0; j < choice_cnt[i]; j++)
			if (es_read_string_alloc(ggzfd, &option_choices[i][j])
			    < 0)
				return -1;
	}

	/* Get the options. */
	set_game_state(STATE_OPTIONS);
	table_get_options(option_cnt, choice_cnt, defaults, option_choices);

	/* Clean up. */
	for (i = 0; i < option_cnt; i++) {
		for (j = 0; j < choice_cnt[i]; j++)
			free(option_choices[i][j]);	/* allocated by
							   easysock */
		ggz_free(option_choices[i]);
	}
	ggz_free(defaults);
	ggz_free(option_choices);
	ggz_free(choice_cnt);

	return 0;
}


/* A newgame message tells the server to start a new game. */
int client_send_newgame(void)
{
	if (write_opcode(ggzfd, RSP_NEWGAME) < 0) {
		ggz_debug("core-error", "Couldn't send newgame.");
		return -1;
	}
	return 0;
}


/* A bid message tells the server our choice for a bid. */
int client_send_bid(int bid)
{
	set_game_state(STATE_WAIT);
	if (write_opcode(ggzfd, RSP_BID) < 0 || es_write_int(ggzfd, bid) < 0) {
		ggz_debug("core-error", "Couldn't send bid.");
		return -1;
	}
	return 0;
}


/* An options message tells the server our choices for options. */
int client_send_options(int option_cnt, int *options)
{
	int i, status = 0;

	if (write_opcode(ggzfd, RSP_OPTIONS) < 0)
		status = -1;
	for (i = 0; i < option_cnt; i++)
		if (es_write_int(ggzfd, options[i]) < 0)
			status = -1;

	set_game_state(STATE_WAIT);

	if (status < 0) {
		ggz_debug("core-error", "Couldn't send options.");
		return -1;
	}
	return status;
}


/* A play message tells the server our choice for a play. */
int client_send_play(card_t card)
{
	set_game_state(STATE_WAIT);
	if (write_opcode(ggzfd, RSP_PLAY) < 0 || write_card(ggzfd, card) < 0) {
		ggz_debug("core-error", "Couldn't send play.");
		return -1;
	}
	return 0;
}


/* A sync request asks for a sync from the server. */
int client_send_sync_request(void)
{
	ggz_debug("core", "Sending sync request to server.");
	if (write_opcode(ggzfd, REQ_SYNC) < 0) {
		ggz_debug("core-error", "Couldn't send sync request.");
		return -1;
	}
	return 0;
}


/* This function handles any input from the server. */
int client_handle_server(void)
{
	int op, status = -1;

	/* Read the opcode */
	if (read_opcode(ggzfd, &op) < 0) {
		ggz_debug("core-error", "Couldn't read server opcode.");
		return -1;
	}

	switch (op) {
	case REQ_NEWGAME:
		table_get_newgame();
		status = 0;
		break;
	case MSG_NEWGAME:
		/* TODO: don't make "new game" until here */
		status = 0;
		break;
	case MSG_GAMEOVER:
		status = handle_msg_gameover();
		break;
	case MSG_PLAYERS:
		status = handle_msg_players();
		break;
	case MSG_HAND:
		status = handle_msg_hand();
		break;
	case REQ_BID:
		status = handle_req_bid();
		break;
	case REQ_PLAY:
		status = handle_req_play();
		break;
	case MSG_BADPLAY:
		status = handle_msg_badplay();
		break;
	case MSG_PLAY:
		status = handle_msg_play();
		break;
	case MSG_TABLE:
		status = handle_msg_table();
		break;
	case MSG_TRICK:
		status = handle_msg_trick();
		break;
	case MESSAGE_GAME:
		status = handle_message_global();
		break;
	case REQ_OPTIONS:
		status = handle_req_options();
		break;
	}

	if (status < 0)
		ggz_debug("core-error",
			  "Error handling message from server (opcode %d).",
			  op);
	return status;
}
