/* 
 * File: common.c
 * Author: Jason Short
 * Project: GGZCards Client-Common
 * Date: 07/22/2001
 * Desc: Backend to GGZCards Client-Common
 * $Id: common.c 2742 2001-11-13 22:58:05Z jdorje $
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
#include <ggz_client.h>

#include "common.h"
#include "protocol.h"


static int handle_message_global();
static int handle_message_player();

static int ggzfd = -1;
static int game_max_hand_size = 0;

struct game_t game = { 0 };


int client_initialize(void)
{
	ggzfd = ggz_connect();
	if (ggzfd < 0)
		exit(-1);
	game.state = STATE_INIT;
	return ggzfd;
}


void client_quit(void)
{
	/* FIXME: is this the desired behavior? */
	if (ggz_disconnect() < 0)
		exit(-2);
}


void client_debug(const char *fmt, ...)
{
	/* Currently the output goes to stderr, but it could be sent
	   elsewhere. */
	/* TODO: having this within #ifdef's wouldn't really work if it was
	   an external lib */
#ifdef DEBUG
	char buf[512];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	fprintf(stderr, "DEBUG: %s\n", buf);
	va_end(ap);
#endif /* DEBUG */
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
	if (state == game.state)
		client_debug("ERROR: " "Staying in state %d.", game.state);
	else {
		client_debug("Changing state from %s to %s.",
			     get_state_name(game.state),
			     get_state_name(state));
		game.state = state;
	}
}

static int handle_global_message_text(char *mark)
{
	char *message;
	if (es_read_string_alloc(ggzfd, &message) < 0)
		return -1;
	table_set_global_text_message(mark, message);
	return 0;
}

static int handle_global_message_cardlist(char *mark)
{
	int status = 0, p, i;
	card_t **cardlist = malloc(game.num_players * sizeof(*cardlist));
	int *lengths = malloc(game.num_players * sizeof(*lengths));

	if (!cardlist || !lengths)
		abort();

	for (p = 0; p < game.num_players; p++) {
		if (es_read_int(ggzfd, &lengths[p]))
			status = -1;
		cardlist[p] = malloc(lengths[p] * sizeof(**cardlist));
		for (i = 0; i < lengths[p]; i++)
			if (read_card(ggzfd, &cardlist[p][i]) < 0)
				status = -1;
	}

	if (status == 0)
		table_set_global_cardlist_message(mark, lengths, cardlist);

	for (p = 0; p < game.num_players; p++)
		free(cardlist[p]);
	free(cardlist);
	free(lengths);

	return status;
}

static int handle_global_message_block(char *mark)
{
	int size;
	char *block;

	if (es_read_int(ggzfd, &size) < 0)
		return -1;

	block = malloc(size);
	if (!block || es_readn(ggzfd, block, size) < 0)
		return -1;

	/* do nothing...yet */
	return 0;
}

/* a message_global message tells you one "global message", which is
   displayed by the client. */
static int handle_message_global(void)
{
	int opcode, status = 0;
	message_type_t op;
	char *mark;

	if (es_read_string_alloc(ggzfd, &mark) < 0 ||
	    read_opcode(ggzfd, &opcode) < 0)
		return -1;

	op = opcode;

	client_debug("Received opcode of type %d.", op);

	switch (op) {
	case GL_MESSAGE_TEXT:
		status = handle_global_message_text(mark);
		break;
	case GL_MESSAGE_CARDLIST:
		status = handle_global_message_cardlist(mark);
		break;
	case GL_MESSAGE_BLOCK:
		status = handle_global_message_block(mark);
		break;
	}

	free(mark);

	return status;
}


/* A message_player message tells you one "player message", which is
   displayed by the client. */
static int handle_message_player(void)
{
	int p;
	char *message;

	if (read_seat(ggzfd, &p) < 0 ||
	    es_read_string_alloc(ggzfd, &message) < 0)
		return -1;
	assert(p >= 0 && p < game.num_players);

	table_set_player_message(p, message);

	free(message);

	return 0;
}


/* A gameover message tells you the game is over, and who won. */
static int handle_msg_gameover(void)
{
	int num_winners, i, *winners = NULL;

	if (es_read_int(ggzfd, &num_winners) < 0)
		return -1;
	assert(num_winners >= 0 && num_winners <= game.num_players);

	if (num_winners > 0) {
		winners = malloc(num_winners * sizeof(*winners));
		if (!winners)
			return -1;
	}

	for (i = 0; i < num_winners; i++)
		if (read_seat(ggzfd, &winners[i]) < 0)
			return -1;

	table_handle_gameover(num_winners, winners);

	set_game_state(STATE_DONE);

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
	different = (game.num_players != numplayers);

	/* reallocate the players, if necessary */
	if (different) {
		if (game.players) {
			for (p = 0; p < game.num_players; p++)
				if (game.players[p].hand.card)
					free(game.players[p].hand.card);
			free(game.players);
		}
		client_debug("get_players: (re)allocating game.players.");
		game.players = malloc(numplayers * sizeof(*game.players));
		memset(game.players, 0, numplayers * sizeof(*game.players));
		game_max_hand_size = 0;	/* this forces reallocating later */
	}

	/* TODO: support for changing the number of players */

	/* read in data about the players */
	for (i = 0; i < numplayers; i++) {
		if (es_read_int(ggzfd, &game.players[i].assign) < 0 ||
		    es_read_string_alloc(ggzfd, &t_name) < 0)
			return -1;

		table_alert_player_name(i, t_name);

		/* this causes unnecessary memory fragmentation */
		if (game.players[i].name)
			free(game.players[i].name);
		game.players[i].name = t_name;
	}

	game.num_players = numplayers;

	/* Redesign the table, if necessary. */
	if (different)
		table_setup();

	/* TODO: should we need to enter a waiting state if players leave? */

	return 0;
}


/* A hand message tells you all the cards in one player's hand. */
static int handle_msg_hand(void)
{
	int i, player;
	struct hand_t *hand;

	assert(game.players);

	/* first read the player whose hand it is */
	if (read_seat(ggzfd, &player) < 0)
		return -1;
	assert(player >= 0 && player < game.num_players);
	hand = &game.players[player].hand;

	/* Zap our current hand */
	for (i = 0; i < game_max_hand_size; i++)
		hand->card[i] = UNKNOWN_CARD;

	/* Find out how many cards in this hand */
	if (es_read_int(ggzfd, &hand->hand_size) < 0)
		return -1;

	/* Reallocate hand structures, if necessary */
	if (hand->hand_size > game_max_hand_size) {
		int p;
		client_debug("Expanding max_hand_size to allow for %d cards"
			     " (previously max was %d).", hand->hand_size,
			     game_max_hand_size);
		game_max_hand_size = hand->hand_size;

		/* Let the table know how big a hand might be. */
		table_alert_hand_size(game_max_hand_size);

		for (p = 0; p < game.num_players; p++) {
			/* TODO: figure out how this code could even fail at
			   all. In the meantime, I've disabled the call to
			   free, conceding the memory leak so that we don't
			   have an unexplained seg fault (which we would have
			   if these two lines were included) */
			/* if (game.players[p].hand.card != NULL)
			   free(game.players[p].hand.card); */
			game.players[p].hand.card =
				malloc(game_max_hand_size *
				       sizeof(*game.players[p].hand.card));
			if (!game.players[p].hand.card)
				return -1;
		}

		table_setup();	/* redesign table */
	}

	/* Read in all the card values */
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

	if (game.state == STATE_BID)
		/* TODO: the new bid request should override the old one */
		return 0;

	/* Determine the number of bidding choices we have */
	if (es_read_int(ggzfd, &possible_bids) < 0)
		return -1;
	bid_choices = malloc(possible_bids * sizeof(*bid_choices));
	if (!bid_choices)
		return -1;

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
		free(bid_choices[i]);
	free(bid_choices);

	return 0;
}


/* A play request asks you to play a card from any hand (most likely your
   own). */
static int handle_req_play(void)
{
	/* Determine which hand we're supposed to be playing from. */
	if (read_seat(ggzfd, &game.play_hand) < 0)
		return -1;
	assert(game.play_hand >= 0 && game.play_hand < game.num_players);

	/* Get the play. */
	set_game_state(STATE_PLAY);
	table_get_play(game.play_hand);

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
	game.players[game.play_hand].table_card = UNKNOWN_CARD;

	/* Get a new play. */
	set_game_state(STATE_PLAY);
	table_alert_badplay(err_msg);

	/* Clean up. */
	free(err_msg);

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
	assert(p >= 0 && p < game.num_players);

	/* Find the hand the card is to be removed from. */
	assert(game.players);
	hand = &game.players[p].hand;
	assert(hand->card);

	/* Find a matching card to remove. */
	tc = match_card(card, hand);

	/* Handle a bad match. */
	if (tc < 0) {
		/* This is theoretically possible even without errors! In
		   fact, a clever server could _force_ us to pick wrong.
		   Figure out how and you'll be ready for a "squeeze" play!
		   Fortunately, it's easily solved. */
		client_send_sync_request();
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

	assert(game.players);
	for (p = 0; p < game.num_players; p++)
		if (read_card(ggzfd, &game.players[p].table_card) < 0)
			return -1;

	/* TODO: verify that the table cards have been removed from the hands */

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
	assert(p >= 0 && p < game.num_players);

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

	if (game.state == STATE_OPTIONS)
		/* Should the new request override the old one? */
		return 0;

	/* Read the number of options. */
	if (es_read_int(ggzfd, &option_cnt) < 0)
		return -1;
	assert(option_cnt > 0);

	/* Allocate all data */
	choice_cnt = malloc(option_cnt * sizeof(*choice_cnt));
	defaults = malloc(option_cnt * sizeof(*defaults));
	option_choices = malloc(option_cnt * sizeof(*option_choices));

	/* Read all the options, their defaults, and the possible choices. */
	for (i = 0; i < option_cnt; i++) {
		if (es_read_int(ggzfd, &choice_cnt[i]) < 0 ||
		    es_read_int(ggzfd, &defaults[i]) < 0)
			return -1;	/* read the default */
		option_choices[i] =
			malloc(choice_cnt[i] * sizeof(**option_choices));
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
			free(option_choices[i][j]);
		free(option_choices[i]);
	}
	free(defaults);
	free(option_choices);
	free(choice_cnt);

	return 0;
}


/* A newgame message tells the server to start a new game. */
int client_send_newgame(void)
{
	if (write_opcode(ggzfd, RSP_NEWGAME) < 0)
		return -1;
	return 0;
}


/* A bid message tells the server our choice for a bid. */
int client_send_bid(int bid)
{
	set_game_state(STATE_WAIT);
	if (write_opcode(ggzfd, RSP_BID) < 0 || es_write_int(ggzfd, bid) < 0)
		return -1;
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

	return status;
}


/* A play message tells the server our choice for a play. */
int client_send_play(card_t card)
{
	set_game_state(STATE_WAIT);
	if (write_opcode(ggzfd, RSP_PLAY) < 0 || write_card(ggzfd, card) < 0)
		return -1;
	return 0;
}


/* A sync request asks for a sync from the server. */
int client_send_sync_request(void)
{
	if (write_opcode(ggzfd, REQ_SYNC) < 0)
		return -1;
	return 0;
}


/* This function handles any input from the server. */
int client_handle_server(void)
{
	int op, status = -1;

	/* Read the opcode */
	if (read_opcode(ggzfd, &op) < 0)
		return -1;

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
	case MESSAGE_GLOBAL:
		status = handle_message_global();
		break;
	case MESSAGE_PLAYER:
		status = handle_message_player();
		break;
	case REQ_OPTIONS:
		status = handle_req_options();
		break;
	}

	if (status < 0) {
		client_debug("Lost connection to server?!  Opcode was %d.",
			     op);
		client_quit();
		exit(-1);
	}

	return status;
}
