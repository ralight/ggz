/*
 * File: common.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards game module
 * Date: 06/20/2001
 * Desc: Game-independent game
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

#include "cards.h"
#include "game.h"
#include "games.h"
#include "ggz.h"
#include "protocols.h"

/* Global game variables */
struct wh_game_t game = {0};

char* game_states[] = {"WH_STATE_PRELAUNCH", "WH_STATE_NOTPLAYING", "WH_STATE_WAITFORPLAYERS",
			"WH_STATE_NEXT_HAND",
			"WH_STATE_FIRST_BID", "WH_STATE_NEXT_BID", "WH_STATE_WAIT_FOR_BID",
			"WH_STATE_FIRST_TRICK", "WH_STATE_NEXT_TRICK", "WH_STATE_NEXT_PLAY",
			"WH_STATE_WAIT_FOR_PLAY"};

void set_game_state(server_state_t state)
{
	/* sometimes an event can happen that changes the state while we're waiting for players,
	 * for instance a player finishing their bid even though someone's left the game.  In this
	 * case we wish to advance to the next game state while continuing to wait for players.
	 * However, this should be handled separately. */
	if (game.state == WH_STATE_WAITFORPLAYERS) {
		if (game.saved_state != state)
			ggz_debug("SERVER BUG: Setting game saved state to %d - %ws.", state, game_states[state]);
		game.saved_state = state;
	} else {
		if (game.state != state)
			ggz_debug("Setting game state to %d - %s.", state, game_states[state]);
		game.state = state;
	}
}

void save_game_state()
{
	if (game.state == WH_STATE_WAITFORPLAYERS) return;
	ggz_debug("Entering waiting state; old state was %d - %s.", game.state, game_states[game.state]);
	game.saved_state = game.state;
	game.state = WH_STATE_WAITFORPLAYERS;
}

void restore_game_state()
{
	ggz_debug("Ending waiting state; new state is %d - %s.", game.saved_state, game_states[game.saved_state]);
	game.state = game.saved_state;
}


/* NOTE ON "MESSAGES"
 *   - Each seat has a message.  The client should display this near to the player.
 *     It may contain information such as bid, etc.
 *   - There is a global message.  The client should display this prominently.
 *     It may contain information such as contract, trump, etc.
 *   - There are additional "tagged" global messages.  These are not implemented.
 */

/* send_player_message()
 *   sends seat s's message to player p
 *   fails silently...
 */
void send_player_message(seat_t s, player_t p)
{
	int fd = ggz_seats[p].fd;
	if (fd == -1) return;
	ggz_debug("Sending seat %d/%s's message to player %d/%s: %s", s, game.seats[s].ggz->name, p, ggz_seats[p].name, game.seats[s].message);
	es_write_int(fd, WH_MESSAGE_PLAYER);
	es_write_int(fd, CONVERT_SEAT(s, p));
	es_write_string(fd, game.seats[s].message);
}

/* send_player_message_toall
 *   sends seat s's message to all players
 *   fails silently...
 */
void send_player_message_toall(seat_t s)
{
	player_t p;
	ggz_debug("Sending seat %d/%s's message to all.", s, game.seats[s].ggz->name);
	for (p = 0; p < game.num_players; p++)
		send_player_message(s, p);
}

/* send_global_message
 *   sends the truly global message to player p
 *   fails silently...
 */
void send_global_message(char* mark, player_t p)
{
	/* the "mark" is a tag on the message that the client uses to identify it */
	int fd = ggz_seats[p].fd;
	char* message = get_global_message(mark);
	if (mark == NULL) {
		ggz_debug("SERVER BUG: send_global_message: NULL mark.");
		return;
	}
	if (fd == -1) return;
	if (message == NULL) message = ""; /* this happens sometimes */
	ggz_debug("Sending global message to player %d/%s: %s", p, ggz_seats[p].name, message);
	es_write_int(fd, WH_MESSAGE_GLOBAL);
	es_write_string(fd, mark);
	es_write_string(fd, message);
}

/* send_global_message_toall
 *   sends the truly global message to all players
 *   fails silently...
 */
void send_global_message_toall(char* mark)
{
	player_t p;
	for (p = 0; p < game.num_players; p++)
		send_global_message(mark, p);
}

void send_all_global_messages(player_t p)
{
	/* TODO: this isn't game-independent */
	send_global_message("", p);
	send_global_message("game", p);
	send_global_message("Options", p);
	send_global_message("Last Trick", p);
}

/* set_global_message
 */
void set_global_message(char* mark, char* message, ...)
{
	/* TODO: we just use the first character of the mark
	 * as an index into an array.  In the long term, we'll need some
	 * sort of hash instead.  Is there a standard hash library? */
	int hash = *mark;
	char buf[4096];
	int different = 1;

	va_list ap;
	va_start(ap, message);
	vsnprintf(buf, sizeof(buf), message, ap);
	va_end(ap);

	ggz_debug("Setting global message for '%s'.  Length is %d.", mark, strlen(buf));

	/* we re-duplicate it each time; this is a little slower but saves memory.
	 * we also check to see if it's identical; this saves bandwidth and user time overall */
	if (game.messages[hash]) {
		if (!strcmp(game.messages[hash], buf))
			different = 0;
		else
			free (game.messages[hash]);
	}
	if (different) {
		game.messages[hash] = strdup(buf);
		if (game.messages[hash] == NULL) {
			ggz_debug("ERROR: set_global_message: strdup returned NULL");
			/* it's NULL, so we should just be able to keep going... */
		}
		send_global_message_toall(mark);
	}
}

char* get_global_message(char* mark)
{
	int hash = *mark;
	return game.messages[hash];
}

/* game_handle_ggz
 *   Handle message from GGZ server
 *   return error status
 */
int handle_ggz(int ggz_fd, int* p_fd)
{
	int op, status = -1;
	player_t p;

	if(ggz_fd == -1 || es_read_int(ggz_fd, &op) < 0) {
		ggz_debug("ERROR: handle_ggz: couldn't read op.");
		return -1;
	}

	switch(op) {
		case REQ_GAME_LAUNCH:
			ggz_debug("Received a GGZ REQ_GAME_LAUNCH.");
			if(ggz_game_launch() == 0)
				status = update(WH_EVENT_LAUNCH, NULL);
			break;
		case REQ_GAME_JOIN:
			ggz_debug("Received a GGZ REQ_GAME_JOIN.");
			if(ggz_player_join(&p, p_fd) == 0) {
				status = update(WH_EVENT_JOIN, &p);
				status = 1;
			}
			break;
		case REQ_GAME_LEAVE:
			ggz_debug("Received a GGZ REQ_GAME_LEAVE.");
			if((status = ggz_player_leave(&p, p_fd)) == 0) {
				update(WH_EVENT_LEAVE, &p);
				status = 2;
			}
			break;
		case RSP_GAME_OVER:
			status = 3; /* Signal safe to exit */
			/* TODO: huh??? is this a real GGZ message??? what does it mean? */
			ggz_debug("Received a GGZ RSP_GAME_OVER.  Status is %i.", status);
			break;
		default:
			/* Unrecognized opcode */
			status = -1;
			ggz_debug("Handled an unknown GGZ message.  Status is %i.", status);
			break;
	}

	if (status < 0)
		ggz_debug("ERROR: handle_ggz: status is %d.", status);
	return status;
}

/* send_player_list
 *   Send out player list to player p
 */
int send_player_list(player_t p)
{
	int fd;
	seat_t s_rel, s;
	int status = 0;

	fd = ggz_seats[p].fd;
	if (fd == -1) return 0;

	s = game.players[p].seat;

	ggz_debug("Sending seat list to player %d/%s (%d seats)", p, ggz_seats[p].name, game.num_seats);
	
	if (es_write_int(fd, WH_MSG_PLAYERS) < 0)
		status = -1;
	if (es_write_int(fd, game.num_seats) < 0)
		status = -1;

	for(s_rel=0; s_rel<game.num_seats; s_rel++) {
		seat_t s_abs = (s_rel + s) % game.num_seats;
		struct ggz_seat_t * ggzseat = game.seats[s_abs].ggz;
		if (ggzseat == NULL) {
			/* We have a problem here, since seats may be sent out BEFORE the
			 * game is determined, in which case GGZ info for the seats wouldn't be
			 * known yet.  Here I fudge it by sending GGZ_SEAT_NONE instead. */
			ggz_debug("SERVER BUG: NULL ggz found for seat %d.", s_abs);
			if (es_write_int(fd, GGZ_SEAT_NONE) < 0)
				status = -1;
			if (es_write_string(fd, "(unknown)") < 0)
				status = -1;
			continue;			
		}
		if (ggzseat->assign == GGZ_SEAT_OPEN)
			/* TODO: this isn't the best place to do this... */
			snprintf(ggzseat->name, 17, "Empty Seat");
		if (es_write_int(fd, ggzseat->assign) < 0)
			status = -1;
		if (es_write_string(fd, ggzseat->name) < 0)
			status = -1;
	}

	if (status != 0)
		ggz_debug("ERROR: send_player_list: status is %d.", status);
	return status;
}

/* game_send_play
 *   Send out play for player to _all_ players
 *   Also symbolizes that this play is over
 */
int send_play(card_t card, seat_t seat)
{
	player_t p;
	int fd;
	int status = 0;

	ggz_debug("Sending seat %d/%s's play (%i %i %i) out to everyone.",
			  seat, game.seats[seat].ggz->name, card.face, card.suit, card.deck);

	for(p=0; p<game.num_players; p++) {
		fd = ggz_seats[p].fd;
		if(fd == -1) continue;
		if(es_write_int(fd, WH_MSG_PLAY) < 0
		   || es_write_int(fd, CONVERT_SEAT(seat, p)) < 0
		   || es_write_card(fd, card) < 0)
			status =  -1;
	}

	if (status != 0)
		ggz_debug("ERROR: send_play: status is %d.", status);
	return status;
}

/* game_send_gameover
 *   game-independent function to send a gameover to all players
 *   cnt is the number of winners, plist is the winner list
 */
int send_gameover(int cnt, player_t* plist)
{
	player_t p;
	int i, fd;
	int status = 0;

	ggz_debug("Sending out game-over message.");

	for (p = 0; p < game.num_players; p++) {
		fd = ggz_seats[p].fd;
		if (fd == -1) continue;

		if(es_write_int(fd, WH_MSG_GAMEOVER) < 0)
			status = -1;
		if (es_write_int(fd, cnt) < 0)
			status = -1;
		for (i = 0; i < cnt; i++) {
			seat_t ws = game.players[plist[i]].seat;
			if (es_write_int(fd, CONVERT_SEAT(ws, p)) < 0)
				status = -1;
		}
	}

	if (status != 0)
		ggz_debug("Error: send_game_over: status is %d.", status);
	return status;
}

/* send_table
 *   sends the list of cards on the table, in their relative orderings
 */
int send_table(player_t p)
{
	int s_r, s_abs, status = 0, fd = ggz_seats[p].fd;

	if (game.num_seats == 0)
		return 0;

	ggz_debug("Sending table to player %d/%s.", p, ggz_seats[p].name);
	
	if (fd == -1) {
		ggz_debug("ERROR: send_table: fd==-1.");
		return -1;
	}

	if (es_write_int(fd, WH_MSG_TABLE) < 0)
		status = -1;
	for (s_r = 0; s_r < game.num_seats; s_r++) {
		s_abs = (game.players[p].seat + s_r) % game.num_seats;
		if (es_write_card(fd, game.seats[s_abs].table) < 0)
			status = -1;
	}

	if (status != 0)
		ggz_debug("ERROR: send_table: status is %d.", status);
	return status;
}

/* send_sync()
 *   Send out current game hand, score, etc.
 *   Doesn't use its own protocol, but calls on others
 */
int send_sync(player_t p)
{	
	seat_t s;
	int status = 0;

	ggz_debug("Sending sync to player %d/%s.  State is %s.", p, ggz_seats[p].name, game_states[game.state]);

	if (send_player_list(p) < 0)
		status = -1;

	/* send out messages again */
	send_all_global_messages(p);
	for(s = 0; s < game.num_seats; s++)
		send_player_message(s, p);

	/* Send out hands */
	for (s=0; s < game.num_seats; s++)
		if (game_send_hand(p, s) < 0)
			status = -1;

	/* Send out table cards */
	if (send_table(p) < 0)
		status = -1;

	/* request bid/play again, if necessary */
	if (game.state == WH_STATE_WAIT_FOR_BID &&
	    game.next_bid == p)
		if (req_bid(game.next_bid, game.num_bid_choices, game.bid_text_ref) < 0)
			status = -1;
	if (game.state == WH_STATE_WAIT_FOR_PLAY &&
	    game.curr_play == p)
		if (req_play(game.curr_play, game.play_seat) < 0)
			status = -1;

	/* request newgame again, if necessary */
/* TODO: how does the client know it's a duplicate?
	if (game.state == WH_STATE_NOTPLAYING &&
	    !game.players[p].ready)
		req_newgame(p);
*/

	if (status != 0)
		ggz_debug("ERROR: send_sync: status is %d.", status);
	return status;
}

int send_sync_all()
{
	player_t p;
	int status = 0;

	for (p=0; p<game.num_players; p++)
		if (ggz_seats[p].assign == GGZ_SEAT_PLAYER)
			if (send_sync(p) < 0)
				status = -1;

	return 0;
}

/* req_play
 *  Game-independent function to request a player to
 *  make a play from a specific seat's hand
 */
int req_play(player_t p, seat_t s)
{
	int fd = ggz_seats[p].fd;
	seat_t s_r = CONVERT_SEAT(s, p);

	ggz_debug("Requesting player %d/%s to play from seat %d/%s's hand.", p, ggz_seats[p].name, s, game.seats[s].ggz->name);

	if (fd == -1 ||
	    es_write_int(fd, WH_REQ_PLAY) < 0 ||
            es_write_int(fd, s_r) < 0) {
		ggz_debug("req_play: couldn't send play request.");
		return -1;
	}

	/* although the game_* functions probably track this data
	 * themselves, we track it here as well just in case. */
	game.curr_play = p;
	game.play_seat = s;

	set_game_state(WH_STATE_WAIT_FOR_PLAY);

	game_set_player_message(p);

	return 0;
}

/* rec_play
 * receives a play from the client, and calls update if necessary.
 * NOTE that a return of -1 here indicates a GGZ error, which will disconnect the player
 */
int rec_play(player_t p, int *card_index)
{
	int fd = ggz_seats[p].fd, i, index = -1;
	card_t card;
	seat_t s = game.play_seat;
	char* err;

	/* read the card played */
	if (fd == -1 || es_read_card(fd, &card) < 0) {
		ggz_debug("ERROR: rec_play: couldn't read card.");
		return -1;
	}
	
	/* are we waiting for a play? */
	if (game.state != WH_STATE_WAIT_FOR_PLAY) {
		ggz_debug("SERVER/CLIENT BUG: we received a play (player %d/%s) when we weren't waiting for one.", p, ggz_seats[p].name);
		return -1;
	}

	/* Is is this player's turn to play? */
	if (game.curr_play != p) {
		/* better to just ignore it; a WH_MSG_BADPLAY requests a new play */
		ggz_debug("SERVER/CLIENT BUG: player %d/%s played out of turn!?!?", p, ggz_seats[p].name);
		return -1;	
	}

	/* find the card played */	
	for (i = 0; i < game.seats[s].hand.hand_size; i++)
		if (game.seats[s].hand.cards[i].face == card.face &&
		    game.seats[s].hand.cards[i].suit == card.suit &&
		    game.seats[s].hand.cards[i].deck == card.deck) {
			index = i;
			break;
		}

	if (index == -1) {
		send_badplay(p, "That card isn't even in your hand.  This must be a bug.");
		ggz_debug("CLIENT BUG: player %d/%s played a card that wasn't in their hand (%i %i %i)!?!?",
			  p, ggz_seats[p].name, card.face, card.suit, card.deck);
		return -1;
	}

	ggz_debug("We received a play of card (%d %d %d) from player %d/%s.", card.face, card.suit, card.deck, p, ggz_seats[p].name);

	/* we've verified that this card could have physically been played; we still
	 * need to check if it's a legal play */
	/* Note, however, that we don't return -1 on an error here.  An error returned indicates a GGZ
	 * error, which is not what happened.  This is just a player mistake */
	err = game_verify_play(index);
	if (err == NULL)
		update(WH_EVENT_PLAY, &index);
	else
		send_badplay(p, err);

	*card_index = index;
	return 0;
}

/* game_req_bid
 *   Request bid from current bidder
 *   parameters are: player to get bid from, number of possible bids, text entry for each bid
 */
int req_bid(player_t p, int num, char** bid_choices)
{
	int i, fd = ggz_seats[p].fd, status = 0;

	ggz_debug("Requesting a bid from player %d/%s; %d choices", p, ggz_seats[p].name, num);


	if (bid_choices == NULL) {
		bid_choices = game.bid_texts;
		for (i=0; i<num; i++)
			game_get_bid_text(bid_choices[i], game.max_bid_length, game.bid_choices[i]);
	}

	if(fd == -1 ||
	   es_write_int(fd, WH_REQ_BID) < 0 ||
	   es_write_int(fd, num) < 0)
		status = -1;
	for(i = 0; i < num; i++) {
		if (es_write_string(fd, bid_choices[i]) < 0)
			status = -1;
	}

	/* although the game_* functions probably track this data
	 * themselves, we track it here as well just in case. */
	game.num_bid_choices = num;
	game.next_bid = p;
	game.bid_text_ref = bid_choices;

	set_game_state( WH_STATE_WAIT_FOR_BID );

	game_set_player_message(p);

	if (status != 0)
		ggz_debug("ERROR: req_bid: status is %d.", status);
	return status;
}

/* rec_bid
 *   Receive a bid from an arbitrary player.  Test to make sure it's not out-of-turn.
 *   Note that a return of -1 here indicates a GGZ error, which will disconnect the player.
 */
int rec_bid(player_t p, int *bid_index)
{
	int fd = ggz_seats[p].fd;

	if(fd == -1 || es_read_int(fd, bid_index) < 0) {
		ggz_debug("ERROR: rec_bid: couldn't read bid index.");
		return -1;
	}
	*bid_index = *bid_index % game.num_bid_choices;
	if (*bid_index < 0) *bid_index += game.num_bid_choices;

	/* First of all, is this a valid bid? */
	if (p != game.next_bid) {
		ggz_debug("It's player %d/%s's turn to bid, not player %d/%s's.", game.next_bid, ggz_seats[game.next_bid].name, p, ggz_seats[p].name);
		return -1;
	} else if (! (game.state == WH_STATE_WAIT_FOR_BID ||
		     (game.state == WH_STATE_WAITFORPLAYERS && game.saved_state == WH_STATE_WAIT_FOR_BID) )) {
		ggz_debug("We're not currently waiting for a bid!");
		return -1;
	}

	ggz_debug("Received bid choice %d (%s) from player %d/%s",
		  *bid_index, game.bid_texts[*bid_index], p, ggz_seats[p].name);
	return 0;
}

void send_badplay(player_t p, char* msg)
{
	int fd = ggz_seats[p].fd;
	if (fd == -1) return;
	ggz_debug("Sending a bad play to player %d/%s - %s.",
		p, ggz_seats[p].name, msg);
	es_write_int(fd, WH_MSG_BADPLAY);
	es_write_string(fd, msg);
	set_game_state( WH_STATE_WAIT_FOR_PLAY );
}

/* send_hand
 *   Show a player a hand.  This will reveal the cards in
 *   the hand iff reveal is true.
 */
int send_hand(const player_t p, const seat_t s, const int reveal)
{
	int fd = ggz_seats[p].fd;
	int i, status = 0;
	card_t card;

	if(fd == -1) return 0; /* Don't send to a bot */

	if (game.seats[s].hand.hand_size == 0) return 0;

	ggz_debug("Sending player %d/%s hand %d/%s - %srevealing",
		  p, ggz_seats[p].name, s, game.seats[s].ggz->name, reveal ? "" : "not ");

	if (es_write_int(fd, WH_MSG_HAND) < 0
	    || es_write_int(fd, CONVERT_SEAT(s, p)) < 0
	    || es_write_int(fd, game.seats[s].hand.hand_size) < 0)
		status = -1;

	for(i=0; i<game.seats[s].hand.hand_size; i++) {
		if (reveal) card = game.seats[s].hand.cards[i];
		else card = UNKNOWN_CARD;
		if (es_write_card(fd, card) < 0)
			status = -1;
	}

	if (status != 0)
		ggz_debug("ERROR: send_hand: status is %d.", status);
	return status;
}

void send_trick(player_t winner)
{
	int fd;
	player_t p;
	seat_t s;

	ggz_debug("Sending out trick (%d/%s won) and cleaning it up.", winner, ggz_seats[winner].name);

	for (s=0; s<game.num_seats; s++)
		/* note: we also clear the table at the beginning of every hand */
		game.seats[s].table = UNKNOWN_CARD;

	for (p=0; p<game.num_players; p++) {
		fd = ggz_seats[p].fd;
		if (fd == -1) continue;

		es_write_int(fd, WH_MSG_TRICK);
		es_write_int(fd, CONVERT_SEAT(game.players[winner].seat, p));
	}
}

int req_newgame(player_t p)
{
	int fd, status;
	fd = ggz_seats[ p ].fd;
	if (fd == -1) {
		ggz_debug("ERROR: req_newgame: fd is -1.");
		return -1;	/* TODO: handle AI */
	}

	ggz_debug("Sending out a WH_REQ_NEWGAME to player %d/%s.", p, ggz_seats[p].name);
	status = es_write_int(fd, WH_REQ_NEWGAME);

	if (status != 0)
		ggz_debug("ERROR: req_newgame: status is %d.", status);
	return status;
}

int send_newgame()
{
	int fd;
	player_t p;

	ggz_debug("Sending out a newgame message.");

	for (p=0; p < game.num_players; p++) {
		fd = ggz_seats[p].fd;
		if (fd == -1 || es_write_int(fd, WH_MSG_NEWGAME) < 0) {
			ggz_debug("ERROR: send_newgame: couldn't send newgame.");
			return -1;
		}
	}

	
	return 0;
}

int rec_options(int num_options, int* options)
{
	int fd = ggz_seats[game.host].fd, status = 0, i;
	if (fd == -1) {
		ggz_debug("SERVER bug: unknown host in rec_options.");
		exit(-1);
	}
	
	for (i = 0; i < num_options; i++)
		if (es_read_int(fd, &options[i]) < 0)
			status = options[i] = -1;

	if (status != 0)
		ggz_debug("ERROR: rec_options: status is %d.", status);
	return status;	
}

static char* player_messages[] = {"WH_RSP_NEWGAME", "WH_RSP_OPTIONS",
	"WH_RSP_PLAY", "WH_RSP_BID", "WH_REQ_SYNC"};

/* Handle message from player */
int handle_player(player_t p)
{
	int fd, op, status = 0;
	int index;

	fd = ggz_seats[p].fd;
	
	if(fd == -1 || es_read_int(fd, &op) < 0) {
		ggz_debug("ERROR: handle_player: can't read op from player %d/%s, fd %d.", p, ggz_seats[p].name, fd);
		return -1;
	}

	if (op >= 0 && op <= WH_REQ_SYNC)
		ggz_debug("Received %d (%s) from player %d/%s.", op, player_messages[op], p, ggz_seats[p].name);
	else
		ggz_debug("Received unknown message %d from player %d/%s.", op, p, ggz_seats[p].name);

	switch (op) {
		case WH_RSP_NEWGAME:
			if (game.state == WH_STATE_NOTPLAYING) {
				status = 0;
				update(WH_EVENT_NEWGAME, &p);
			} else {
				ggz_debug("SERVER/CLIENT bug?: received WH_RSP_NEWGAME while we were in state %d (%s).", game.state, game_states[game.state]);
				status = -1;
			}
			break;
		case WH_RSP_OPTIONS:
			if (p != game.host) {
				/* how could this happen? */
				ggz_debug("SERVER/CLIENT bug: received options from non-host player.");
				status = -1;
			} else {
				if (game.which_game == GGZ_GAME_UNKNOWN) {
					int option;
					rec_options(1, &option);
					game.which_game = game_types[option];

					if (game.which_game < 0 || game.which_game >= GGZ_NUM_GAMES) {
						ggz_debug("SERVER/CLIENT error: bad game type %d selected; using %d instead.", game.which_game, game_types[0]);
						game.which_game = game_types[0];
					}

					game_init_game();
					send_sync_all();

					if (!ggz_seats_open())
						next_play();
				} else {
					int options[game.num_options];
					rec_options(game.num_options, options);
					ggz_debug("Entering game_handle_options.");
					game_handle_options(options);
					if (game.options_initted)
						next_play();
					else
						/* eventually we may be able to do multiple rounds of options. */
						ggz_debug("SERVER BUG: handle_player: options not initted after game_handle_options called.");
				}
			}
			break;
		case WH_RSP_BID:
			if((status = rec_bid(p, &index)) == 0)
				update(WH_EVENT_BID, &index);
			break;
		case WH_RSP_PLAY:
			status = rec_play(p, &index);
			break;
		case WH_REQ_SYNC:
			status = send_sync(p);
			break;
		default:
			/* Unrecognized opcode */
			ggz_debug("SERVER/CLIENT BUG: game_handle_player: unrecognized opcode %d.", op);
			status = -1;
			break;
	}

	if (status != 0)
		ggz_debug("ERROR: handle_player: status is %d on message from player %d/%s.", status, p, ggz_seats[p].name);
	return status;
}

/* init_game
 *   Setup game state and board
 *   also initializes the _type_ of game
 */
void init_game(int which)
{
	memset(&game, 0, sizeof(struct wh_game_t));

	/* JDS: Note: the game type must have been initialized by here */
	game.which_game = which;

	game.state = WH_STATE_PRELAUNCH;

	ggz_debug("Game initialized as game %d.", game.which_game);
}

/* start a new game! */
int newgame()
{
	player_t p;

	if (game.which_game == GGZ_GAME_UNKNOWN) {
		ggz_debug("SERVER BUG: starting newgame() on unknown game.");
		exit(-1);
	}

	/* should be entered only when we're ready for a new game */
	for (p=0; p<game.num_players; p++)
		game.players[p].ready = 0;
	if (!game.initted) game_init_game();
	game_start_game();
	for (p=0; p<game.num_players; p++)
		game_set_player_message(p);
	send_newgame();
	game.dealer = random() % game.num_players;
	set_game_state( WH_STATE_NEXT_HAND );

	ggz_debug("Game start completed successfully.");

	next_play();
}

/* Do the next play */
void next_play(void)
{
	player_t p;
	seat_t s;
	/* TODO: split this up into functions */
	/* TODO: use looping instead of recursion */

	ggz_debug("Next play called while state is %s.", game_states[game.state]);

	switch (game.state) {
		case WH_STATE_NOTPLAYING:
			ggz_debug("Next play: trying to start a game.");
			for (p=0; p<game.num_players; p++)
				game.players[p].ready = 0;
			for (p=0; p<game.num_players; p++)
				req_newgame(p);
			break;
		case WH_STATE_NEXT_HAND:
			ggz_debug("Next play: dealing a new hand.");
			if (game_test_for_gameover()) {
				game_handle_gameover();
				cards_destroy_deck();
				if (game.specific != NULL) free(game.specific);
				set_game_state( WH_STATE_NOTPLAYING );
				return;
			}

			/* shuffle and deal a hand */
			cards_shuffle_deck();
			for(p=0; p<game.num_players; p++)
				game.players[p].tricks = 0;

			ggz_debug("Dealing hand %d.", game.hand_num);
			if(game_deal_hand() < 0)
				return;
			ggz_debug("Dealt hand successfully.");

			/* Now send the resulting hands to each player */
			for(p=0; p<game.num_players; p++)
				for (s=0; s<game.num_seats; s++)
					if (game_send_hand(p, s) < 0)
						ggz_debug("Error: game_send_hand returned -1.");

			set_game_state( WH_STATE_FIRST_BID );
			ggz_debug("Done generating the next hand; entering bidding phase.");
			next_play(); /* recursion */
			break;
		case WH_STATE_FIRST_BID:
			ggz_debug("Next play: starting the bidding.");
			set_game_state( WH_STATE_NEXT_BID );

			for (p = 0; p < game.num_players; p++) {
				game.players[p].bid.bid = 0;
				game_set_player_message(p); /* TODO: are all these player messages really necessary? */
			}
			game.bid_count = 0;

			game_start_bidding();
			next_play(); /* recursion */
			break;
		case WH_STATE_NEXT_BID:
			ggz_debug("Next play: bid %d/%d - player %d/%s.", game.bid_count, game.bid_total, game.next_bid, ggz_seats[game.next_bid].name);
			game_get_bid();
			break;
		case WH_STATE_NEXT_PLAY:
			ggz_debug("Next play: playing %d/%d - player %d/%s.", game.play_count, game.play_total, game.next_play, ggz_seats[game.next_play].name);
			game_get_play(game.next_play);
			break;
		case WH_STATE_FIRST_TRICK:
			ggz_debug("Next play: first trick of the hand.");
			set_game_state( WH_STATE_NEXT_TRICK );

			game.trick_total = game.hand_size;
			game.play_total = game.num_players;
			/* we also clear all cards after every trick */
			for (s=0; s<game.num_seats; s++)
				game.seats[s].table = UNKNOWN_CARD;

			game_start_playing();
			set_all_player_messages();
			game.trump_broken = 0;
			game.next_play = game.leader;
			game.play_count = game.trick_count = 0;
			next_play();
			break;
		case WH_STATE_NEXT_TRICK:
			ggz_debug("Next play: next trick %d/%d - leader is %d/%s.", game.trick_count, game.trick_total, game.leader, ggz_seats[game.leader].name);
			game.play_count = 0;
			game.next_play = game.leader;
			set_game_state( WH_STATE_NEXT_PLAY );
			next_play(); /* minor recursion */
			break;
		default:
			ggz_debug("SERVER BUG: game_play called with unknown state: %d", game.state);
			break;
	}

	return;
}

/* Update game state */
int update(server_event_t event, void *data)
{
	bid_t bid;
	int bid_index;
	player_t p, player;
	int ready = 0;
	
	switch (event) {
		case WH_EVENT_LAUNCH:
			ggz_debug("Handling a WH_EVENT_LAUNCH.");
			if(game.state != WH_STATE_PRELAUNCH) {
				ggz_debug("ERROR: update: state wasn't prelaunch when handling a launch.");
				return -1;
			}
			/* determine number of players. */
			game.num_players = ggz_seats_num(); /* ggz seats == players */
			game.players = (struct game_player_t *)alloc(game.num_players * sizeof(struct game_player_t));
			/* we don't yet know the number of seats */

			/* as soon as we know which game we're playing, we should init the game */
			if (game.which_game != GGZ_GAME_UNKNOWN)
				game_init_game();

			set_game_state(WH_STATE_NOTPLAYING);
			save_game_state(); /* no players are connected yet, so we enter waiting phase */
			break;
		case WH_EVENT_JOIN:
			ggz_debug("Handling a WH_EVENT_JOIN.");
			if(game.state != WH_STATE_WAITFORPLAYERS) {
				ggz_debug("SERVER BUG: someone joined while we weren't waiting.");
				return -1;
			}

			player = *(int*)data;

			/* if all seats are occupied, we restore the former state and
			 * continue playing (below).  The state is restored up here
			 * so that the sync will be handled correctly. */
			if (!ggz_seats_open())
				restore_game_state();

			/* send all table info to joiner */
			send_sync(player);

			/* send player list to everyone else */
			for(p = 0; p < game.num_players; p++)
				if (p != player)
					send_player_list(p);

			if (game.state != WH_STATE_NOTPLAYING &&
			    !(game.state == WH_STATE_WAITFORPLAYERS && game.saved_state == WH_STATE_NOTPLAYING))
				send_player_message_toall(player); /* should this be in sync??? */

			if (player == game.host && game.which_game == GGZ_GAME_UNKNOWN)
				games_req_gametype();

			if (!ggz_seats_open() && game.which_game != GGZ_GAME_UNKNOWN) {
				/* (Re)Start game play */
				if (game.state != WH_STATE_WAIT_FOR_BID && game.state != WH_STATE_WAIT_FOR_PLAY)
					/* if we're in one of these two states, we have to wait for a response anyway */
					next_play();
			}
			break;
		case WH_EVENT_NEWGAME:
                        player = *(player_t *)data;
			ggz_debug("Handling a WH_EVENT_NEWGAME for player %d/%s.", player, ggz_seats[player].name);
			game.players[player].ready = 1;
			ready = 1;
			ggz_debug("Determining options.");
			if (player == game.host && !game.options_initted &&
			    game_get_options() < 0) {
				ggz_debug("Error in game_get_options.  Using defaults.");
				game.options_initted = 1;
			}
			for (p=0; p<game.num_players; p++)
				if (!game.players[p].ready) {
					/* TODO: should another WH_REQ_NEWGAME be sent, just as a reminder?
					 * if we do, then the client may not be able to determine that it's a duplicate... */
					ggz_debug("Player %d/%s is not ready.", p, ggz_seats[p].name);
					ready = 0;
				}
			if (ready && game.options_initted) newgame();
			break;
		case WH_EVENT_LEAVE:
			ggz_debug("Handling a WH_EVENT_LEAVE.");
			for(p = 0; p < game.num_players; p++)
				send_player_list(p);

			/* save old state and enter waiting phase */
			save_game_state();
			break;
		case WH_EVENT_PLAY:
			{
			int card_index, i;
			card_t c;
			hand_t* hand;

			ggz_debug("Handling a WH_EVENT_PLAY.");
			/* determine the play */
			card_index = *(int *)data;
			hand = &game.seats[game.play_seat].hand;
			c = hand->cards[card_index];

			/* send the play */
			send_play(c, game.play_seat);

			/* remove the card from the player's hand */
			/* TODO: this is quite inefficient */
			hand->cards[card_index] = UNKNOWN_CARD;
			for (i=card_index; i<hand->hand_size; i++)
				hand->cards[i] = hand->cards[i+1];
			hand->hand_size--;

			/* Move the card onto the table */
			game.seats[ game.play_seat ].table = c;
			if(game.next_play == game.leader)
				game.lead_card = c;

			/* do extra handling */
			if (c.suit == game.trump) game.trump_broken = 1;
			game_handle_play(c);

			/* set up next move */
			game.play_count++;
			game_next_play();
			if (game.play_count != game.play_total)
				set_game_state(WH_STATE_NEXT_PLAY);
			else {
				/* end of trick */
				ggz_debug("End of trick; %d/%d.  Scoring it.", game.trick_count, game.trick_total);
				sleep(1);
				game_end_trick();
				send_trick(game.winner);
				game.trick_count++;
				set_game_state( WH_STATE_NEXT_TRICK );
				if (game.trick_count == game.trick_total) {
					/* end of the hand */
					ggz_debug("End of hand number %d.", game.hand_num);
					sleep(1);
					game_end_hand();
					set_all_player_messages();
					game.dealer = (game.dealer + 1) % game.num_players;
					game.hand_num++;
					set_game_state( WH_STATE_NEXT_HAND );
				}
			}

			/* this is the player that just finished playing */
			game_set_player_message(game.curr_play);

			/* do next move */
			next_play();
			}
			break;
		case WH_EVENT_BID:
			{
			int was_waiting = 0;
			ggz_debug("Handling a WH_EVENT_BID.");
			if (game.state == WH_STATE_WAITFORPLAYERS) {
				/* if a player left while another player was in the middle of bidding, this
				 * can happen.  The solution is to temporarily return to playing, handle the
				 * bid, and then (below) return to waiting. */
				restore_game_state();
				was_waiting = 1;
			}

			/* determine the bid */
			bid_index = *(int*)data;
			p = game.next_bid;
			bid = game.bid_choices[bid_index];
			game.players[p].bid = bid;

			/* handle the bid */
			ggz_debug("Entering game_handle_bid(%d).", bid_index);
			game_handle_bid(bid_index);

			/* set up next move */
			ggz_debug("Setting up next bid.");
			game.bid_count++;
			game_next_bid();
			if (game.bid_count == game.bid_total)
				set_game_state(WH_STATE_FIRST_TRICK);
			else if (game.state == WH_STATE_WAIT_FOR_BID)
				set_game_state(WH_STATE_NEXT_BID);
			else
				ggz_debug("SERVER BUG: handled WH_EVENT_BID while not in WH_STATE_WAIT_FOR_BID.");

			/* this is the player that just finished bidding */
			game_set_player_message(p);

			if (was_waiting)
				save_game_state();
			else
				/* do next move */
				next_play();
			}
			break;
	}
	
	return 0;
}

void set_all_player_messages()
{
	player_t p;
	for (p=0; p<game.num_players; p++)
		game_set_player_message(p);	
}

void set_num_seats(int num_seats)
{
	ggz_debug("Setting number of seats to %d.", num_seats);
	game.num_seats = num_seats;
	if (game.seats != NULL)
		free(game.seats);
	game.seats = (struct game_seat_t *)alloc(game.num_seats * sizeof(struct game_seat_t));
}



