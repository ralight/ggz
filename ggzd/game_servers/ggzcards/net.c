/* 
 * File: net.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: Game-independent game network functions
 * $Id: net.c 2730 2001-11-13 06:29:00Z jdorje $
 *
 * This file contains code that controls the flow of a general
 * trick-taking game.  Game states, event handling, etc. are all
 * handled within this code.  For specialty handling we call a
 * game-specific function from game.c or games/<game>.c.
 *
 * Copyright (C) 2001 GGZ Dev Team.
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

#include <config.h>		/* Site-specific config */

#include <assert.h>

#include <easysock.h>
#include "../../ggzdmod/ggz_server.h"
#include "../../ggzdmod/ggz_stats.h"	/* FIXME */

#include "common.h"


/* Send out player ist to player p */
/** A player list packet is composed of:
 *    - The WH_MSG_PLAYERS opcode
 *    - An integer containing the number of seats
 *    - For each seat, in *relative* ordering to
 *      the player being sent the message:
 *        - An integer that is the GGZ status of the seat
 *        - A string that is the name of the player at the seat
 */
int send_player_list(player_t p)
{
	int fd;
	seat_t s_rel;
	int status = 0;

	fd = ggzd_get_player_socket(p);
	if (fd == -1)
		return 0;

	ggzd_debug("Sending seat list to player %d/%s (%d seats)", p,
		   ggzd_get_player_name(p), game.num_seats);

	if (write_opcode(fd, WH_MSG_PLAYERS) < 0 ||
	    es_write_int(fd, game.num_seats) < 0)
		status = -1;

	/* Note that this function can be called before we know what game
	   we're playing.  In this case, we'll know the number of players
	   (and possibly even who some of them are) but not the number of
	   seats.  Currently this will result in the number of seats being
	   set to zero and no data being sent out.  Later it might be
	   desirable to finesse data by sending the player list instead. */
	for (s_rel = 0; s_rel < game.num_seats; s_rel++) {
		seat_t s_abs = UNCONVERT_SEAT(s_rel, p);
		if (es_write_int(fd, get_seat_status(s_abs)) < 0 ||
		    es_write_string(fd, get_seat_name(s_abs)) < 0)
			status = -1;
	}

	if (status != 0)
		ggzd_debug("ERROR: send_player_list: status is %d.", status);
	return status;
}

int broadcast_player_list(void)
{
	player_t p;
	int status = 0;
	for (p = 0; p < game.num_players; p++)
		if (send_player_list(p) < 0)
			status = -1;
	return status;
}

/* Send out play for player to _all_ players. Also symbolizes that this play
   is over. */
/** A play packet is composed of:
 *    - A WH_MSG_PLAY opcode
 *    - A seat that is the relative seat of the player playing
 *    - A card that is the card being played
 */
int send_play(card_t card, seat_t seat)
{
	player_t p;
	int fd;
	int status = 0;

	for (p = 0; p < game.num_players; p++) {
		fd = ggzd_get_player_socket(p);
		if (fd == -1)	/* don't send to bots */
			continue;
		if (write_opcode(fd, WH_MSG_PLAY) < 0
		    || write_seat(fd, CONVERT_SEAT(seat, p)) < 0
		    || write_card(fd, card) < 0)
			status = -1;
	}

	if (status != 0)
		ggzd_debug("ERROR: send_play: status is %d.", status);
	return status;
}

/* Send a gameover to all players. cnt is the number of winners, plist is the
   winner list */
/** A gameover packet is composed of:
 *    - A WH_MSG_GAMEOVER opcode
 *    - An integer that is the number of winners
 *    - For each winning player:
 *        - A seat that is the relative seat of the player
 */
int send_gameover(int winner_cnt, player_t * winners)
{
	player_t p;
	int i, fd;
	int status = 0;

	ggzd_debug("Sending out game-over message.");

	/* calculate new player ratings */
	/* FIXME: this shouldn't be handled here.  It should be handled in
	   the calling function. */
	for (i = 0; i < winner_cnt; i++)
		ggzd_set_game_winner(winners[i], 1.0 / (double) winner_cnt);
	if (ggzd_recalculate_ratings() < 0) {
		ggzd_debug("ERROR: couldn't recalculate ratings.");
	}

	for (p = 0; p < game.num_players; p++) {
		set_player_message(p);	/* some data could have changed */

		fd = ggzd_get_player_socket(p);
		if (fd == -1)
			continue;

		if (write_opcode(fd, WH_MSG_GAMEOVER) < 0 ||
		    es_write_int(fd, winner_cnt) < 0)
			status = -1;
		for (i = 0; i < winner_cnt; i++) {
			seat_t ws = game.players[winners[i]].seat;
			if (write_seat(fd, CONVERT_SEAT(ws, p)) < 0)
				status = -1;
		}
	}

	if (status != 0)
		ggzd_debug("Error: send_game_over: status is %d.", status);
	return status;
}

/* sends the list of cards on the table, in their relative orderings */
int send_table(player_t p)
{
	int s_r, s_abs, status = 0, fd = ggzd_get_player_socket(p);

	if (game.num_seats == 0)
		return 0;

	ggzd_debug("Sending table to player %d/%s.", p,
		   ggzd_get_player_name(p));

	if (fd == -1) {
		ggzd_debug("ERROR: send_table: fd==-1.");
		return -1;
	}

	if (write_opcode(fd, WH_MSG_TABLE) < 0)
		status = -1;
	for (s_r = 0; s_r < game.num_seats; s_r++) {
		s_abs = UNCONVERT_SEAT(s_r, p);
		if (write_card(fd, game.seats[s_abs].table) < 0)
			status = -1;
	}

	if (status != 0)
		ggzd_debug("ERROR: send_table: status is %d.", status);
	return status;
}

/* Send out current game hand, score, etc.  Doesn't use its own protocol, but
   calls on others */
int send_sync(player_t p)
{
	seat_t s;
	int status = 0;

	ggzd_debug("Sending sync to player %d/%s.", p,
		   ggzd_get_player_name(p));

	if (send_player_list(p) < 0)
		status = -1;

	/* send out messages again */
	send_all_global_messages(p);
	for (s = 0; s < game.num_seats; s++)
		send_player_message(s, p);

	/* Send out hands */
	for (s = 0; s < game.num_seats; s++)
		if (game.funcs->send_hand(p, s) < 0)
			status = -1;

	/* Send out table cards */
	if (send_table(p) < 0)
		status = -1;

	/* request bid/play again, if necessary */
	if (game.players[p].bid_data.is_bidding)
		if (req_bid(p) < 0)
			status = -1;
	if (game.state == WH_STATE_WAIT_FOR_PLAY && game.curr_play == p)
		if (send_play_request(game.curr_play, game.play_seat) < 0)
			status = -1;

	if (status != 0)
		ggzd_debug("ERROR: send_sync: status is %d.", status);
	return status;
}

int send_sync_all(void)
{
	player_t p;
	int status = 0;

	for (p = 0; p < game.num_players; p++)
		if (ggzd_get_seat_status(p) == GGZ_SEAT_PLAYER)
			if (send_sync(p) < 0)
				status = -1;

	return status;
}

/* Request a bid from player p.  bid_count is the number of bids; bids is an
   array listing the possible bids. */
int send_bid_request(player_t p, int bid_count, bid_t * bids)
{
	int i, status = 0;

	int fd = ggzd_get_player_socket(p);
	/* request a bid from the client */
	if (fd == -1 ||
	    write_opcode(fd, WH_REQ_BID) < 0 ||
	    es_write_int(fd, bid_count) < 0)
		status = -1;
	for (i = 0; i < bid_count; i++) {
		char bid_text[4096];
		game.funcs->get_bid_text(bid_text, sizeof(bid_text), bids[i]);
		if (es_write_string(fd, bid_text) < 0)
			status = -1;
	}
	return status;
}

/* Request a player p to make a play from a specific seat s's hand */
int send_play_request(player_t p, seat_t s)
{
	int fd = ggzd_get_player_socket(p);
	seat_t s_r = CONVERT_SEAT(s, p);

	ggzd_debug("Requesting player %d/%s "
		   "to play from seat %d/%s's hand.", p,
		   ggzd_get_player_name(p), s, get_seat_name(s));

	/* although the game_* functions probably track this data themselves,
	   we track it here as well just in case. */
	game.curr_play = p;
	game.play_seat = s;

	set_game_state(WH_STATE_WAIT_FOR_PLAY);
	set_player_message(p);

	if (ggzd_get_seat_status(p) == GGZ_SEAT_BOT) {
		/* request a play from the ai */
		handle_play_event(ai_get_play(p, s));
	} else {
		if (fd == -1)
			ggzd_debug("ERROR: SERVER BUG: " "-1 fd in req_play");
		if (write_opcode(fd, WH_REQ_PLAY) < 0
		    || write_seat(fd, s_r) < 0)
			return -1;
	}

	return 0;
}

int send_badplay(player_t p, char *msg)
{
	int fd = ggzd_get_player_socket(p);
	if (fd == -1)		/* don't send to bots */
		return 0;
	set_game_state(WH_STATE_WAIT_FOR_PLAY);
	if (write_opcode(fd, WH_MSG_BADPLAY) < 0 ||
	    es_write_string(fd, msg) < 0)
		return -1;
	return 0;
}

/* Show a player a hand.  This will reveal the cards in the hand iff reveal
   is true. */
int send_hand(const player_t p, const seat_t s, int reveal)
{
	int fd = ggzd_get_player_socket(p);
	int i, status = 0;

	if (fd == -1)		/* Don't send to a bot */
		return 0;

	if (game.seats[s].hand.hand_size == 0)
		return 0;

	/* The open_hands option causes everyone's hand to always be
	   revealed. */
	if (game.open_hands)
		reveal = 1;

	ggzd_debug("Sending player %d/%s hand %d/%s - %srevealing",
		   p, ggzd_get_player_name(p), s, get_seat_name(s),
		   reveal ? "" : "not ");

	if (write_opcode(fd, WH_MSG_HAND) < 0
	    || write_seat(fd, CONVERT_SEAT(s, p)) < 0
	    || es_write_int(fd, game.seats[s].hand.hand_size) < 0)
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
		ggzd_debug("ERROR: " "send_hand: communication error.");
	return status;
}

int send_trick(player_t winner)
{
	int fd, status = 0;
	player_t p;
	seat_t s;

	ggzd_debug("Sending out trick (%d/%s won) and cleaning it up.",
		   winner, ggzd_get_player_name(winner));

	for (s = 0; s < game.num_seats; s++)
		/* note: we also clear the table at the beginning of every
		   hand */
		game.seats[s].table = UNKNOWN_CARD;

	for (p = 0; p < game.num_players; p++) {
		fd = ggzd_get_player_socket(p);
		if (fd == -1)
			continue;

		if (write_opcode(fd, WH_MSG_TRICK) < 0 ||
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
	fd = ggzd_get_player_socket(p);
	if (fd == -1) {
		ggzd_debug("ERROR: "
			   "req_newgame: fd is -1 for player %d/%s.", p,
			   ggzd_get_player_name(p));
		return -1;
	}

	ggzd_debug("Sending out a WH_REQ_NEWGAME to player %d/%s.", p,
		   ggzd_get_player_name(p));
	status = write_opcode(fd, WH_REQ_NEWGAME);

	if (status != 0)
		ggzd_debug("ERROR: "
			   "req_newgame: status is %d for player %d/%s.",
			   status, p, ggzd_get_player_name(p));
	return status;
}

int send_newgame(void)
{
	int fd;
	player_t p;

	ggzd_debug("Sending out a newgame message.");

	for (p = 0; p < game.num_players; p++) {
		if (ggzd_get_seat_status(p) == GGZ_SEAT_BOT)
			continue;
		fd = ggzd_get_player_socket(p);
		if (fd == -1 || write_opcode(fd, WH_MSG_NEWGAME) < 0) {
			ggzd_debug
				("ERROR: send_newgame: couldn't send newgame.");
			return -1;
		}
	}

	return 0;
}


/* receives a play from the client, and calls update if necessary.  */
int rec_play(player_t p)
{
	int fd = ggzd_get_player_socket(p), i;
	card_t card;
	seat_t s = game.play_seat;
	hand_t *hand = &game.seats[s].hand;
	char *err;

	/* read the card played */
	if (read_card(fd, &card) < 0)
		return -1;

	/* are we waiting for a play? */
	if (game.state != WH_STATE_WAIT_FOR_PLAY) {
		ggzd_debug
			("SERVER/CLIENT BUG: we received a play (player %d/%s) when we weren't waiting for one.",
			 p, ggzd_get_player_name(p));
		return -1;
	}

	/* Is is this player's turn to play? */
	if (game.curr_play != p) {
		/* better to just ignore it; a WH_MSG_BADPLAY requests a new
		   play */
		ggzd_debug
			("SERVER/CLIENT BUG: player %d/%s played out of turn!?!?",
			 p, ggzd_get_player_name(p));
		return -1;
	}

	/* find the card played */
	for (i = 0; i < hand->hand_size; i++)
		if (cards_equal(hand->cards[i], card))
			break;

	if (i == hand->hand_size) {
		(void) send_badplay(p,
				    "That card isn't even in your hand."
				    "  This must be a bug.");
		ggzd_debug("CLIENT BUG: "
			   "player %d/%s played a card that wasn't in their hand (%i %i %i)!?!?",
			   p, ggzd_get_player_name(p), card.face, card.suit,
			   card.deck);
		return -1;
	}

	ggzd_debug("We received a play of card "
		   "(%d %d %d) from player %d/%s.", card.face, card.suit,
		   card.deck, p, ggzd_get_player_name(p));

	/* we've verified that this card could have physically been played;
	   we still need to check if it's a legal play Note, however, that we
	   don't return -1 on an error here.  An error returned indicates a
	   GGZ error, which is not what happened.  This is just a player
	   mistake */
	err = game.funcs->verify_play(card);
	if (err == NULL)
		/* any AI routine would also call handle_play_event, so the
		   ai must also check the validity as above.  This could be
		   changed... */
		handle_play_event(card);
	else
		(void) send_badplay(p, err);

	return 0;
}

void send_global_text_message(player_t p, const char *mark,
			      const char *message)
{
	int fd = ggzd_get_player_socket(p);
	if (ggzd_get_seat_status(p) == GGZ_SEAT_BOT
	    || ggzd_get_seat_status(p) == GGZ_SEAT_OPEN)
		return;
	assert(mark);
	if (message == NULL)
		message = "";	/* this happens sometimes (hmmm, really?
				   how?) */
	if (fd < 0
	    || write_opcode(fd, WH_MESSAGE_GLOBAL) < 0
	    || es_write_string(fd, mark) < 0
	    || write_opcode(fd, GL_MESSAGE_TEXT) < 0
	    || es_write_string(fd, message) < 0)
		ggzd_debug("ERROR: " "send_global_text_message: es error.");

}

void send_global_cardlist_message(player_t p, const char *mark, int *lengths,
				  card_t ** cardlist)
{
	int fd = ggzd_get_player_socket(p);
	int error = 0, i;
	seat_t s_rel;

	if (ggzd_get_seat_status(p) == GGZ_SEAT_BOT
	    || ggzd_get_seat_status(p) == GGZ_SEAT_OPEN)
		return;
	assert(mark && cardlist && lengths);
	ggzd_debug("Sending global cardlist message to player %d.", p);
	if (fd < 0 || write_opcode(fd, WH_MESSAGE_GLOBAL) < 0
	    || es_write_string(fd, mark) < 0
	    || write_opcode(fd, GL_MESSAGE_CARDLIST) < 0)
		error++;

	for (s_rel = 0; s_rel < game.num_seats; s_rel++) {
		seat_t s = UNCONVERT_SEAT(s_rel, p);
		if (es_write_int(fd, lengths[s]) < 0)
			error++;
		for (i = 0; i < lengths[s]; i++)
			if (write_card(fd, cardlist[s][i]) < 0)
				error++;
	}

	if (error)
		ggzd_debug("ERROR: "
			   "send_global_cardlist_message: es error.");
}

void broadcast_global_cardlist_message(const char *mark, int *lengths,
				       card_t ** cardlist)
{
	player_t p;
	for (p = 0; p < game.num_players; p++)
		send_global_cardlist_message(p, mark, lengths, cardlist);
}
