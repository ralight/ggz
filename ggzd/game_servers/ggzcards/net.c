/* 
 * File: net.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: Game-independent game network functions
 * $Id: net.c 3742 2002-04-05 05:29:49Z jdorje $
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

#include "common.h"
#include "message.h"
#include "net.h"
#include "play.h"

#ifdef USE_GGZ_STATS		/* defined in common.h */
# include "../../ggzdmod/ggz_stats.h"
#endif /* USE_GGZ_STATS */


/* Send out player ist to player p */
/** A player list packet is composed of:
 *    - The MSG_PLAYERS opcode
 *    - An integer containing the number of seats
 *    - For each seat, in *relative* ordering to
 *      the player being sent the message:
 *        - An integer that is the GGZ status of the seat
 *        - A string that is the name of the player at the seat
 */
static int send_player_list(player_t p)
{
	int fd = get_player_socket(p);
	seat_t s_rel;
	int status = 0;

	ggzdmod_log(game.ggz, "Sending seat list to player %d/%s (%d seats)",
		    p, get_player_name(p), game.num_seats);

	if (write_opcode(fd, MSG_PLAYERS) < 0 ||
	    ggz_write_int(fd, game.num_seats) < 0)
		status = -1;

	/* Note that this function can be called before we know what game
	   we're playing.  In this case, we'll know the number of players
	   (and possibly even who some of them are) but not the number of
	   seats.  Currently this will result in the number of seats being
	   set to zero and no data being sent out.  Later it might be
	   desirable to finesse data by sending the player list instead. */
	for (s_rel = 0; s_rel < game.num_seats; s_rel++) {
		seat_t s_abs = UNCONVERT_SEAT(s_rel, p);
		if (ggz_write_int(fd, get_seat_status(s_abs)) < 0 ||
		    ggz_write_string(fd, get_seat_name(s_abs)) < 0)
			status = -1;
	}

	if (status != 0)
		ggzdmod_log(game.ggz,
			    "ERROR: send_player_list: status is %d.", status);
	return status;
}

void broadcast_player_list(void)
{
	player_t p;
	
	for (p = 0; p < game.num_players; p++) {
		/* Sometimes this means we'll send the list to a player who's
		   not connected or a bot that hasn't been spawned yet.  This
		   isn't great.  Note, though, that we *do* need to send a
		   real player list to bot players, who may be relying on this
		   information. */
		(void) send_player_list(p);
	}
}

/* Send out play for player to _all_ players. Also symbolizes that this play
   is over. */
/** A play packet is composed of:
 *    - A MSG_PLAY opcode
 *    - A seat that is the relative seat of the player playing
 *    - A card that is the card being played
 */
static int send_play(player_t p, seat_t player, card_t card)
{
	int fd = get_player_socket(p);
	
	if (write_opcode(fd, MSG_PLAY) < 0 ||
	    write_seat(fd, CONVERT_SEAT(player, p)) < 0 ||
	    write_card(fd, card) < 0)
		return -1;
	return 0;
}

void broadcast_play(seat_t player, card_t card)
{
	player_t p;
	
	for (p = 0; p < game.num_players; p++)
		(void) send_play(p, player, card);
}

/* Send a gameover to all players. cnt is the number of winners, plist is the
   winner list */
/** A gameover packet is composed of:
 *    - A MSG_GAMEOVER opcode
 *    - An integer that is the number of winners
 *    - For each winning player:
 *        - A seat that is the relative seat of the player
 */
int send_gameover(int winner_cnt, player_t * winners)
{
	player_t p;
	int i, fd;
	int status = 0;

	ggzdmod_log(game.ggz, "Sending out game-over message.");


#ifdef USE_GGZ_STATS		/* defined in common.h */
	/* calculate new player ratings */
	/* FIXME: this shouldn't be handled here.  It should be handled in
	   the calling function. */
	for (i = 0; i < winner_cnt; i++)
		ggzd_set_game_winner(game.ggz, winners[i],
				     1.0 / (double) winner_cnt);
	if (ggzd_recalculate_ratings(game.ggz) < 0) {
		ggzdmod_log(game.ggz, "ERROR: couldn't recalculate ratings.");
	}
#endif /* USE_GGZ_STATS */

	for (p = 0; p < game.num_players; p++) {
		set_player_message(p);	/* some data could have changed */

		fd = get_player_socket(p);

		if (write_opcode(fd, MSG_GAMEOVER) < 0 ||
		    ggz_write_int(fd, winner_cnt) < 0)
			status = -1;
		for (i = 0; i < winner_cnt; i++) {
			seat_t ws = game.players[winners[i]].seat;
			if (write_seat(fd, CONVERT_SEAT(ws, p)) < 0)
				status = -1;
		}
	}

	if (status != 0)
		ggzdmod_log(game.ggz, "Error: send_game_over: status is %d.",
			    status);
	return status;
}

/* sends the list of cards on the table, in their relative orderings */
int send_table(player_t p)
{
	int s_r, s_abs, status = 0, fd = get_player_socket(p);

	if (game.num_seats == 0)
		return 0;

	ggzdmod_log(game.ggz, "Sending table to player %d/%s.", p,
		    get_player_name(p));

	if (write_opcode(fd, MSG_TABLE) < 0)
		status = -1;
	for (s_r = 0; s_r < game.num_seats; s_r++) {
		s_abs = UNCONVERT_SEAT(s_r, p);
		if (write_card(fd, game.seats[s_abs].table) < 0)
			status = -1;
	}

	if (status != 0)
		ggzdmod_log(game.ggz, "ERROR: send_table: status is %d.",
			    status);
	return status;
}

/* Send out current game hand, score, etc.  Doesn't use its own protocol, but
   calls on others */
int send_sync(player_t p)
{
	seat_t s;
	int status = 0;

	ggzdmod_log(game.ggz, "Sending sync to player %d/%s.", p,
		    get_player_name(p));

	if (send_player_list(p) < 0)
		status = -1;

	/* send out messages again */
	send_all_global_messages(p);
	for (s = 0; s < game.num_seats; s++)
		send_player_message(s, p);

	/* Send out hands */
	if (game.state != STATE_NOTPLAYING)
		for (s = 0; s < game.num_seats; s++)
			if (game.funcs->send_hand(p, s) < 0)
				status = -1;

	/* Send out table cards */
	if (send_table(p) < 0)
		status = -1;

	/* request bid/play again, if necessary */
	if (game.players[p].bid_data.is_bidding) {
		/* We can't call req_bid, because it has side effects (like
		   changing the game's state). */
		if (send_bid_request
		    (p, game.players[p].bid_data.bid_count,
		     game.players[p].bid_data.bids) < 0)
			status = -1;
	}
	if (game.state == STATE_WAIT_FOR_PLAY && game.players[p].is_playing)
		if (send_play_request(p, game.players[p].play_seat) < 0)
			status = -1;

	if (status != 0)
		ggzdmod_log(game.ggz, "ERROR: send_sync: status is %d.",
			    status);
	return status;
}

void broadcast_sync(void)
{
	player_t p;

	for (p = 0; p < game.num_players; p++)
		(void) send_sync(p);
}

/* Request a bid from player p.  bid_count is the number of bids; bids is an
   array listing the possible bids. */
int send_bid_request(player_t p, int bid_count, bid_t * bids)
{
	int i, status = 0;
	int fd = get_player_socket(p);
	GGZSeatType seat_type = get_player_status(p);

	ggzdmod_log(game.ggz, "Sending bid request to player %d/%s.", p,
		    get_player_name(p));

	/* request a bid from the client */
	if (fd == -1 ||
	    write_opcode(fd, REQ_BID) < 0 || ggz_write_int(fd, bid_count) < 0)
		status = -1;
	for (i = 0; i < bid_count; i++) {
		char bid_text[128] = "";
		char bid_desc[1024] = "";
		if (seat_type != GGZ_SEAT_BOT) {
			game.funcs->get_bid_text(bid_text, sizeof(bid_text),
			                         bids[i]);
			game.funcs->get_bid_desc(bid_desc, sizeof(bid_desc),
			                         bids[i]);
		}
		if (write_bid(fd, bids[i]) < 0 ||
		    ggz_write_string(fd, bid_text) < 0 ||
		    ggz_write_string(fd, bid_desc) < 0)
			status = -1;
	}
	return status;
}

static int send_bid(player_t p, player_t bidder, bid_t bid)
{
	seat_t seat = game.players[bidder].seat;
	int fd = get_player_socket(p);
	
	if (write_opcode(fd, MSG_BID) < 0 ||
	    write_seat(fd, CONVERT_SEAT(seat, p)) < 0 ||
	    write_bid(fd, bid) < 0)
		return -1;
	return 0;
	    	
}

void broadcast_bid(player_t bidder, bid_t bid)
{
	player_t p;
	
	for (p = 0; p < game.num_players; p++)
		(void) send_bid(p, bidder, bid);
}

/* Request a player p to make a play from a specific seat s's hand */
int send_play_request(player_t p, seat_t s)
{
	seat_t s_r = CONVERT_SEAT(s, p);
	int fd = get_player_socket(p);
	
	assert(game.players[p].is_playing);

	ggzdmod_log(game.ggz, "Requesting player %d/%s "
		    "to play from seat %d/%s's hand.", p,
		    get_player_name(p), s, get_seat_name(s));

	if (write_opcode(fd, REQ_PLAY) < 0 || write_seat(fd, s_r) < 0)
		return -1;

	return 0;
}

int send_badplay(player_t p, char *msg)
{
	int fd = get_player_socket(p);
	set_game_state(STATE_WAIT_FOR_PLAY);
	if (write_opcode(fd, MSG_BADPLAY) < 0 || ggz_write_string(fd, msg) < 0)
		return -1;
	return 0;
}

/* Show a player a hand.  This will reveal the cards in the hand iff reveal
   is true. */
int send_hand(const player_t p, const seat_t s, int reveal)
{
	int fd = get_player_socket(p);
	int i, status = 0;

	/* We used to refuse to send hands of size 0, but some games may need
	   to do this! */
	assert(game.state != STATE_NOTPLAYING);

	/* The open_hands option causes everyone's hand to always be
	   revealed. */
	if (game.open_hands)
		reveal = 1;

	ggzdmod_log(game.ggz,
		    "Sending player %d/%d/%s hand %d/%s - %srevealing", p,
		    game.players[p].seat, get_player_name(p), s,
		    get_seat_name(s), reveal ? "" : "not ");

	if (write_opcode(fd, MSG_HAND) < 0
	    || write_seat(fd, CONVERT_SEAT(s, p)) < 0
	    || ggz_write_int(fd, game.seats[s].hand.hand_size) < 0)
		status = -1;

	for (i = 0; i < game.seats[s].hand.hand_size; i++) {
		card_t card;
		if (reveal)
			card = game.seats[s].hand.cards[i];
		else
			card = UNKNOWN_CARD;
		if (write_card(fd, card) < 0)
			status = -1;
	}

	if (status != 0)
		ggzdmod_log(game.ggz,
			    "ERROR: " "send_hand: communication error.");
	return status;
}

int send_trick(player_t winner)
{
	int fd, status = 0;
	player_t p;
	seat_t s;

	ggzdmod_log(game.ggz,
		    "Sending out trick (%d/%s won) and cleaning it up.",
		    winner, get_player_name(winner));

	for (s = 0; s < game.num_seats; s++)
		/* note: we also clear the table at the beginning of every
		   hand */
		game.seats[s].table = UNKNOWN_CARD;

	for (p = 0; p < game.num_players; p++) {
		fd = get_player_socket(p);

		if (write_opcode(fd, MSG_TRICK) < 0 ||
		    write_seat(fd,
			       CONVERT_SEAT(game.players[winner].seat,
					    p)) < 0)
			status = -1;
	}

	return 0;
}

int send_newgame_request(player_t p)
{
	int fd, status;
	fd = get_player_socket(p);

	ggzdmod_log(game.ggz, "Sending out a REQ_NEWGAME to player %d/%s.", p,
		    get_player_name(p));
	status = write_opcode(fd, REQ_NEWGAME);

	if (status != 0)
		ggzdmod_log(game.ggz, "ERROR: "
			    "req_newgame: status is %d for player %d/%s.",
			    status, p, get_player_name(p));
	return status;
}

int send_newgame(player_t p)
{
	int fd = get_player_socket(p);
	
	return write_opcode(fd, MSG_NEWGAME);
}

void broadcast_newgame(void)
{
	player_t p;

	ggzdmod_log(game.ggz, "Broadcasting a newgame message.");

	for (p = 0; p < game.num_players; p++)
		(void) send_newgame(p);
}


static int send_newhand(player_t p)
{
	int fd = get_player_socket(p);
	
	return write_opcode(fd, MSG_NEWHAND);
}


void broadcast_newhand(void)
{
	player_t p;
	
	for (p = 0; p < game.num_players; p++)
		(void) send_newhand(p);
}

int send_global_text_message(player_t p, const char *mark,
                             const char *message)
{
	int fd = get_player_socket(p);
	
	assert(mark);
	
	if (message == NULL)
		message = "";	/* this happens sometimes (hmmm, really?
				   how?) */
	if (write_opcode(fd, MESSAGE_GAME) < 0 ||
	    write_opcode(fd, GAME_MESSAGE_TEXT) < 0 ||
	    ggz_write_string(fd, mark) < 0 ||
	    ggz_write_string(fd, message) < 0)
		return -1;
	return 0;
}

/* send_global_message_toall sends the truly global message to all players */
void broadcast_global_text_message(const char *mark, const char* message)
{
	player_t p;
	
	for (p = 0; p < game.num_players; p++)
		if (get_player_socket(p) >= 0)
			(void) send_global_text_message(p, mark, message);
}

int send_global_cardlist_message(player_t p, const char *mark, int *lengths,
                                 card_t ** cardlist)
{
	int fd = get_player_socket(p);
	int status = 0, i;
	seat_t s_rel;

	assert(mark && cardlist && lengths);
	ggzdmod_log(game.ggz, "Sending global cardlist message to player %d.",
		    p);
		
	if (write_opcode(fd, MESSAGE_GAME) < 0 ||
	    write_opcode(fd, GAME_MESSAGE_CARDLIST) < 0 ||
	    ggz_write_string(fd, mark) < 0)
		status = -1;

	for (s_rel = 0; s_rel < game.num_seats; s_rel++) {
		seat_t s = UNCONVERT_SEAT(s_rel, p);
		if (ggz_write_int(fd, lengths[s]) < 0)
			status = -1;
		for (i = 0; i < lengths[s]; i++)
			if (write_card(fd, cardlist[s][i]) < 0)
				status = -1;
	}

	return status;
}

void broadcast_global_cardlist_message(const char *mark, int *lengths,
				       card_t ** cardlist)
{
	player_t p;
	for (p = 0; p < game.num_players; p++)
		(void) send_global_cardlist_message(p, mark, lengths, cardlist);
}


int rec_language(player_t p)
{
	char *lang;
	int fd = get_player_socket(p);
	
	if (ggz_read_string_alloc(fd, &lang) < 0)
		return -1;
		
	handle_player_language(p, lang);
	
	ggz_free(lang);
	
	return 0;
}


/* receives a play from the client, and calls update if necessary.  */
int rec_play(player_t p)
{
	int fd = get_player_socket(p);
	card_t card;

	/* read the card played */
	if (read_card(fd, &card) < 0)
		return -1;
		
	handle_client_play(p, card);
	return 0;
}
