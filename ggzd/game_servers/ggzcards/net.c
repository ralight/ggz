/* 
 * File: net.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: Game-independent game network functions
 * $Id: net.c 8552 2006-08-30 06:03:56Z jdorje $
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
#  include <config.h>	/* Site-specific config */
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <ggz.h>

#include "net_common.h"

#include "bid.h"
#include "common.h"
#include "message.h"
#include "net.h"
#include "options.h"
#include "play.h"
#include "team.h"

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
	if (get_player_dio(p))
		return TRUE;

#if 0	/* This is now possible because of non-atomic seat operations. */
	assert(p >= 0 || get_player_name(p) == NULL);
	assert(get_player_status(p) != GGZ_SEAT_PLAYER);
#endif

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
	GGZDataIO *dio = get_player_dio(p);
	seat_t s_rel;

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_PLAYERS);
	ggz_dio_put_int(dio, game.num_seats);
	ggz_dio_put_int(dio, game.num_players);
	ggz_dio_put_int(dio, game.num_teams);

	/* Note that this function can be called before we know what game
	   we're playing.  In this case, we'll know the number of players
	   (and possibly even who some of them are) but not the number of
	   seats.  Currently this will result in the number of seats being
	   set to zero and no data being sent out.  Later it might be
	   desirable to finesse data by sending the player list instead. */
	for (s_rel = 0; s_rel < game.num_seats; s_rel++) {
		seat_t s_abs = UNCONVERT_SEAT(s_rel, p);

		ggz_dio_put_int(dio, get_seat_status(s_abs));
		ggz_dio_put_string(dio, get_seat_name(s_abs));
		ggz_dio_put_int(dio, game.seats[s_abs].player);
		ggz_dio_put_int(dio, get_seat_team(s_abs));
	}
	ggz_dio_packet_end(dio);
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

void net_send_scores(player_t p, int hand_num)
{
	GGZDataIO *dio = get_player_dio(p);
	int i;

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_SCORES);
	ggz_dio_put_int(dio, hand_num);
	teams_iterate(t) {
		score_data_t score = game.teams[t].scores[hand_num];

		ggz_dio_put_int(dio, score.score);
		for (i = 0; i < SCORE_EXTRAS; i++) {
			ggz_dio_put_int(dio, score.extra[i]);
		}
	}
	teams_iterate_end;
	ggz_dio_packet_end(dio);
}

void net_send_tricks_count(player_t p)
{
	GGZDataIO *dio = get_player_dio(p);
	seat_t s_r, s_abs;
	int tricks;

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_TRICKS_COUNT);
	for (s_r = 0; s_r < game.num_seats; s_r++) {
		s_abs = UNCONVERT_SEAT(s_r, p);
		if (game.seats[s_abs].player >= 0) {
			tricks =
			    game.players[game.seats[s_abs].player].tricks;
		} else {
			tricks = -1;
		}
		ggz_dio_put_int(dio, tricks);
	}
	ggz_dio_packet_end(dio);
}

void net_send_all_scores(player_t p)
{
	int i;

	for (i = 0; i < game.hand_num; i++) {
		net_send_scores(p, i);
	}
}

void net_broadcast_scores(int hand_num)
{
	broadcast_iterate(p) {
		net_send_scores(p, hand_num);
	} broadcast_iterate_end;
}

void net_send_options_request(player_t p,
			      int num_options,
			      char **option_types,
			      char **option_descs,
			      int *num_choices,
			      int *option_defaults, char ***option_choices)
{
	GGZDataIO *dio = get_player_dio(p);
	int i, j;

	ggz_dio_packet_start(dio);
	write_opcode(dio, REQ_OPTIONS);
	ggz_dio_put_int(dio, num_options);
	for (i = 0; i < num_options; i++) {
		ggz_dio_put_string(dio, option_types[i]);
		ggz_dio_put_string(dio, option_descs[i]);
		ggz_dio_put_int(dio, num_choices[i]);
		ggz_dio_put_int(dio, option_defaults[i]);
		for (j = 0; j < num_choices[i]; j++) {
			ggz_dio_put_string(dio, option_choices[i][j]);
		}
	}
	ggz_dio_packet_end(dio);
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
	GGZDataIO *dio = get_player_dio(p);

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_PLAY);
	write_seat(dio, CONVERT_SEAT(player, p));
	write_card(dio, card);
	ggz_dio_packet_end(dio);
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
static void net_send_gameover(player_t p, int winner_cnt,
			      player_t * winners)
{
	GGZDataIO *dio = get_player_dio(p);
	int i;

	assert(winner_cnt >= 0 && winner_cnt <= game.num_players);

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_GAMEOVER);
	ggz_dio_put_int(dio, winner_cnt);

	for (i = 0; i < winner_cnt; i++) {
		seat_t ws = game.players[winners[i]].seat;

		write_seat(dio, CONVERT_SEAT(ws, p));
	}
	ggz_dio_packet_end(dio);
}

void net_broadcast_gameover(int winner_cnt, player_t * winners)
{
	broadcast_iterate(p) {
		net_send_gameover(p, winner_cnt, winners);
	} broadcast_iterate_end;
}

/* sends the list of cards on the table, in their relative orderings */
void net_send_table(player_t p)
{
	seat_t s_r, s_abs;
	GGZDataIO *dio = get_player_dio(p);

	if (game.num_seats == 0)	/* FIXME: don't check this here */
		return;

	ggz_debug(DBG_NET, "Sending table to player %d/%s.", p,
		  get_player_name(p));

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_TABLE);
	for (s_r = 0; s_r < game.num_seats; s_r++) {
		s_abs = UNCONVERT_SEAT(s_r, p);
		write_card(dio, game.seats[s_abs].table);
	}
	ggz_dio_packet_end(dio);
}

/* Request a bid from player p.  bid_count is the number of bids; bids is an
   array listing the possible bids. */
void net_send_bid_request(player_t p, int bid_count, bid_t * bids)
{
	int i;
	GGZDataIO *dio = get_player_dio(p);
	GGZSeatType seat_type = get_player_status(p);

	ggz_debug(DBG_NET, "Sending bid request to player %d/%s.", p,
		  get_player_name(p));

	/* request a bid from the client */
	ggz_dio_packet_start(dio);
	write_opcode(dio, REQ_BID);
	ggz_dio_put_int(dio, bid_count);
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
		write_bid(dio, bids[i]);
		ggz_dio_put_string(dio, bid_text);
		ggz_dio_put_string(dio, bid_desc);
	}
	ggz_dio_packet_end(dio);
}

void net_send_bid(player_t p, player_t bidder, bid_t bid)
{
	seat_t seat = game.players[bidder].seat;
	GGZDataIO *dio = get_player_dio(p);

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_BID);
	write_seat(dio, CONVERT_SEAT(seat, p));
	write_bid(dio, bid);
	ggz_dio_packet_end(dio);
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
	GGZDataIO *dio = get_player_dio(p);

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

	ggz_dio_packet_start(dio);
	write_opcode(dio, REQ_PLAY);
	write_seat(dio, s_r);
	ggz_dio_put_int(dio, num_valid_plays);
	for (i = 0; i < num_valid_plays; i++)
		write_card(dio, valid_plays[i]);
	ggz_dio_packet_end(dio);
}

void net_send_badplay(player_t p, char *msg)
{
	GGZDataIO *dio = get_player_dio(p);

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_BADPLAY);
	ggz_dio_put_string(dio, msg);
	ggz_dio_packet_end(dio);
}

/* Show a player a hand.  This will reveal the cards in the hand iff reveal
   is true. */
void net_send_hand(const player_t p, const seat_t s,
		   bool show_fronts, bool show_backs)
{
	GGZDataIO *dio = get_player_dio(p);
	int i;

	/* This can happen with the weird broadcast rules - see
	   game.data->send_hand() calls in common.c. */
	if (!dio)
		return;

	ggz_debug(DBG_NET,
		  "Sending player %d/%d/%s hand %d/%s - revealing(%d,%d)",
		  p, PLAYER_TO_SEAT(p), get_player_name(p), s,
		  get_seat_name(s), show_fronts, show_backs);

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_HAND);
	write_seat(dio, CONVERT_SEAT(s, p));
	ggz_dio_put_int(dio, game.seats[s].hand.hand_size);

	for (i = 0; i < game.seats[s].hand.hand_size; i++) {
		card_t card = game.seats[s].hand.cards[i];
		if (!show_fronts) {
			card.suit = UNKNOWN_SUIT;
			card.face = UNKNOWN_FACE;
		}
		if (!show_backs)
			card.deck = UNKNOWN_DECK;

		write_card(dio, card);
	}
	ggz_dio_packet_end(dio);
}

void net_send_players_status(player_t p)
{
	GGZDataIO *dio = get_player_dio(p);
	int bidding = 0, playing = 0;
	seat_t s_rel, s_abs;

	for (s_rel = 0; s_rel < game.num_seats; s_rel++) {
		s_abs = UNCONVERT_SEAT(s_rel, p);
		player_t p2 = game.seats[s_abs].player;

		if (p2 >= 0) {
			if (game.players[p2].bid_data.is_bidding) {
				bidding |= 1 << s_rel;
			}
			if (game.players[p2].is_playing) {
				playing |= 1 << s_rel;
			}
		}
	}

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_PLAYERS_STATUS);
	ggz_dio_put_int(dio, bidding);
	ggz_dio_put_int(dio, playing);
	ggz_dio_packet_end(dio);
}

void net_broadcast_players_status(void)
{

	broadcast_iterate(p) {
		net_send_players_status(p);
	} broadcast_iterate_end;
}

static void net_send_trick(player_t p, player_t winner)
{
	GGZDataIO *dio = get_player_dio(p);

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_TRICK);
	write_seat(dio, CONVERT_SEAT(game.players[winner].seat, p));
	ggz_dio_packet_end(dio);
}

void net_broadcast_trick(player_t winner)
{
	broadcast_iterate(p) {
		net_send_trick(p, winner);
	} broadcast_iterate_end;
}

void net_send_newgame_request(player_t p)
{
	GGZDataIO *dio = get_player_dio(p);

	assert(get_player_status(p) == GGZ_SEAT_PLAYER);

	ggz_debug(DBG_NET, "Sending out a REQ_NEWGAME to player %d/%s.", p,
		  get_player_name(p));

	ggz_dio_packet_start(dio);
	write_opcode(dio, REQ_NEWGAME);
	ggz_dio_packet_end(dio);
}

void net_send_newgame(player_t p)
{
	GGZDataIO *dio = get_player_dio(p);

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_NEWGAME);
	ggz_dio_put_string(dio, game.data->name);
	ggz_dio_put_int(dio, get_cardset_type());
	ggz_dio_packet_end(dio);
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
	GGZDataIO *dio = get_player_dio(p);

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_NEWHAND);
	ggz_dio_put_int(dio, game.hand_num);
	ggz_dio_put_char(dio, game.trump);
	ggz_dio_packet_end(dio);
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
	GGZDataIO *dio = get_player_dio(p);

	/* Special case - don't send to dead bots */
	if (!dio)
		return;

	assert(mark);

	if (message == NULL)
		message = "";	/* this happens sometimes (hmmm, really?
				   how?) */

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_GAME_MESSAGE_TEXT);
	ggz_dio_put_string(dio, mark);
	ggz_dio_put_string(dio, message);
	ggz_dio_packet_end(dio);
}

/* send_global_message_toall sends the truly global message to all players */
void net_broadcast_global_text_message(const char *mark,
				       const char *message)
{
	broadcast_iterate(p) {
		net_send_global_text_message(p, mark, message);
	} broadcast_iterate_end;
}

void net_send_player_text_message(player_t p, seat_t s,
				  const char *message)
{
	GGZDataIO *dio = get_player_dio(p);

	assert(message);

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_GAME_MESSAGE_PLAYER);
	write_seat(dio, CONVERT_SEAT(s, p));
	ggz_dio_put_string(dio, message);
	ggz_dio_packet_end(dio);
}

void net_send_global_cardlist_message(player_t p, const char *mark,
				      int *lengths, card_t ** cardlist)
{
	GGZDataIO *dio = get_player_dio(p);
	int i;
	seat_t s_rel;

	assert(mark && cardlist && lengths);
	ggz_debug(DBG_NET, "Sending global cardlist message to player %d.",
		  p);

	ggz_dio_packet_start(dio);
	write_opcode(dio, MSG_GAME_MESSAGE_CARDLIST);
	ggz_dio_put_string(dio, mark);

	for (s_rel = 0; s_rel < game.num_seats; s_rel++) {
		seat_t s = UNCONVERT_SEAT(s_rel, p);
		ggz_dio_put_int(dio, lengths[s]);
		for (i = 0; i < lengths[s]; i++)
			write_card(dio, cardlist[s][i]);
	}
	ggz_dio_packet_end(dio);
}

void net_broadcast_global_cardlist_message(const char *mark, int *lengths,
					   card_t ** cardlist)
{
	broadcast_iterate(p) {
		net_send_global_cardlist_message(p, mark, lengths,
						 cardlist);
	} broadcast_iterate_end;
}


/*
 * NETWORK READING CODE
 */

static void net_rec_language(player_t p)
{
	GGZDataIO *dio = get_player_dio(p);
	char lang[128];

	/* Read the language (string) */
	ggz_dio_get_string(dio, lang, sizeof(lang));

	handle_client_language(p, lang);
}


/* receives a bid from the client, and calls handle_client_bid
   to handle it.   Returns 0 on success; -1 on (communication) error */
static void net_rec_play(player_t p)
{
	GGZDataIO *dio = get_player_dio(p);
	card_t card;

	/* read the card played */
	read_card(dio, &card);

	handle_client_play(p, card);
}

/* Receive a bid from an arbitrary player, and call another function
   to handle it. */
static void net_rec_bid(player_t p)
{
	GGZDataIO *dio = get_player_dio(p);
	int bid_choice;

	/* Receive the bid index */
	ggz_dio_get_int(dio, &bid_choice);

	handle_client_bid(p, bid_choice);
}

static void net_rec_options(player_t p)
{
	GGZDataIO *dio = get_player_dio(p);
	int num_options;
	const int max_options = get_num_pending_options();

	ggz_dio_get_int(dio, &num_options);

	ggz_debug(DBG_MISC, "Reading %d options (max %d).",
		  num_options, max_options);
	num_options = CLIP(0, num_options, get_num_pending_options());
	{
		int options[num_options], i;

		for (i = 0; i < num_options; i++)
			ggz_dio_get_int(dio, &options[i]);

		handle_client_options(p, num_options, options);
	}
}


static void net_read_player_data_cb(GGZDataIO * dio, void *userdata)
{
	player_t *p = userdata;
	int opcode;
	client_msg_t op;

	read_opcode(dio, &opcode);
	op = opcode;

	ggz_debug(DBG_NET, "Received %d (%s) from player %d/%s.",
		  op, get_client_opcode_name(op), *p, get_player_name(*p));

	switch (op) {
	case MSG_LANGUAGE:
		net_rec_language(*p);
		return;
	case RSP_NEWGAME:
		handle_client_newgame(*p);
		return;
	case RSP_OPTIONS:
		net_rec_options(*p);
		return;
	case RSP_BID:
		net_rec_bid(*p);
		return;
	case RSP_PLAY:
		net_rec_play(*p);
		return;
	case REQ_SYNC:
		handle_client_sync(*p);
		return;
	}

	/* Unrecognized opcode */
	ggz_debug(DBG_CLIENT,
		  "game_handle_player: unrecognized opcode %d.", op);
}

void net_read_player_data(player_t p)
{
	GGZDataIO *dio = get_player_dio(p);

	ggz_dio_set_read_callback(dio, net_read_player_data_cb, &p);
	ggz_dio_read_data(dio);
}
