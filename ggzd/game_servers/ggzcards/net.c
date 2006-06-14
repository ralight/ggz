/* 
 * File: net.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: Game-independent game network functions
 * $Id: net.c 8192 2006-06-14 03:01:41Z jdorje $
 *
 * This file contains code that controls the flow of a general
 * trick-taking game.  Game states, event handling, etc. are all
 * handled within this code.  For specialty handling we call a
 * game-specific function from game.c or games/<game>.c.
 *
 * Copyright (C) 2001-2002 GGZ Development Team
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

#include <assert.h>
#include <stdlib.h>

#include <ggz.h>

#include "net_common.h"

#include "bid.h"
#include "common.h"
#include "message.h"
#include "net.h"
#include "options.h"
#include "play.h"

#define NET_ERROR(p) return handle_neterror_event(p)

seat_t convert_seat(seat_t s_abs, player_t p)
{
	/* Treat spectators as if they're sitting at seat 0. */
	if (p < 0)
		p = 0;

	assert(game.players[p].seat >= 0);
	return (s_abs - game.players[p].seat + game.num_seats)
	  % game.num_seats;
}

seat_t unconvert_seat(seat_t s_rel, player_t p)
{
	/* Treat spectators as if they're sitting at seat 0. */
	if (p < 0)
		p = 0;

	assert(game.players[p].seat >= 0);
	return (game.players[p].seat + s_rel) % game.num_seats;
}

#define broadcast_iterate(p)                                \
{                                                           \
	player_t p;                                         \
	for (p = -ggzdmod_get_max_num_spectators(game.ggz); \
	     p < game.num_players;                          \
	     p++)                                           \
		if (is_broadcast_seat(p)) {

#define broadcast_iterate_end } }


/* Return TRUE if this is a seat we should broadcast to,
   FALSE otherwise. */
static bool is_broadcast_seat(player_t p)
{
	if (get_player_socket(p) >= 0)
		return TRUE;

	assert(p >= 0 || get_player_name(p) == NULL);
	assert(get_player_status(p) != GGZ_SEAT_PLAYER);

	/* This can happen at end-of-game. */
	/* assert(get_player_status(p) != GGZ_SEAT_BOT); */

	return FALSE;
}


/* Send out player list to player p */
/** A player list packet is composed of:
 *    - The MSG_PLAYERS opcode
 *    - An integer containing the number of seats
 *    - For each seat, in *relative* ordering to
 *      the player being sent the message:
 *        - An integer that is the GGZ status of the seat
 *        - A string that is the name of the player at the seat
 */
void net_send_player_list(player_t p)
{
	int fd = get_player_socket(p);
	seat_t s_rel;

	if (write_opcode(fd, MSG_PLAYERS) < 0 ||
	    ggz_write_int(fd, game.num_seats) < 0)
		NET_ERROR(p);

	/* Note that this function can be called before we know what game
	   we're playing.  In this case, we'll know the number of players
	   (and possibly even who some of them are) but not the number of
	   seats.  Currently this will result in the number of seats being
	   set to zero and no data being sent out.  Later it might be
	   desirable to finesse data by sending the player list instead. */
	for (s_rel = 0; s_rel < game.num_seats; s_rel++) {
		seat_t s_abs = UNCONVERT_SEAT(s_rel, p);
		if (ggz_write_int(fd, get_seat_status(s_abs)) < 0
		    || ggz_write_string(fd, get_seat_name(s_abs)) < 0
		    || ggz_write_int(fd, game.seats[s_abs].player) < 0)
			NET_ERROR(p);
	}
}

void net_broadcast_player_list(void)
{
	broadcast_iterate(p) {
		/* Sometimes this means we'll send the list to a player
		   who's not connected or a bot that hasn't been
		   spawned yet.  This isn't great.  Note, though, that
		   we *do* need to send a real player list to bot
		   players, who may be relying on this information. */
		net_send_player_list(p);
	} broadcast_iterate_end;
}

void net_send_options_request(player_t p,
                              int num_options,
                              char **option_descs,
                              int *num_choices,
                              int *option_defaults,
                              char ***option_choices)
{
	int fd = get_player_socket(p);
	int i, j;
	
	if (write_opcode(fd, REQ_OPTIONS) < 0 ||
	    ggz_write_int(fd, num_options) < 0)
		NET_ERROR(p);
	for (i = 0; i < num_options; i++) {
		if (ggz_write_string(fd, option_descs[i]) < 0 ||
		    ggz_write_int(fd, num_choices[i]) < 0 ||
		    ggz_write_int(fd, option_defaults[i]) < 0)
			NET_ERROR(p);
		for (j = 0; j < num_choices[i]; j++)
			if (ggz_write_string(fd, option_choices[i][j]) < 0)
				NET_ERROR(p);
	}
}

/* Send out play for player to _all_ players. Also symbolizes that this play
   is over. */
/** A play packet is composed of:
 *    - A MSG_PLAY opcode
 *    - A seat that is the relative seat of the player playing
 *    - A card that is the card being played
 */
static void net_send_play(player_t p, seat_t player, card_t card)
{
	int fd = get_player_socket(p);
	
	if (write_opcode(fd, MSG_PLAY) < 0 ||
	    write_seat(fd, CONVERT_SEAT(player, p)) < 0 ||
	    write_card(fd, card) < 0)
		NET_ERROR(p);
}

void net_broadcast_play(seat_t player, card_t card)
{
	broadcast_iterate(p) {
		net_send_play(p, player, card);
	} broadcast_iterate_end;
}

/* Send a gameover to all players. cnt is the number of winners, plist is the
   winner list */
/** A gameover packet is composed of:
 *    - A MSG_GAMEOVER opcode
 *    - An integer that is the number of winners
 *    - For each winning player:
 *        - A seat that is the relative seat of the player
 */
static void net_send_gameover(player_t p, int winner_cnt, player_t * winners)
{
	int fd = get_player_socket(p);
	int i;
	
	assert(winner_cnt >= 0 && winner_cnt <= game.num_players);

	if (write_opcode(fd, MSG_GAMEOVER) < 0 ||
	    ggz_write_int(fd, winner_cnt) < 0)
		NET_ERROR(p);
	
	for (i = 0; i < winner_cnt; i++) {
		seat_t ws = game.players[winners[i]].seat;
		
		if (write_seat(fd, CONVERT_SEAT(ws, p)) < 0)
			NET_ERROR(p);
	}
}

void net_broadcast_gameover(int winner_cnt, player_t *winners)
{
	broadcast_iterate(p) {
		net_send_gameover(p, winner_cnt, winners);
	} broadcast_iterate_end;
}

/* sends the list of cards on the table, in their relative orderings */
void net_send_table(player_t p)
{
	seat_t s_r, s_abs;
	int fd = get_player_socket(p);

	if (game.num_seats == 0) /* FIXME: don't check this here */
		return;

	ggz_debug(DBG_NET, "Sending table to player %d/%s.", p,
		    get_player_name(p));

	if (write_opcode(fd, MSG_TABLE) < 0)
		NET_ERROR(p);
	for (s_r = 0; s_r < game.num_seats; s_r++) {
		s_abs = UNCONVERT_SEAT(s_r, p);
		if (write_card(fd, game.seats[s_abs].table) < 0)
			NET_ERROR(p);
	}
}

/* Request a bid from player p.  bid_count is the number of bids; bids is an
   array listing the possible bids. */
void net_send_bid_request(player_t p, int bid_count, bid_t * bids)
{
	int i;
	int fd = get_player_socket(p);
	GGZSeatType seat_type = get_player_status(p);

	ggz_debug(DBG_NET, "Sending bid request to player %d/%s.", p,
		    get_player_name(p));

	/* request a bid from the client */
	if (write_opcode(fd, REQ_BID) < 0 || ggz_write_int(fd, bid_count) < 0)
		NET_ERROR(p);
	for (i = 0; i < bid_count; i++) {
		char bid_text[128] = "";
		char bid_desc[1024] = "";
		if (seat_type != GGZ_SEAT_BOT) {
			/* HACK: we need to send the full bid text to
			   a bot. */
			game.data->get_bid_text(bid_text, sizeof(bid_text),
			                         bids[i]);
			game.data->get_bid_desc(bid_desc, sizeof(bid_desc),
			                         bids[i]);
		}
		if (write_bid(fd, bids[i]) < 0 ||
		    ggz_write_string(fd, bid_text) < 0 ||
		    ggz_write_string(fd, bid_desc) < 0)
			NET_ERROR(p);
	}
}

void net_send_bid(player_t p, player_t bidder, bid_t bid)
{
	seat_t seat = game.players[bidder].seat;
	int fd = get_player_socket(p);
	
	if (write_opcode(fd, MSG_BID) < 0 ||
	    write_seat(fd, CONVERT_SEAT(seat, p)) < 0 ||
	    write_bid(fd, bid) < 0)
		NET_ERROR(p);
}

void net_broadcast_bid(player_t bidder, bid_t bid)
{
	broadcast_iterate(p) {
		net_send_bid(p, bidder, bid);
	} broadcast_iterate_end;
}

/* Request a player p to make a play from a specific seat s's hand */
void net_send_play_request(player_t p, seat_t s)
{
	seat_t s_r = CONVERT_SEAT(s, p);
	int fd = get_player_socket(p);
	
	hand_t *hand = &game.seats[s].hand;
	card_t valid_plays[hand->hand_size];
	int i, num_valid_plays = 0;
	
	assert(game.players[p].is_playing);
	
	for (i = 0; i < hand->hand_size; i++) {
		card_t card = hand->cards[i];
		if (!game.data->verify_play(p, card)) {
			valid_plays[num_valid_plays] = card;
			num_valid_plays++;	
		}
	}

	ggz_debug(DBG_NET, "Requesting player %d/%s "
		    "to play from seat %d/%s's hand.", p,
		    get_player_name(p), s, get_seat_name(s));

	if (write_opcode(fd, REQ_PLAY) < 0 ||
	    write_seat(fd, s_r) < 0 ||
	    ggz_write_int(fd, num_valid_plays) < 0)
		NET_ERROR(p);
		
	for (i = 0; i < num_valid_plays; i++)
		if (write_card(fd, valid_plays[i]) < 0)
			NET_ERROR(p);
}

void net_send_badplay(player_t p, char *msg)
{
	int fd = get_player_socket(p);
	
	if (write_opcode(fd, MSG_BADPLAY) < 0 || ggz_write_string(fd, msg) < 0)
		NET_ERROR(p);
}

/* Show a player a hand.  This will reveal the cards in the hand iff reveal
   is true. */
void net_send_hand(const player_t p, const seat_t s,
                   bool show_fronts, bool show_backs)
{
	int fd = get_player_socket(p);
	int i;

	ggz_debug(DBG_NET,
		    "Sending player %d/%d/%s hand %d/%s - revealing(%d,%d)", p,
		    PLAYER_TO_SEAT(p), get_player_name(p), s,
		    get_seat_name(s), show_fronts, show_backs);

	if (write_opcode(fd, MSG_HAND) < 0
	    || write_seat(fd, CONVERT_SEAT(s, p)) < 0
	    || ggz_write_int(fd, game.seats[s].hand.hand_size) < 0)
		NET_ERROR(p);

	for (i = 0; i < game.seats[s].hand.hand_size; i++) {
		card_t card = game.seats[s].hand.cards[i];
		if (!show_fronts) {
			card.suit = UNKNOWN_SUIT;
			card.face = UNKNOWN_FACE;
		}
		if (!show_backs)
			card.deck = UNKNOWN_DECK;

		if (write_card(fd, card) < 0)
			NET_ERROR(p);
	}
}

static void net_send_trick(player_t p, player_t winner)
{
	int fd = get_player_socket(p);

	if (write_opcode(fd, MSG_TRICK) < 0 ||
	    write_seat(fd,CONVERT_SEAT(game.players[winner].seat, p)) < 0)
		NET_ERROR(p);
}

void net_broadcast_trick(player_t winner)
{
	broadcast_iterate(p) {
		net_send_trick(p, winner);
	} broadcast_iterate_end;
}

void net_send_newgame_request(player_t p)
{
	int fd = get_player_socket(p);

	assert(get_player_status(p) == GGZ_SEAT_PLAYER);

	ggz_debug(DBG_NET, "Sending out a REQ_NEWGAME to player %d/%s.", p,
		    get_player_name(p));
	if (write_opcode(fd, REQ_NEWGAME) < 0)
		NET_ERROR(p);
}

void net_send_newgame(player_t p)
{
	int fd = get_player_socket(p);
	
	if (write_opcode(fd, MSG_NEWGAME) < 0
	    || ggz_write_int(fd, get_cardset_type()) < 0)
		NET_ERROR(p);
}

void net_broadcast_newgame(void)
{
	ggz_debug(DBG_NET, "Broadcasting a newgame message.");

	broadcast_iterate(p) {
		net_send_newgame(p);
	} broadcast_iterate_end;
}


static void net_send_newhand(player_t p)
{
	int fd = get_player_socket(p);
	
	if (write_opcode(fd, MSG_NEWHAND) < 0)
		NET_ERROR(p);
}


void net_broadcast_newhand(void)
{
	broadcast_iterate(p) {
		net_send_newhand(p);
	} broadcast_iterate_end;
}

void net_send_global_text_message(player_t p, const char *mark,
                             const char *message)
{
	int fd = get_player_socket(p);
	
	/* Special case - don't send to dead bots */
	if (fd < 0)
		return;
	
	assert(mark);
	
	if (message == NULL)
		message = "";	/* this happens sometimes (hmmm, really?
				   how?) */
	if (write_opcode(fd, MESSAGE_GAME) < 0 ||
	    write_opcode(fd, GAME_MESSAGE_TEXT) < 0 ||
	    ggz_write_string(fd, mark) < 0 ||
	    ggz_write_string(fd, message) < 0)
		NET_ERROR(p);
}

/* send_global_message_toall sends the truly global message to all players */
void net_broadcast_global_text_message(const char *mark, const char* message)
{
	broadcast_iterate(p) {
		net_send_global_text_message(p, mark, message);
	} broadcast_iterate_end;
}

void net_send_player_text_message(player_t p, seat_t s, const char *message)
{
	int fd = get_player_socket(p);
	
	assert(message);
	
	if (write_opcode(fd, MESSAGE_GAME) < 0 ||
	    write_opcode(fd, GAME_MESSAGE_PLAYER) < 0 ||
	    write_seat(fd, CONVERT_SEAT(s, p)) < 0 ||
	    ggz_write_string(fd, message) < 0)
		NET_ERROR(p);
}

void net_send_global_cardlist_message(player_t p, const char *mark, int *lengths,
                                 card_t ** cardlist)
{
	int fd = get_player_socket(p);
	int i;
	seat_t s_rel;

	assert(mark && cardlist && lengths);
	ggz_debug(DBG_NET, "Sending global cardlist message to player %d.",
		    p);
		
	if (write_opcode(fd, MESSAGE_GAME) < 0 ||
	    write_opcode(fd, GAME_MESSAGE_CARDLIST) < 0 ||
	    ggz_write_string(fd, mark) < 0)
		NET_ERROR(p);

	for (s_rel = 0; s_rel < game.num_seats; s_rel++) {
		seat_t s = UNCONVERT_SEAT(s_rel, p);
		if (ggz_write_int(fd, lengths[s]) < 0)
			NET_ERROR(p);
		for (i = 0; i < lengths[s]; i++)
			if (write_card(fd, cardlist[s][i]) < 0)
				NET_ERROR(p);
	}
}

void net_broadcast_global_cardlist_message(const char *mark, int *lengths,
					   card_t ** cardlist)
{
	broadcast_iterate(p) {
		net_send_global_cardlist_message(p, mark, lengths, cardlist);
	} broadcast_iterate_end;
}


/*
 * NETWORK READING CODE
 */

static int net_rec_language(player_t p)
{
	int fd = get_player_socket(p);
	char lang[128];
	
	/* Read the language (string) */
	if (ggz_read_string(fd, lang, sizeof(lang)) < 0)
		return -1;
		
	handle_client_language(p, lang);
	return 0;
}


/* receives a bid from the client, and calls handle_client_bid
   to handle it.   Returns 0 on success; -1 on (communication) error */
static int net_rec_play(player_t p)
{
	int fd = get_player_socket(p);
	card_t card;

	/* read the card played */
	if (read_card(fd, &card) < 0)
		return -1;
		
	handle_client_play(p, card);
	return 0;
}

/* Receive a bid from an arbitrary player, and call another function
   to handle it. */
static int net_rec_bid(player_t p)
{
	int fd = get_player_socket(p);
	int bid_choice;

	/* Receive the bid index */
	if (ggz_read_int(fd, &bid_choice) < 0)
		return -1;
		
	handle_client_bid(p, bid_choice);
	return 0;
}

static int net_rec_options(player_t p)
{
	int fd = get_player_socket(p);
	int status = 0;
	int num_options, *options, i;
	
	if (ggz_read_int(fd, &num_options) < 0
	    || num_options <= 0)
		return -1;
		
	/* FIXME: this is a major security hole.  */
	options = ggz_malloc(num_options * sizeof(*options));
	
	for (i = 0; i < num_options; i++)
		if (ggz_read_int(fd, &options[i]) < 0)
			status = -1;
	
	if (status == 0)	
		handle_client_options(p, num_options, options);
	ggz_free(options);
	return status;
}

	
void net_read_player_data(player_t p)
{
	int fd = get_player_socket(p);
	int status = 0;
	int opcode;
	client_msg_t op;

	if (read_opcode(fd, &opcode) < 0)
		NET_ERROR(p);
	op = opcode;

	ggz_debug(DBG_NET, "Received %d (%s) from player %d/%s.",
	            op, get_client_opcode_name(op), p, get_player_name(p));

	switch (op) {
	case MSG_LANGUAGE:
		status = net_rec_language(p);
		break;
	case RSP_NEWGAME:
		status = 0;
		handle_client_newgame(p);
		break;
	case RSP_OPTIONS:
		status = net_rec_options(p);
		break;
	case RSP_BID:
		status = net_rec_bid(p);
		break;
	case RSP_PLAY:
		status = net_rec_play(p);
		break;
	case REQ_SYNC:
		status = 0;
		handle_client_sync(p);
		break;
	default:
		/* Unrecognized opcode */
		ggz_debug(DBG_CLIENT,
			  "game_handle_player: unrecognized opcode %d.",
			  op);
		status = -1;
		break;
	}

	if (status != 0)
		NET_ERROR(p);
}
