/* $Id: common.c 2076 2001-07-23 08:13:23Z jdorje $ */
/*
 * File: common.c
 * Author: Jason Short
 * Project: GGZCards Client-Common
 * Date: 07/22/2001
 * Desc: Backend to GGZCards Client-Common
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>		/* for close() */

#include <easysock.h>
#include <ggz_client.h>

#include "common.h"
#include "protocol.h"


static int handle_message_global();
static int handle_message_player();

static int ggzfd = -1;

struct game_t game = { 0 };


static int es_read_card(int fd, card_t * card)
{
	if (es_read_char(fd, &card->face) < 0)
		return -1;
	if (es_read_char(fd, &card->suit) < 0)
		return -1;
	if (es_read_char(fd, &card->deck) < 0)
		return -1;
	return 0;
}


static int es_write_card(int fd, card_t card)
{
	if (es_write_char(fd, card.face) < 0)
		return -1;
	if (es_write_char(fd, card.suit) < 0)
		return -1;
	if (es_write_char(fd, card.deck) < 0)
		return -1;
	return 0;
}


int client_initialize(void)
{
	ggz_client_init("GGZCards");
	ggzfd = ggz_client_connect();
	if (ggzfd < 0)
		exit(-1);
	return ggzfd;
}


void client_quit(void)
{
	ggz_client_quit();
}


void client_debug(const char *fmt, ...)
{
	/* Currently the output goes to stderr, but it could be sent elsewhere. */
	/* TODO: having this within #ifdef's wouldn't work if it was an external lib */
#ifdef DEBUG
	char buf[512];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	fprintf(stderr, "DEBUG: %s\n", buf);
	va_end(ap);
#endif /* DEBUG */
}


static void set_game_state(client_state_t state)
{
	char *game_states[] =
		{ "INIT", "WAIT", "PLAY", "BID", "DONE", "OPTIONS" };
	if (state == game.state)
		client_debug("Staying in state %d.", game.state);
	else {
		client_debug("Changing state from %s to %s.",
			     game_states[(int) game.state],
			     game_states[(int) state]);
		game.state = state;
	}
}


static int handle_message_global()
{
	char *mark;
	char *message;

	if (es_read_string_alloc(ggzfd, &mark) < 0
	    || es_read_string_alloc(ggzfd, &message) < 0)
		return -1;
	assert(message);

	client_debug("     Global message received, marked as '%s':%s", mark,
		     message);

	table_set_global_message(mark, message);

	free(message);
	free(mark);

	return 0;
}


static int handle_message_player()
{
	int p;
	char *message;
	if (es_read_int(ggzfd, &p) < 0
	    || es_read_string_alloc(ggzfd, &message) < 0)
		return -1;
	assert(p >= 0 && p < game.num_players);

	client_debug("    Player message recived:%s", message);

	table_set_player_message(p, message);

	free(message);

	return 0;
}


static int handle_msg_gameover()
{
	int num_winners, i, *winners = NULL;

	if (es_read_int(ggzfd, &num_winners) < 0)
		return -1;
	assert(num_winners >= 0 && num_winners <= game.num_players);

	if (num_winners > 0) {
		winners = malloc(num_winners * sizeof(int));
		if (!winners)
			return -1;
	}

	for (i = 0; i < num_winners; i++)
		if (es_read_int(ggzfd, &winners[i]) < 0)
			return -1;

	table_handle_gameover(num_winners, winners);

	set_game_state(WH_STATE_DONE);

	return 0;
}


static int handle_msg_players()
{
	int i, left = 0, p, numplayers, different;
	char *t_name;

	if (es_read_int(ggzfd, &numplayers) < 0)
		return -1;

	/* we may need to allocate memory for the players */
	different = (game.num_players != numplayers);

	if (different) {
		if (game.players) {
			for (p = 0; p < game.num_players; p++)
				if (game.players[p].hand.card)
					free(game.players[p].hand.card);
			free(game.players);
		}
		client_debug("get_players: (re)allocating game.players.");
		game.players = (seat_t *) malloc(numplayers * sizeof(seat_t));
		memset(game.players, 0, numplayers * sizeof(seat_t));
		game.max_hand_size = 0;	/* this forces reallocating later */
	}

	/* TODO: support for changing the number of players */

	for (i = 0; i < numplayers; i++) {
		if (es_read_int(ggzfd, &game.players[i].seat) < 0)
			return -1;
		if (es_read_string_alloc(ggzfd, &t_name) < 0)
			return -1;

		table_alert_player_name(i, t_name);

		/* Note: this approach promotes hard-core memory
		 * fragmentation! */
		if (game.players[i].name)
			free(game.players[i].name);
		game.players[i].name = t_name;
	}

	game.num_players = numplayers;

	if (different)
		table_setup();

	if (left && game.state == WH_STATE_BID) {
		/* TODO: cancel bid (I think????) */
	}
	if (left)
		set_game_state(WH_STATE_WAIT);

	return 0;
}


static int handle_msg_hand()
{
	int i, player;
	struct hand_t *hand;

	assert(game.players);

	/* first read the player whose hand it is */
	if (es_read_int(ggzfd, &player) < 0)
		return -1;
	assert(player >= 0 && player < game.num_players);
	hand = &game.players[player].hand;

	client_debug("     Reading the hand of player %d.", player);

	/* Zap our hand */
	for (i = 0; i < game.max_hand_size; i++)
		hand->card[i] = UNKNOWN_CARD;
	hand->selected_card = -1;	/* index into the array */

	/* First find out how many cards in this hand */
	if (es_read_int(ggzfd, &hand->hand_size) < 0)
		return -1;

	if (hand->hand_size > game.max_hand_size) {
		int p;
		client_debug
			("Expanding max_hand_size to allow for %d cards (previously max was %d).",
			 hand->hand_size, game.max_hand_size);
		game.max_hand_size = hand->hand_size;

		/* TODO: this shouldn't be handled like this.  Rather,
		 * the table should maintain it's own max_hand_size separately. */
		while (!table_verify_hand_size())
			game.max_hand_size++;

		for (p = 0; p < game.num_players; p++) {
			/* TODO: figure out how this code could even fail at all.
			   In the meantime, I've disabled the call to free, conceding
			   the memory leak so that we don't have an unexplained seg fault
			   (which we would have if these two lines were included) */
/*
			if (game.players[p].hand.card != NULL)
				free(game.players[p].hand.card);
*/
			game.players[p].hand.card =
				(card_t *) malloc(game.max_hand_size *
						  sizeof(card_t));
			if (!game.players[p].hand.card)
				return -1;
		}
		table_setup();
	}

	client_debug("     Read hand_size as %d.",
		     game.players[player].hand.hand_size);

	/* Read in all the card values */
	for (i = 0; i < hand->hand_size; i++)
		if (es_read_card(ggzfd, &hand->card[i]) < 0)
			return -1;

	table_display_hand(player);

	return 0;
}


static int handle_req_bid()
{
	int i;
	int possible_bids;
	char **bid_choices;

	if (game.state == WH_STATE_BID)
		/* TODO: the new bid request should override the old one */
		return 0;
	set_game_state(WH_STATE_BID);

	if (es_read_int(ggzfd, &possible_bids) < 0)
		return -1;

	client_debug("     Handling bid request: %d possible bids.",
		     possible_bids);
	bid_choices = (char **) malloc(possible_bids * sizeof(char *));
	if (!bid_choices)
		return -1;

	for (i = 0; i < possible_bids; i++) {
		if (es_read_string_alloc(ggzfd, &bid_choices[i]) < 0) {
			client_debug("Error reading string %d.", i);
			return -1;
		}
	}

	table_get_bid(possible_bids, bid_choices);

	for (i = 0; i < possible_bids; i++)
		free(bid_choices[i]);
	free(bid_choices);

	return 0;
}


static int handle_req_play()
{
	if (es_read_int(ggzfd, &game.play_hand) < 0)
		return -1;

	assert(game.play_hand >= 0 && game.play_hand < game.num_players);

	set_game_state(WH_STATE_PLAY);

	table_get_play(game.play_hand);
	return 0;
}


static int handle_msg_badplay(void)
{
	char *err_msg;
	int p = game.play_hand;

	if (es_read_string_alloc(ggzfd, &err_msg) < 0)
		return -1;

	/* Restore the cards the way they should be */
	game.players[p].table_card = UNKNOWN_CARD;

	set_game_state(WH_STATE_PLAY);

	table_alert_badplay(err_msg);

	free(err_msg);

	return 0;
}


static int handle_msg_play(void)
{
	int p, c, tc;
	card_t card;
	struct hand_t *hand;

	if (es_read_int(ggzfd, &p) < 0 || es_read_card(ggzfd, &card) < 0)
		return -1;

	assert(p >= 0 && p < game.num_players);

	client_debug("     Received play from player %d: %i %i %i.", p,
		     card.face, card.suit, card.deck);

	/* remove the card from the hand */
	assert(game.players);
	hand = &game.players[p].hand;
	assert(game.players[p].hand.card);

	/* first, find a matching card to remove.
	 * Anything "unknown" will match, as will the card itself*/
	for (tc = hand->hand_size - 1; tc >= 0; tc--) {
		/* TODO: this won't work in mixed known-unknown hands */
		card_t hcard = hand->card[tc];
		if (hcard.deck != -1 && hcard.deck != card.deck)
			continue;
		if (hcard.suit != -1 && hcard.suit != card.suit)
			continue;
		if (hcard.face != -1 && hcard.face != card.face)
			continue;
		break;
	}
	if (tc == -1) {
		client_debug("SERVER/CLIENT BUG: unknown card played.");
		return -1;
	}

	/* now, remove the card */
	for (c = tc; c < hand->hand_size; c++)
		hand->card[c] = hand->card[c + 1];
	hand->card[hand->hand_size] = UNKNOWN_CARD;
	hand->hand_size--;

	/* now update the graphics */
	table_alert_play(p, card);

	return 0;
}


static int handle_msg_table()
{
	int p;

	assert(game.players);
	for (p = 0; p < game.num_players; p++)
		if (es_read_card(ggzfd, &game.players[p].table_card) < 0)
			return -1;

	/* TODO: verify that the table cards have been removed from the hands */

	table_alert_table();
	return 0;
}


/* get_trick_winner
 *   handles the end of a trick
 */
static int handle_msg_trick()
{
	int p;

	if (es_read_int(ggzfd, &p) < 0)
		return -1;
	assert(p >= 0 && p < game.num_players);

	table_alert_trick(p);

	return 0;
}


int handle_req_options()
{
	int i, j;
	int option_cnt;		/* the number of options */
	int *choice_cnt;	/* The number of choices for each option */
	int *defaults;		/* What option choice is currently chosen for each option */
	char ***option_choices;	/* The texts for each option choice of each option */

	if (es_read_int(ggzfd, &option_cnt) < 0)
		return -1;
	client_debug("     Handling option request: %d possible options.",
		     option_cnt);

	choice_cnt = (int *) malloc(option_cnt * sizeof(int));
	defaults = (int *) malloc(option_cnt * sizeof(int));
	option_choices = (char ***) malloc(option_cnt * sizeof(char **));

	for (i = 0; i < option_cnt; i++) {
		if (es_read_int(ggzfd, &choice_cnt[i]) < 0)
			return -1;
		if (es_read_int(ggzfd, &defaults[i]) < 0)
			return -1;	/* read the default */
		option_choices[i] =
			(char **) malloc(choice_cnt[i] * sizeof(char *));
		for (j = 0; j < choice_cnt[i]; j++)
			if (es_read_string_alloc(ggzfd, &option_choices[i][j])
			    < 0)
				return -1;
	}

	if (game.state == WH_STATE_OPTIONS) {
		client_debug("Received second option request.  Ignoring it.");
		free(defaults);
	} else {
		set_game_state(WH_STATE_OPTIONS);
		table_get_options(option_cnt, choice_cnt, defaults,
				  option_choices);
	}

	for (i = 0; i < option_cnt; i++) {
		for (j = 0; j < choice_cnt[i]; j++)
			free(option_choices[i][j]);
		free(option_choices[i]);
	}
	free(option_choices);
	free(choice_cnt);

	return 0;
}


int client_send_newgame()
{
	int status;
	status = es_write_int(ggzfd, WH_RSP_NEWGAME);
	client_debug("     Handled WH_REQ_NEWGAME: status is %d.", status);
	return status;
}


int client_send_bid(int bid)
{
	client_debug("Sending bid to server: index %i.", bid);
	if (es_write_int(ggzfd, WH_RSP_BID) < 0 ||
	    es_write_int(ggzfd, bid) < 0)
		return -1;
	set_game_state(WH_STATE_WAIT);
	return 0;
}


int client_send_options(int option_cnt, int *options)
{
	int i, status = 0;
	if (es_write_int(ggzfd, WH_RSP_OPTIONS) < 0)
		status = -1;
	for (i = 0; i < option_cnt; i++)
		if (es_write_int(ggzfd, options[i]) < 0)
			status = -1;

	free(options);

	set_game_state(WH_STATE_WAIT);

	return status;
}


int client_send_play(card_t card)
{
	int status = 0;
	client_debug("Sending play to server: %i %i %i.", card.face,
		     card.suit, card.deck);
	if (es_write_int(ggzfd, WH_RSP_PLAY) < 0
	    || es_write_card(ggzfd, card) < 0)
		status = -1;

	set_game_state(WH_STATE_WAIT);

	return status;
}


int client_handle_server()
{
	int op, status = 0;
	char *opstr[] =
		{ "WH_REQ_NEWGAME", "WH_MSG_NEWGAME", "WH_MSG_GAMEOVER",
		"WH_MSG_PLAYERS", "WH_MSG_HAND", "WH_REQ_BID", "WH_REQ_PLAY",
		"WH_MSG_BADPLAY", "WH_MSG_PLAY", "WH_MSG_TRICK",
		"WH_MESSAGE_GLOBAL", "WH_MESSAGE_PLAYER", "WH_REQ_OPTIONS",
		"WH_MSG_TABLE"
	};

	if (es_read_int(ggzfd, &op) < 0)
		return -1;

	status = 0;

	if (op >= WH_REQ_NEWGAME && op <= WH_REQ_OPTIONS)
		client_debug("Received opcode: %s", opstr[op]);
	else
		client_debug("Received opcode %d", op);

	switch (op) {
	case WH_REQ_NEWGAME:
		table_get_newgame();
		break;
	case WH_MSG_NEWGAME:
		/* TODO: don't make "new game" until here */
		break;
	case WH_MSG_GAMEOVER:
		status = handle_msg_gameover();
		break;
	case WH_MSG_PLAYERS:
		status = handle_msg_players();
		break;
	case WH_MSG_HAND:
		status = handle_msg_hand();
		break;
	case WH_REQ_BID:
		if (handle_req_bid() < 0)
			client_debug("Error or bug: -1 returned by handle_bid_request.");	/* TODO */
		break;
	case WH_REQ_PLAY:
		status = handle_req_play();
		break;
	case WH_MSG_BADPLAY:
		handle_msg_badplay();
		break;
	case WH_MSG_PLAY:
		status = handle_msg_play();
		break;
	case WH_MSG_TABLE:
		status = handle_msg_table();
		break;
	case WH_MSG_TRICK:
		status = handle_msg_trick();
		break;
	case WH_MESSAGE_GLOBAL:
		status = handle_message_global();
		break;
	case WH_MESSAGE_PLAYER:
		status = handle_message_player();
		break;
	case WH_REQ_OPTIONS:
		status = handle_req_options();
		break;
	default:
		client_debug("SERVER/CLIENT bug: unknown opcode received %d",
			     op);
		status = -1;
		break;
	}

	if (status == -1) {
		client_debug("Lost connection to server?!");
		close(ggzfd);
		exit(-1);
	}

	return status;
}
