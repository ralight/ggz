/*
 * File: game.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards game module
 * Date: 06/29/2000
 * Desc: Game functions
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
#include "protocols.h"

/* Global game variables */
struct wh_game_t game = {0};

/* these aren't *quite* worthy of being in the game struct */
int game_type_cnt;
int game_types[GGZ_NUM_GAMES];	/* possible types of games; used for option requests */

/* suaro-specific */
/* TODO: these should be low, clubs, diamonds, ..., high, but that won't fit in the client window */
char* suaro_suit_names[6] = {"lo", "C", "D", "H", "S", "hi"};
char* bridge_suit_names[5] = {"C", "D", "H", "S", "NT"};

/* helper functions */
static char** alloc_string_array(int, int);
/* static void free_string_array(char**, int); */

/* Game-independent private functions */
static void set_game_state(int state);
static void save_game_state();
static void restore_game_state();
static void send_player_message(seat_t, player_t);
static void send_player_message_toall(seat_t);
static void set_all_player_messages();

static void send_global_message(char*, player_t);
static void send_global_message_toall(char*);
static void send_all_global_messages(player_t p);
static void set_global_message(char*, char*, ...);
static char* get_global_message(char*);

static int send_player_list(player_t);
static int send_play(card_t card, seat_t);
static int send_sync(player_t);
static int send_sync_all();
static int send_gameover(int, player_t*);
static int req_bid(player_t, int, char**);
static int req_play(player_t, seat_t);
static int rec_play(player_t, int *);
static void send_badplay(player_t, char*);
static int send_hand(player_t, seat_t, int);
static int req_newgame(player_t);
static int send_newgame();

static int rec_options(int, int*);

static void next_play(void);			/* make the next move */
static int update(int event, void *data);	/* receive a player move */

/* Game-specific private functions */

static void game_set_player_message(player_t); /* determine and send the player message */

static int game_get_bid_text(char*, int, bid_t);	/* determines the textual string for the bid */
static void game_start_bidding();	/* updates data for the first bid */
static int game_get_bid();		/* gets a bid from next player */
static int game_handle_bid(int);	/* handles a bid from current bidder */
static void game_next_bid();		/* updates data for the next bid */
static void game_start_playing();	/* updates data after the last bid/before the playing starts */

static char* game_verify_play(int card_index);	/* verifies the play is legal */
static void game_next_play();			/* sets up for next play */
static void game_get_play(player_t);		/* retreives a play */
static void game_handle_play(card_t);		/* handle a play */

static int game_deal_hand(void);		/* deal next hand */
static int game_send_hand(int, int);		/* send a hand to a player */
static void game_end_trick(void);		/* end-of-trick calculations */
static void game_end_hand(void);		/* end-of-hand calculations */

static void game_init_game();			/* initialize the game data */
static int game_req_gametype();                 /* request the type of game (as an option) */
static int game_get_options();			/* determine/request options */
static void game_handle_options();		/* handle options from player */
static void game_start_game();			/* start a game */
static int game_test_for_gameover();		/* returns TRUE iff gameover */
static int game_handle_gameover();		/* handle a gameover */

static int game_valid_game(int);		/* is the game valid? */


/* JDS: these are just helper functions which should be moved to another
 * file */

void* alloc(int size)
{
	void* ret = malloc(size);
	if (ret == NULL) {
		/* TODO... */
		ggz_debug("SERVER error: failed malloc.");
		exit (-1);
	}
	memset(ret, 0, size); /* zero it all */
	return ret;
}	

/* alloc_char_array, free_char_array
 *   this allocates/frees an array of strings
 *   it really shouldn't go here...
 */
static char** alloc_string_array(int num, int len)
{
	int i;
	char** bids;
	char* bids2;
	i = num * sizeof(char*);
	bids = (char**)alloc(i);

	i = len * num * sizeof(char);
	bids2 = (char*)alloc(i);

	for(i = 0; i < num; i++) {
		bids[i] = bids2;
		bids2 += len;
	}

	return bids;
}
/*
static void free_string_array(char** bids)
{
	int i;
	free(bids[0]);
	free(bids);
}
*/



/* JDS: these top functions are common to all games.  They should be
 * moved to game-common.c, or something like that. */

/* NOTE ON "MESSAGES"
 *   - Each seat has a message.  The client should display this near to the player.
 *     It may contain information such as bid, etc.
 *   - There is a global message.  The client should display this prominently.
 *     It may contain information such as contract, trump, etc.
 *   - There are additional "tagged" global messages.  These are not implemented.
 */

char* game_states[] = {"WH_STATE_NOTPLAYING", "WH_STATE_WAITFORPLAYERS", "WH_STATE_NEXT_HAND",
			"WH_STATE_FIRST_BID", "WH_STATE_NEXT_BID", "WH_STATE_WAIT_FOR_BID",
			"WH_STATE_FIRST_TRICK", "WH_STATE_NEXT_TRICK", "WH_STATE_NEXT_PLAY",
			"WH_STATE_WAIT_FOR_PLAY"};

static void set_game_state(int state)
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

static void save_game_state()
{
	if (game.state == WH_STATE_WAITFORPLAYERS) return;
	ggz_debug("Entering waiting state; old state was %d - %s.", game.state, game_states[game.state]);
	game.saved_state = game.state;
	game.state = WH_STATE_WAITFORPLAYERS;
}

static void restore_game_state()
{
	ggz_debug("Ending waiting state; new state is %d - %s.", game.saved_state, game_states[game.saved_state]);
	game.state = game.saved_state;
}

/* send_player_message()
 *   sends seat s's message to player p
 *   fails silently...
 */
static void send_player_message(seat_t s, player_t p)
{
	int fd = ggz_seats[p].fd;
	if (fd == -1) return;
	ggz_debug("Sending seat %d's message to player %d: %s", s, p, game.seats[s].message);
	es_write_int(fd, WH_MESSAGE_PLAYER);
	es_write_int(fd, CONVERT_SEAT(s, p));
	es_write_string(fd, game.seats[s].message);
}

/* send_player_message_toall
 *   sends seat s's message to all players
 *   fails silently...
 */
static void send_player_message_toall(seat_t s)
{
	player_t p;
	for (p = 0; p < game.num_players; p++)
		send_player_message(s, p);
}

/* send_global_message
 *   sends the truly global message to player p
 *   fails silently...
 */
static void send_global_message(char* mark, player_t p)
{
	/* the "mark" is a tag on the message that the client uses to identify it */
	int fd = ggz_seats[p].fd;
	char* message = get_global_message(mark);
	if (fd == -1) return;
	if (message == NULL) return;
	ggz_debug("Sending global message to player %d: %s", p, message);
	es_write_int(fd, WH_MESSAGE_GLOBAL);
	es_write_string(fd, mark);
	es_write_string(fd, message);
}

/* send_global_message_toall
 *   sends the truly global message to all players
 *   fails silently...
 */
static void send_global_message_toall(char* mark)
{
	player_t p;
	for (p = 0; p < game.num_players; p++)
		send_global_message(mark, p);
}

static void send_all_global_messages(player_t p)
{
	/* TODO: this isn't game-independent */
	send_global_message("", p);
	send_global_message("game", p);
}

/* set_global_message
 */
static void set_global_message(char* mark, char* message, ...)
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
			/* TODO */
			ggz_debug("strdup returned NULL");
			/* it's NULL, so we should just be able to keep going... */
		}
		send_global_message_toall(mark);
	}
}

static char* get_global_message(char* mark)
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

	if(es_read_int(ggz_fd, &op) < 0)
		return -1;

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
	return status;
}

/* send_player_list
 *   Send out player list to player p
 */
static int send_player_list(player_t p)
{
	int fd;
	seat_t s_rel, s;
	int status = 0;

	fd = ggz_seats[p].fd;
	if (fd == -1) return -1;

	s = game.players[p].seat;

	ggz_debug("Sending seat list to player %d (%d seats)", p, game.num_seats);
	
	if (es_write_int(fd, WH_MSG_PLAYERS) < 0)
		return -1;
	if (es_write_int(fd, game.num_seats) < 0)
		return -1;

	for(s_rel=0; s_rel<game.num_seats; s_rel++) {
		seat_t s_abs = (s_rel + s) % game.num_seats;
		struct ggz_seat_t * ggzseat = game.seats[s_abs].ggz;
		if (ggzseat == NULL) {
			/* TODO: we have a problem here, since seats may be sent out BEFORE the
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

	return status;
}

/* game_send_play
 *   Send out play for player to _all_ players
 *   Also symbolizes that this play is over
 */
static int send_play(card_t card, seat_t seat)
{
	player_t p;
	int fd;
	int status = 0;

	ggz_debug("Sending seat %d's play (%i %i %i) out to everyone.",
			  seat, card.face, card.suit, card.deck);

	for(p=0; p<game.num_players; p++) {
		fd = ggz_seats[p].fd;
		if(fd == -1) continue;
		if(es_write_int(fd, WH_MSG_PLAY) < 0
		   || es_write_int(fd, CONVERT_SEAT(seat, p)) < 0
		   || es_write_card(fd, card) < 0)
			status =  -1;
	}

	return status;
}

/* game_send_gameover
 *   game-independent function to send a gameover to all players
 *   cnt is the number of winners, plist is the winner list
 */
static int send_gameover(int cnt, player_t* plist)
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
	return status;
}

/* send_sync()
 *   Send out current game hand, score, etc.
 *   Doesn't use its own protocol, but calls on others
 */
static int send_sync(player_t p)
{	
	seat_t s;
	int status = 0;

	ggz_debug("Sending sync to player %d (%s)", p, ggz_seats[p].name);

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

	/* request bid/play again, if necessary */
	if (game.state == WH_STATE_WAIT_FOR_BID &&
	    game.next_bid == p)
		if (req_bid(game.next_bid, game.num_bid_choices, game.bid_text_ref) < 0)
			status = -1;
	if (game.state == WH_STATE_WAIT_FOR_PLAY &&
	    game.next_play == p)
		if (req_play(game.next_play, game.play_seat) < 0)
			status = -1;

	/* request newgame again, if necessary */
/* TODO: how does the client know it's a duplicate?
	if (game.state == WH_STATE_NOTPLAYING &&
	    !game.players[p].ready)
		req_newgame(p);
*/

ggz_debug("done sending sync");

	return status;
}

static int send_sync_all()
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
static int req_play(player_t p, seat_t s)
{
	int fd = ggz_seats[p].fd;
	seat_t s_r = CONVERT_SEAT(s, p);

	ggz_debug("Requesting player %i to play from seat %i (relative %i)'s hand.", p, s, s_r);

	if (es_write_int(fd, WH_REQ_PLAY) < 0)
		return -1;
	if (es_write_int(fd, s_r) < 0)
		return -1;

	/* although the game_* functions probably track this data
	 * themselves, we track it here as well just in case. */
	game.curr_play = p;
	game.play_seat = s;

	set_game_state(WH_STATE_WAIT_FOR_PLAY);

	return 0;
}

/* rec_play
 * receives a play from the client, and calls update if necessary.
 * NOTE that a return of -1 here indicates a GGZ error, which will disconnect the player
 */
static int rec_play(player_t p, int *card_index)
{
	int fd;
	card_t card;
	int i, index = -1;
	seat_t s = game.play_seat;
	char* err;

	fd = ggz_seats[p].fd;
	if (fd == -1) return -1;

	/* read the card played */
	if (es_read_card(fd, &card) < 0)
		return -1;
	
	/* are we waiting for a play? */
	if (game.state != WH_STATE_WAIT_FOR_PLAY) {
		ggz_debug("SERVER/CLIENT BUG: we received a play (player %d) when we weren't waiting for one.", p);
		return -1;
	}

	/* Is is this player's turn to play? */
	if (game.curr_play != p) {
		/* better to just ignore it; a WH_MSG_BADPLAY requests a new play */
		ggz_debug("SERVER/CLIENT BUG: player %d played out of turn!?!?", p);
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
		/* TODO: bug: somehow after this happens, we stop waiting for a play??? */
		send_badplay(p, "That card isn't even in your hand.  This must be a bug.");
		ggz_debug("CLIENT BUG: player %d played a card that wasn't in their hand (%i %i %i)!?!?",
			  p, card.face, card.suit, card.deck);
		return -1;
	}

	ggz_debug("We received a play of card (%d %d %d) from player %d.", card.face, card.suit, card.deck, p);

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
static int req_bid(player_t p, int num, char** bid_choices)
{
	int i;
	int fd;

	fd = ggz_seats[p].fd;
        if (fd == -1) return -1;

	ggz_debug("Requesting a bid from player %d (%s); %d choices", p, ggz_seats[p].name, num);

	if(es_write_int(fd, WH_REQ_BID) < 0)
		return -1;
	if (es_write_int(fd, num) < 0)
		return -1;
	for(i = 0; i < num; i++) {
		if (es_write_string(fd, bid_choices[i]) < 0)
			return -1;
	}

	/* although the game_* functions probably track this data
	 * themselves, we track it here as well just in case. */
	game.num_bid_choices = num;
	game.next_bid = p;
	game.bid_text_ref = bid_choices;

	set_game_state( WH_STATE_WAIT_FOR_BID );

	return 0;
}

/* rec_bid
 *   Receive a bid from an arbitrary player.  Test to make sure it's not out-of-turn.
 *   Note that a return of -1 here indicates a GGZ error, which will disconnect the player.
 */
static int rec_bid(player_t p, int *bid_index)
{
	int fd = ggz_seats[p].fd;
	if (fd == -1) return -1;

	if(es_read_int(fd, bid_index) < 0)
		return -1;
	*bid_index = *bid_index % game.num_bid_choices;
	if (*bid_index < 0) *bid_index += game.num_bid_choices;

	/* First of all, is this a valid bid? */
	if (p != game.next_bid) {
		ggz_debug("It's player %d's turn to bid, not player %d's.", game.next_bid, p);
		return -1;
	} else if (! (game.state == WH_STATE_WAIT_FOR_BID ||
		     (game.state == WH_STATE_WAITFORPLAYERS && game.saved_state == WH_STATE_WAIT_FOR_BID) )) {
		ggz_debug("We're not currently waiting for a bid!");
		return -1;
	}

	ggz_debug("Received bid choice %d (%s) from player %d (%s)",
		  *bid_index, game.bid_texts[*bid_index], p, ggz_seats[p].name);
	return 0;
}

static void send_badplay(player_t p, char* msg)
{
	int fd = ggz_seats[p].fd;
	if (fd == -1) return;
	ggz_debug("Sending a bad play to player %d (%s) - %s.",
		p, ggz_seats[p].name, msg);
	es_write_int(fd, WH_MSG_BADPLAY);
	es_write_string(fd, msg);
	set_game_state( WH_STATE_WAIT_FOR_PLAY );
}

/* send_hand
 *   Show a player a hand.  This will reveal the cards in
 *   the hand iff reveal is true.
 */
static int send_hand(const player_t p, const seat_t s, const int reveal)
{
	int fd = ggz_seats[p].fd;
	int i;
	card_t card;

	if(fd == -1) return 0; /* Don't send to a bot */

	if (game.seats[s].hand.hand_size == 0) return 0;

	ggz_debug("Sending player %d (%s) hand %d (%s) - %srevealing",
		  p, ggz_seats[p].name, s, game.seats[s].ggz->name, reveal ? "" : "not ");

	if (es_write_int(fd, WH_MSG_HAND) < 0
	    || es_write_int(fd, CONVERT_SEAT(s, p)) < 0
	    || es_write_int(fd, game.seats[s].hand.hand_size) < 0)
		return -1;

	for(i=0; i<game.seats[s].hand.hand_size; i++) {
		if (reveal) card = game.seats[s].hand.cards[i];
		else card = UNKNOWN_CARD; /* TODO: la pocha specific */
		if (es_write_card(fd, card) < 0)
			return -1;
	}

	return 0;
}

static void send_trick(player_t winner)
{
	int fd;
	player_t p;

	ggz_debug("Sending out trick (%d - %s won) and cleaning it up.", winner, ggz_seats[winner].name);

	for(p=0; p<game.num_players; p++) {
		/* note: we also clear the table at the beginning of every hand */
		game.players[p].table = UNKNOWN_CARD;

		fd = ggz_seats[p].fd;
		if (fd == -1) continue;

		es_write_int(fd, WH_MSG_TRICK);
		es_write_int(fd, CONVERT_SEAT(game.players[winner].seat, p));
	}
}

static int req_newgame(player_t p)
{
	int fd;
	fd = ggz_seats[ p ].fd;
	if (fd == -1) return -1;	/* TODO: handle AI */

	ggz_debug("Sending out a WH_REQ_NEWGAME to player %d.", p);
	return es_write_int(fd, WH_REQ_NEWGAME);
}

static int send_newgame()
{
	int fd;
	player_t p;
	int status = 0;

	ggz_debug("Sending out a newgame message.");

	for (p=0; p < game.num_players; p++) {
		fd = ggz_seats[p].fd;
		if (fd == -1) continue;
		if (es_write_int(fd, WH_MSG_NEWGAME) < 0)
			status = -1;
	}

	return status;
}

static int rec_options(int num_options, int* options)
{
	int fd = ggz_seats[game.host].fd, status = 0, i;
	if (fd == -1) {
		ggz_debug("SERVER bug: unknown host in rec_options.");
		exit(-1);
	}
	
	for (i = 0; i < num_options; i++)
		if (es_read_int(fd, &options[i]) < 0)
			status = options[i] = -1;

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
	
	if(es_read_int(fd, &op) < 0)
		return -1;

	if (op >= 0 && op <= WH_REQ_SYNC)
		ggz_debug("Received %d (%s) from player %d.", op, player_messages[op], p);
	else
		ggz_debug("Received unknown message %d from player %d.", op, p);

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
						ggz_debug("SERVER/CLIENT error: bad game type %d selected; using %d instead.", game.which_game, GGZ_DEFAULT_GAME);
						game.which_game = GGZ_DEFAULT_GAME;
					}

					game_init_game();
					send_sync_all();

					if (!ggz_seats_open())
						next_play();
				} else {
					int options[game.num_options];
					rec_options(game.num_options, options);
					game_handle_options(options);
					if (game.options_initted)
						/* TODO: what if options aren't initted? */
						next_play();
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
static int newgame()
{
	player_t p;

	if (game.which_game == GGZ_GAME_UNKNOWN) {
		ggz_debug("SERVER BUG: starting newgame() on unknown game.");
		exit(-1);
	}

	/* should be entered only when we're ready for a new game */
	for (p=0; p<game.num_players; p++) game.players[p].ready = 0;
		/* TODO: for now these two are together, but they
		 * could/should be spread out */
	if (!game.initted) game_init_game();
	game_start_game();
	send_newgame();
	game.dealer = random() % game.num_players;
	set_game_state( WH_STATE_NEXT_HAND );
	next_play();
}

/* Do the next play */
static void next_play(void)
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
				/* TODO: fix memory leaks */
				set_game_state( WH_STATE_NOTPLAYING );
				return;
			}

			/* shuffle and deal a hand */
			cards_shuffle_deck();
			for(p=0; p<game.num_players; p++)
				game.players[p].tricks = 0;
			if(game_deal_hand() < 0)
				return;

			/* Now send the resulting hands to each player */
			for(p=0; p<game.num_players; p++)
				for (s=0; s<game.num_seats; s++)
					game_send_hand(p, s);

			set_game_state( WH_STATE_FIRST_BID );
			ggz_debug("Done generating the next hand; entering bidding phase.");
			next_play(); /* recursion */
			break;
		case WH_STATE_FIRST_BID:
			ggz_debug("Next play: starting the bidding.");
			set_game_state( WH_STATE_NEXT_BID );

			for (p = 0; p < game.num_players; p++)
				game.players[p].bid.bid = 0;
			game.bid_count = 0;

			game_start_bidding();
			next_play(); /* recursion */
			break;
		case WH_STATE_NEXT_BID:
			ggz_debug("Next play: bid %d/%d - player %d.", game.bid_count, game.bid_total, game.next_bid);
			game_get_bid();
			break;
		case WH_STATE_NEXT_PLAY:
			ggz_debug("Next play: playing %d/%d - player %d.", game.play_count, game.play_total, game.next_play);
			game_get_play(game.next_play);
			break;
		case WH_STATE_FIRST_TRICK:
			ggz_debug("Next play: first trick of the hand.");
			set_game_state( WH_STATE_NEXT_TRICK );

			game.trick_total = game.hand_size;
			game.play_total = game.num_players;
			/* we also clear all cards after every trick */
			for (p=0; p<game.num_players; p++)
				game.players[p].table = UNKNOWN_CARD;

			game_start_playing();
			set_all_player_messages();
			game.trump_broken = 0;
			game.next_play = game.leader;
			game.play_count = game.trick_count = 0;
			next_play();
			break;
		case WH_STATE_NEXT_TRICK:
			ggz_debug("Next play: next trick %d/%d - leader is %d.", game.trick_count, game.trick_total, game.leader);
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
static int update(int event, void *data)
{
	bid_t bid;
	int bid_index;
	player_t p, player;
	int ready = 0;
	
	switch (event) {
		case WH_EVENT_LAUNCH:
			ggz_debug("Handling a WH_EVENT_LAUNCH.");
			if(game.state != WH_STATE_PRELAUNCH)
				return -1;
			/* determine number of players. */
			game.num_players = ggz_seats_num(); /* ggz seats == players */
			game.players = (struct game_player_t *)alloc(game.num_players * sizeof(struct game_player_t));

			/* TODO: for now we assume 4 seats, which is all that is supported by the client.
			 * However, this is a big issue with the multi-game capability.  We may not
			 * know what game we're playing until later. */
			game.num_seats = 4;
			game.seats = (struct game_seat_t *)alloc(game.num_seats * sizeof(struct game_seat_t));

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
				game_set_player_message(player); /* should this be in sync??? */

			if (player == game.host && game.which_game == GGZ_GAME_UNKNOWN)
				game_req_gametype();

			if (!ggz_seats_open() && game.which_game != GGZ_GAME_UNKNOWN)
				/* (Re)Start game play */
				next_play();
			break;
		case WH_EVENT_NEWGAME:
                        player = *(player_t *)data;
			ggz_debug("Handling a WH_EVENT_NEWGAME for player %d.", player);
			game.players[player].ready = 1;
			ready = 1;
			if (player == game.host && !game.options_initted &&
			    game_get_options() < 0) {
				ggz_debug("Error in game_get_options.  Using defaults.");
				game.options_initted = 1;
			}
			for (p=0; p<game.num_players; p++)
				if (!game.players[p].ready) {
					/* TODO: should another WH_REQ_NEWGAME be sent, just as a reminder?
					 * if we do, then the client may not be able to determine that it's a duplicate... */
					ggz_debug("Player %d is not ready.", p);
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
			game.players[game.next_play].table = c;
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
				ggz_debug("End of trick; %d/%d.", game.trick_count, game.trick_total);
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
			bid = game.bid_choices[bid_index];
			game.players[game.next_bid].bid = bid;

			/* handle the bid */
			game_handle_bid(bid_index);

			/* set up next move */
			game.bid_count++;
			game_next_bid();
			if (game.bid_count == game.bid_total)
				set_game_state(WH_STATE_FIRST_TRICK);
			else if (game.state == WH_STATE_WAIT_FOR_BID)
				set_game_state(WH_STATE_NEXT_BID);
			else
				ggz_debug("SERVER BUG: handled WH_EVENT_BID while not in WH_STATE_WAIT_FOR_BID.");

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

static void set_all_player_messages()
{
	player_t p;
	for (p=0; p<game.num_players; p++)
		game_set_player_message(p);	
}


/*
 * JDS: these functions are game-specific or contain game-specific code
 */

/* will be used to determine the game type from command-line parameter  */
int game_get_gametype(char* text)
{
	if (!strcmp(text, "bridge"))
		return GGZ_GAME_BRIDGE;

	if (!strcmp(text, "suaro"))
		return GGZ_GAME_SUARO;

	if (!strcmp(text, "lapocha"))
		return GGZ_GAME_LAPOCHA;

	if (!strcmp(text, "spades"))
		return GGZ_GAME_SPADES;

	if (!strcmp(text, "hearts"))
		return GGZ_GAME_HEARTS;

	/* NOTE: we may not yet be connected to the ggz server, in which case this won't work. */
	ggz_debug("Unknown game for '%s'.", text);
	return GGZ_GAME_UNKNOWN;
}

/* as soon as the game type is determined, the game should be initialized.  This is called
 * by game-independent code, but may be called in one of several places (e.g. launch, connection, newgame) */
/* TODO: it may be necessary to uninitialize a game before initializing a new one */
static void game_init_game()
{
	player_t p;
	seat_t s;
	int proper_players = 4; /* the correct number of players */

	if (game.initted || game.which_game == GGZ_GAME_UNKNOWN) {
		ggz_debug("SERVER BUG: game_init_game called on unknown or previously initialized game.");
		return;
	}

	/* First, allocate the game */
	switch (game.which_game) {
		case GGZ_GAME_LAPOCHA:
			game.specific = alloc(sizeof(lapocha_game_t));
			break;
		case GGZ_GAME_SUARO:
			game.specific = alloc(sizeof(suaro_game_t));
			break;
		case GGZ_GAME_SPADES:
			game.specific = alloc(sizeof(spades_game_t));
			break;
		case GGZ_GAME_HEARTS:
			game.specific = alloc(sizeof(hearts_game_t));
			break;
		case GGZ_GAME_BRIDGE:
			game.specific = alloc(sizeof(bridge_game_t));
			break;
		default:
			ggz_debug("game_alloc_game not implemented for game %d.", game.which_game);
			exit(-1);
	}

	cards_create_deck(game.which_game);

	/* TODO: see TODO by other game.seats allocation */
	if (game.num_seats != 4) {
		game.num_seats = 4; /* default value */
		game.seats = (struct game_seat_t *)alloc(game.num_seats * sizeof(struct game_seat_t));
	}

	/* second round of game-specific initialization */
	switch (game.which_game) {
		case GGZ_GAME_SUARO:
			{
			static struct ggz_seat_t ggz[2] = { {GGZ_SEAT_NONE, "Kitty", -1},
							    {GGZ_SEAT_NONE, "Key Card", -1} };
			proper_players = 2;
			game.seats[0].ggz = &ggz_seats[0];
			game.players[0].seat = 0;
			game.seats[2].ggz = &ggz_seats[1];
			game.players[1].seat = 2;
			game.seats[1].ggz = &ggz[0];
			game.seats[3].ggz = &ggz[1];
			/* most possible bids for suaro: 6 suits * 5 numbers + pass + double = 32
			 * for shotgun suaro, that becomes 62 possible bids
			 * longest possible bid: 9 diamonds = 11
			 * for shotgun suaro that becomes K 9 diamonds = 13 */
			game.max_bid_choices = 62;
			game.max_bid_length = 13;
			game.max_hand_length = 9;
			game.target_score = 50;
			SUARO.shotgun = 1;	/* shotgun suaro */
			SUARO.declarer = -1;
			game.name = "Suaro";
			}
			break;
		case GGZ_GAME_LAPOCHA:
			proper_players = 4;
			for(p = 0; p < game.num_players; p++) {
				s = p;
				game.players[p].seat = s;
				game.seats[s].ggz = &ggz_seats[p];
			}
			game.max_bid_choices = 12;
			game.max_bid_length = 4;
			game.max_hand_length = 10;
			game.must_overtrump = 1;	/* in La Pocha, you *must* overtrump if you can */
			game.name = "La Pocha";
			break;
		case GGZ_GAME_BRIDGE:
			proper_players = 4;
			for(p = 0; p < game.num_players; p++) {
				s = p;
				game.players[p].seat = s;
				game.seats[s].ggz = &ggz_seats[p];
			}
			/* most possible bids for bridge: 7 * 5 suit bids + (re)double + pass = 37
			 * longest possible bid: "redouble" = 9 */
			game.max_bid_choices = 37;
			game.max_bid_length = 9;
			game.max_hand_length = 13;
			game.name = "Bridge";

			/* TODO: for now we won't use bridge scoring */
			BRIDGE.declarer = -1;
			break;
		case GGZ_GAME_SPADES:
			proper_players = 4;
			for(p = 0; p < game.num_players; p++) {
				s = p;
				game.players[p].seat = s;
				game.seats[s].ggz = &ggz_seats[p];
			}
			/* most possible bids for spades: 0-13 + Nil = 15
			 * longest possible bid: "Nil" = length 4 */
			game.max_bid_choices = 15;
			game.max_bid_length = 4;
			game.max_hand_length = 13;
			game.must_break_trump = 1;	/* in spades, you can't lead trump until it's broken */
			game.target_score = 500;	/* TODO: override with options */
			GSPADES.nil_value = 100;
			game.trump = SPADES;
			game.name = "Spades";
			break;
		case GGZ_GAME_HEARTS:
			game.trump = -1; /* no trump in hearts */
			proper_players = 4; /* TODO: doesn't have to be 4 */
			for(p = 0; p < game.num_players; p++) {
				s = p;
				game.players[p].seat = s;
				game.seats[s].ggz = &ggz_seats[p];
			}
			/* hearts has no bidding */
			game.max_bid_choices = 0;
			game.max_bid_length = 0;
			game.max_hand_length = 13;
			game.target_score = 100;
			game.name = "Hearts";
			break;
		default:  	
			ggz_debug("SERVER BUG: game_launch not implemented for game %d.", game.which_game);
			game.name = "(Unknown)";
	}

	set_global_message("game", game.name);
	send_global_message_toall("game");

	/* allocate hands */
	for (s=0; s<game.num_seats; s++) {
		game.seats[s].hand.cards = (card_t *)alloc(game.max_hand_length * sizeof(card_t));
	}

	/* allocate bidding arrays */
	game.bid_texts = alloc_string_array(game.max_bid_choices, game.max_bid_length);
	game.bid_choices = (bid_t*)alloc(game.max_bid_choices * sizeof(bid_t));

	set_global_message("", "");
	for (s = 0; s < game.num_seats; s++)
		game.seats[s].message[0] = 0;
	if (game.bid_texts == NULL || game.bid_choices == NULL) {
		ggz_debug("SERVER BUG: game.bid_texts not allocated.");
		exit(-1);
	}
	if (game.num_players != proper_players) {
		ggz_debug("SERVER BUG: wrong number of players for game %d: %d instead of %d.", game.which_game, game.num_players, proper_players);
		exit(-1);
	}

	game.initted = 1;
}

static int game_req_gametype()
{
	int fd = ggz_seats[game.host].fd;
	int cnt = 0, i;
	int status = 0;
	if (fd == -1) {
		ggz_debug("SERVER BUG: nonexistent host.");
		return -1;
	}

	for (i=0; i < GGZ_NUM_GAMES; i++) {
		if (game_valid_game(i)) {
			game_types[cnt] = i;
			cnt++;
		}
	}

	if (cnt == 0) {
		ggz_debug("SERVER BUG: no valid games in game_req_gametype.");
		exit(-1);
	}

	if (cnt == 1) {
		ggz_debug("Just one valid game: choosing %d.", game_types[0]);
		game.which_game = game_types[0];
		game_init_game();
		send_sync_all();
		return 0;
	}

	if (es_write_int(fd, WH_REQ_OPTIONS) < 0 ||
	    es_write_int(fd, 1) < 0 || /* 1 option */
	    es_write_int(fd, cnt) < 0 || /* cnt choices */
	    es_write_int(fd, 0) < 0) /* default is 0 */
		status = -1;
	for (i=0; i<cnt; i++)
		if (es_write_string(fd, game_names[game_types[i]]) < 0)
			status = -1;

	return status;
}

/* game_get_options
 *   this is very clunky; this must request options from the client
 *   also, everything in it is closely tied to game_handle_options()
 */
static int game_get_options()
{
	int fd;
	fd = ggz_seats[game.host].fd;
	ggz_debug("Entering game_get_options.");
	if (fd == -1) {
		ggz_debug("SERVER BUG: nonexistent host.");
		return -1;
	}
	switch (game.which_game) {
		case GGZ_GAME_SUARO:
			/* four options for now:
			 *   shotgun -> boolean
			 *   unlimited redoubling -> boolean
			 *   persistent doubles -> boolean
			 *   target score -> 25, 50, 100, 200, 500, 1000
			 */
			game.num_options = 4;
			if (es_write_int(fd, WH_REQ_OPTIONS) < 0 ||
				es_write_int(fd, game.num_options) < 0 || /* number of options */
				es_write_int(fd, 1) < 0 || /* first option: 1 choice */
				es_write_int(fd, 1) < 0 || /* first option: default is 1 */
				es_write_string(fd, "shotgun") < 0 || /* the first option is "shotgun" */
				es_write_int(fd, 1) < 0 || /* second option: 1 choice */
				es_write_int(fd, 0) < 0 || /* second option: default is 0 */
				es_write_string(fd, "unlimited redoubling") < 0 ||
				es_write_int(fd, 1) < 0 || /* third option: 1 choice */
				es_write_int(fd, 0) < 0 || /* third option: default is 0 */
				es_write_string(fd, "persistent doubles") < 0 ||
				es_write_int(fd, 6) < 0 || /* fourth option: 6 choices */
				es_write_int(fd, 1) < 0 || /* fourth option: default is 1 (on a scale from 0-2) */
				es_write_string(fd, "Game to 25") < 0 ||
				es_write_string(fd, "Game to 50") < 0 ||
				es_write_string(fd, "Game to 100") < 0 ||
				es_write_string(fd, "Game to 200") < 0 ||
				es_write_string(fd, "Game to 500") < 0 ||
				es_write_string(fd, "Game to 1000") < 0
			   )
				return -1;
			break;
		case GGZ_GAME_SPADES:
			/* two options for now:
			 *   target score: 100, 250, 500, 1000
			 *   nil value: 50, 100
			 */
			game.num_options = 2;
			if (es_write_int(fd, WH_REQ_OPTIONS) < 0 ||
				es_write_int(fd, game.num_options) < 0 || /* number of options */
				es_write_int(fd, 2) < 0 || /* first option: 2 choices */
				es_write_int(fd, 1) < 0 || /* first option: default is 1 */
				es_write_string(fd, "Nil is worth 50") < 0 ||
				es_write_string(fd, "Nil is worth 100") < 0 ||
				es_write_int(fd, 4) < 0 || /* second option: 2 choices */
				es_write_int(fd, 2) < 0 || /* second option: default is 2 */
				es_write_string(fd, "Game to 100") < 0 ||
				es_write_string(fd, "Game to 250") < 0 ||
				es_write_string(fd, "Game to 500") < 0 ||
				es_write_string(fd, "Game to 1000") < 0
			   )
				return -1;
			break;
		default:
			game.options_initted = 1; /* no options */
			break;
	}	
	return 0;
}

/* game_handle_options
 *   handles options being sent from the client
 *   a value of -1 will be passed in as an option to indicate an error */
static void game_handle_options(int *options)
{
	ggz_debug("Entering game_handle_options.");
	switch (game.which_game) {
		case GGZ_GAME_SUARO:
			if (options[0] >= 0) SUARO.shotgun = options[0];
			if (options[1] >= 0) SUARO.unlimited_redoubling = options[1];
			if (options[2] >= 0) SUARO.persistent_doubles = options[2];
			switch (options[3]) {
				/* this highlights a problem with this protocol.  Only discrete entries are possible.
				 * it would be better if the client could just enter a number */
				case 0: game.target_score = 25; break;
				case 1: game.target_score = 50; break;
				case 2: game.target_score = 100; break;
				case 3: game.target_score = 200; break;
				case 4: game.target_score = 500; break;
				case 5: game.target_score = 1000; break;
				default: break;
			}
			game.options_initted = 1; /* we could do another round of option requests, if we wanted */
			break;
		case GGZ_GAME_SPADES:
			switch (options[0]) {
				case 0: GSPADES.nil_value = 50;  break;
				case 1: GSPADES.nil_value = 100; break;
				default: break;
			}
			switch (options[1]) {
				case 0: game.target_score = 100; break;
				case 1: game.target_score = 250; break;
				case 2: game.target_score = 500; break;
				case 3: game.target_score = 1000; break;
				default: break;
			}
			game.options_initted = 1;
			break;
		default:
			ggz_debug("SERVER/CLIENT bug: game_handle_options called incorrectly.");
			break;
	}
}

/* game_start_game
 *   Called at the beginning of a game!
 */
static void game_start_game(void)
{
	player_t p;

	/* TODO: initialize the game; right now we just assume everything's
	 * zero which won't be true the second time around. */
	for (p=0; p<game.num_players; p++) {
		game.players[p].score = 0;
	}

	ggz_debug("Game start completed successfully.");
}

/* game_handle_gameover
 *   The game is over; end out game-over message
 *   game-specific code to determine the winner(s) and
 *   call send_gameover */
static int game_handle_gameover(void)
{
	player_t p;
	int hi_score = -9999;
	player_t winners[game.num_players];
	int winner_cnt = 0;

	switch (game.which_game) {
		case GGZ_GAME_HEARTS:
			/* in hearts, the low score wins */
			{
			int lo_score = 100;	
			for (p=0; p<game.num_players; p++) {
				if (game.players[p].score < lo_score) {
					winner_cnt = 1;
					winners[0] = p;
					lo_score = game.players[p].score;
				} else if (game.players[p].score == lo_score) {
					winners[winner_cnt] = p;
					winner_cnt++;
				}
			}
			}
			break;
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
		case GGZ_GAME_BRIDGE: /* TODO */
		case GGZ_GAME_SUARO:
		case GGZ_GAME_SPADES:
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
 *   called at the very start of bidding
 */
static void game_start_bidding()
{
	switch (game.which_game) {
		case GGZ_GAME_LAPOCHA:
			/* all 4 players bid once, but the first bid determines the trump */
			game.bid_total = 5;
			LAPOCHA.bid_sum = 0;
			break;
		case GGZ_GAME_SUARO:
			{
			char suit;
			game.bid_total = -1; /* no set total */

			/* key card determines first bidder */
			suit = game.seats[3].hand.cards[0].suit;
			game.next_bid = (suit == CLUBS || suit == SPADES) ? 0 : 1;

			SUARO.pass_count = 0;
			SUARO.bonus = 1;
			}
			break;
		case GGZ_GAME_BRIDGE:
			game.next_bid = game.dealer; /* dealer bids first */
			game.bid_total = -1; /* no set total */
			BRIDGE.pass_count = 0;
			BRIDGE.bonus = 1;
			break;
		case GGZ_GAME_HEARTS:
			/* there is no bidding phase */
			set_game_state( WH_STATE_FIRST_TRICK );
			break;
		case GGZ_GAME_SPADES:
		default:
			/* all players bid once */
			game.bid_total = game.num_players;
			game.next_bid = (game.dealer + 1) % game.num_players;
	}
}

/* game_get_bid()
 *   asks for bid from the client/AI
 *   AI can be inserted here; just call update with a WH_EVENT_BID
 */
/* TODO: verify that it will work with and without bots */
static int game_get_bid()
{
	int status = 0, index=0;
	bid_t bid;
	bid.bid = 0;

	switch (game.which_game) {
		case GGZ_GAME_SPADES:
			{
			int i;
			for (i = 0; i <= game.hand_size; i++) {
				bid.sbid.val = i;
				game.bid_choices[i] = bid;
				game_get_bid_text(game.bid_texts[i], game.max_bid_length, game.bid_choices[i]);
			}
			
			/* "Nil" bid */
			bid.bid = 0;
			bid.sbid.spec = SPADES_NIL;
			game.bid_choices[i] = bid;
			game_get_bid_text(game.bid_texts[i], game.max_bid_length, game.bid_choices[i]);

			/* TODO: other specialty bids */

			status = req_bid(game.next_bid, game.hand_size+2, game.bid_texts);
			}
			break;
		case GGZ_GAME_LAPOCHA:
			if (game.bid_count == 0) { /* determine the trump suit */
				/* handled just like a bid */
				if(game.hand_size != 10) {
					bid.bid = (long)cards_deal_card().suit;
					update(WH_EVENT_BID, (void*)&bid);
				} else
					status = req_bid(game.dealer, 4, suit_names);
			} else { /* get a player's numberical bid */
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
					game_get_bid_text(game.bid_texts[num], game.max_bid_length, game.bid_choices[num]);
					num++;
				}

				status = req_bid(game.next_bid, num, game.bid_texts);
			}
			break;
		case GGZ_GAME_BRIDGE:
			/* this is based closely on the Suaro code, below */

			/* make a list of regular bids */
			for (bid.sbid.val = 1; bid.sbid.val <= 7; bid.sbid.val++) {
				for (bid.sbid.suit = CLUBS; bid.sbid.suit <= BRIDGE_NOTRUMP; bid.sbid.suit++) {
					if (bid.sbid.val < BRIDGE.contract) continue;
					if (bid.sbid.val == BRIDGE.contract && bid.sbid.suit <= BRIDGE.contract_suit) continue;
					snprintf(game.bid_texts[index], game.max_bid_length,
					"%d %s", bid.sbid.val, bridge_suit_names[(int)bid.sbid.suit]);
					game.bid_choices[index] = bid;
					index++;
				}
			}

			/* make "double" or "redouble" bid */
			if (BRIDGE.contract != 0
				/* TODO: one double *per team* */
			    && BRIDGE.bonus < 4) {
				bid.bid = 0;
				bid.sbid.spec = BRIDGE_DOUBLE;
				game.bid_choices[index] = bid;
				snprintf(game.bid_texts[index], game.max_bid_length,
					BRIDGE.bonus == 1 ? "double" : "redouble");
				index++;
			}

			/* make "pass" bid */
			bid.bid = 0;
			bid.sbid.spec = BRIDGE_PASS;
			game.bid_choices[index] = bid;
			snprintf(game.bid_texts[index], game.max_bid_length, "pass");
			index++;

			status = req_bid(game.next_bid, index, game.bid_texts);
			break;
		case GGZ_GAME_SUARO:
			/* in suaro, a bid consists of a number and a suit. */

			/* make a list of regular bids */
			for(bid.sbid.val = 5; bid.sbid.val <= 9; bid.sbid.val++) {
				for(bid.sbid.suit = SUARO_LOW; bid.sbid.suit <= SUARO_HIGH; bid.sbid.suit++) {
					if (bid.sbid.val < SUARO.contract) continue;
					if (bid.sbid.val == SUARO.contract && bid.sbid.suit <= SUARO.contract_suit) continue;
					snprintf(game.bid_texts[index], game.max_bid_length,
						"%d %s", bid.sbid.val, suaro_suit_names[(int)bid.sbid.suit]);
					game.bid_choices[index] = bid;
					index++;
					if (SUARO.shotgun) {
						/* in "shotgun" suaro, you are allowed to bid on the kitty just like on your hand! */
						bid_t kbid = bid;
						kbid.sbid.spec = SUARO_KITTY;
						snprintf(game.bid_texts[index], game.max_bid_length,
							"K %d %s", kbid.sbid.val, suaro_suit_names[(int)kbid.sbid.suit]);
						game.bid_choices[index] = kbid;
						index++;
					}
				}
			}

			/* make "double" or "redouble" bid */
			if ( SUARO.contract > 0
			     && (SUARO.bonus < 4 || SUARO.unlimited_redoubling)) {
				/* unless unlimited doubling is specifically allowed,
				 * only double and redouble are possible */
				bid.bid = 0;
				bid.sbid.spec = SUARO_DOUBLE;
				game.bid_choices[index] = bid;
				snprintf(game.bid_texts[index], game.max_bid_length,
					 SUARO.bonus == 1 ? "double" : "redouble");
				index++;
			}

			/* make "pass" bid */
			bid.bid = 0;
			bid.sbid.spec = SUARO_PASS;
			game.bid_choices[index] = bid;
			snprintf(game.bid_texts[index], game.max_bid_length, "pass");
			index++;

			status = req_bid(game.next_bid, index, game.bid_texts);
			break;
		default:
			ggz_debug("SERVER BUG: game_get_bid called for unimplemented game.");
			status = -1;
	}

	return status;
}

/* Handle incoming bid from player */
static int game_handle_bid(int bid_index)
{
	bid_t bid;
	bid.bid = -1;

	ggz_debug("Entering game_handle_bid(%d).", bid_index);

	switch (game.which_game) {
		case GGZ_GAME_BRIDGE:
			/* closely based on the Suaro code, below */
			bid = game.bid_choices[bid_index];

			ggz_debug("The bid chosen is %d %s %d.", bid.sbid.val, bridge_suit_names[(int)bid.sbid.suit], bid.sbid.spec);
			
			if (bid.sbid.spec == BRIDGE_PASS) {
				BRIDGE.pass_count++;
			} else if (bid.sbid.spec == BRIDGE_DOUBLE) {
				BRIDGE.pass_count = 1;
				BRIDGE.bonus *= 2;
			} else {
				BRIDGE.declarer = game.next_bid;
				BRIDGE.dummy = (BRIDGE.declarer + 2) % 4;
				BRIDGE.pass_count = 1;
				BRIDGE.contract = bid.sbid.val;
				BRIDGE.contract_suit = bid.sbid.suit;
				ggz_debug("Setting bridge contract to %d %s.", BRIDGE.contract, bridge_suit_names[BRIDGE.contract_suit]);
				if (bid.sbid.suit < BRIDGE_NOTRUMP)
					game.trump = bid.sbid.suit;
				else
					game.trump = -1;
			}
			if (BRIDGE.contract)
				set_global_message("", "%d %s x%d", BRIDGE.contract,
					bridge_suit_names[(int)BRIDGE.contract_suit], BRIDGE.bonus);
			else
				set_global_message("", "no contract");
			send_global_message_toall("");
			break;
		case GGZ_GAME_SUARO:
			bid = game.bid_choices[bid_index];

			if (bid.sbid.spec == SUARO_PASS) {
				SUARO.pass_count++;
			} else if (bid.sbid.spec == SUARO_DOUBLE) {
				SUARO.pass_count = 1; /* one more pass will end it */
				SUARO.bonus *= 2;
			} else {
				SUARO.declarer = game.next_bid;
				SUARO.pass_count = 1; /* one more pass will end it */
				if (!SUARO.persistent_doubles)
					SUARO.bonus = 1; /* reset any doubles */
				SUARO.contract = bid.sbid.val;
				SUARO.kitty = (bid.sbid.spec == SUARO_KITTY);
				SUARO.contract_suit = bid.sbid.suit;
				if (bid.sbid.suit > SUARO_LOW && bid.sbid.suit < SUARO_HIGH)
					game.trump = bid.sbid.suit - 1; /* a minor hack */
				else
					game.trump = -1;
			}
			if (SUARO.contract)
				set_global_message("", "%s%d %s x%d",
					SUARO.kitty ? "K " : "",
					SUARO.contract, suaro_suit_names[(int)SUARO.contract_suit], SUARO.bonus);
			else
				set_global_message("", "no contract");
			send_global_message_toall("");
			break;
		case GGZ_GAME_SPADES:
			break; /* no special handling necessary */
		case GGZ_GAME_LAPOCHA:
			if (game.bid_count == 0) {
				game.trump = bid_index;

				set_global_message("", "Trump: %s.", suit_names[(int)game.trump % 4]);
				send_global_message_toall("");
			} else {
				bid = game.bid_choices[bid_index];
				LAPOCHA.bid_sum += bid.bid;
			}
			break;	
		default:
			ggz_debug("SERVER Not Implemented: game_handle_bid.");
 			break;
	}

	game_set_player_message(game.next_bid);
	
	return 0;	
}

/* sets up for the next bid
 * note that game.bid_count has already been incremented */
static void game_next_bid()
{
	ggz_debug("Determining next bid.");
	switch (game.which_game) {
		case GGZ_GAME_BRIDGE:
			/* closely based on Suaro code, below */
			if (BRIDGE.pass_count == 4) {
				/* done bidding */
				if (BRIDGE.contract == 0) {
					ggz_debug("Four passes; redealing hand.");
					set_global_message("", "redeal");
					send_global_message_toall("");
					set_game_state( WH_STATE_NEXT_HAND ); /* redeal hand */
				} else {
					ggz_debug("Three passes; bidding is over.");
					game.bid_total = game.bid_count;
					/* contract was determined in game_handle_bid */
				}
			} else {
				if (game.bid_count == 0)
					game.next_bid = game.dealer;
				else
					game.next_bid = (game.next_bid + 1) % game.num_players;
			}
			break;
		case GGZ_GAME_SUARO:
			if (SUARO.pass_count == 2) {
				/* done bidding */
				if (SUARO.contract == 0) {
					ggz_debug("Two passes; redealing hand.");
					set_global_message("", "redeal");
					send_global_message_toall("");
					set_game_state( WH_STATE_NEXT_HAND ); /* redeal hand */
				} else {
					ggz_debug("A pass; bidding is over.");
					game.bid_total = game.bid_count;
					/* contract was determined in game_handle_bid */
				}
			} else
				goto normal_order;
			break;
		case GGZ_GAME_LAPOCHA:
			if (game.bid_count == 1) {
				game.next_bid = (game.dealer + 1) % game.num_players;
				break;
			}
			goto normal_order;
		case GGZ_GAME_SPADES:
		default:
normal_order:
			if (game.bid_count == 0)
				game.next_bid = (game.dealer + 1) % game.num_players;
			else
				game.next_bid = (game.next_bid + 1) % game.num_players;
	}
	
}

/* called between the bidding and playing sequences */
static void game_start_playing(void)
{
	player_t p;

	switch (game.which_game) {
		case GGZ_GAME_LAPOCHA:
			game.leader = (game.dealer + 1) % game.num_players;
			set_global_message("", "%d %s x%d", BRIDGE.contract,
				bridge_suit_names[(int)BRIDGE.contract_suit], BRIDGE.bonus);
			send_global_message_toall("");
			break;
		case GGZ_GAME_BRIDGE:
			/* declarer is set in game_handle_bid */
			game.leader = (BRIDGE.declarer + 1) % game.num_players;
			game.trick_total = game.hand_size;
			game.play_total = game.num_players;
			break;
		case GGZ_GAME_SUARO:
			/* declarer is set in game_handle_bid */
			game.leader = 1 - SUARO.declarer;
			set_global_message("", "%s%d %s x%d",
				SUARO.kitty ? "K " : "",
				SUARO.contract, suaro_suit_names[(int)SUARO.contract_suit], SUARO.bonus);
			send_global_message_toall("");
			if (SUARO.kitty) {
				/* if it's a kitty bid, the declarer takes up the kitty and
				 * lays their own hand down (face up) in its place */
				card_t* temp;
				seat_t s1, s2;
				s1 = 1; /* kitty hand */
				s2 = game.players[SUARO.declarer].seat;
				/* this "swapping" only works because the cards in the hands
				 * are specifically allocated by malloc.  Also note that there
				 * are always 9 cards in both hands. */
				temp = game.seats[s1].hand.cards;
				game.seats[s1].hand.cards = game.seats[s2].hand.cards;
				game.seats[s2].hand.cards = temp;
				send_hand(SUARO.declarer, s2, 1); /* reveal the new hand to the player */
				for(p = 0; p<game.num_players; p++)
					/* reveal the kitty to everyone */
					send_hand(p, s1, 1);
			}
			break;
		case GGZ_GAME_HEARTS:
			/* we track the points that will be earned this hand. */
			for (p=0; p<game.num_players; p++)
				GHEARTS.points_on_hand[p] = 0;
		case GGZ_GAME_SPADES:
			/* fall through */
		default:
			game.leader = (game.dealer + 1) % game.num_players;
			break;
	}
}

/* verify that the play is legal; return NULL if it is an an error message otherwise
 * the error message should be statically declared! */
static char* game_verify_play(int card_index)
{
	card_t card = game.seats[game.play_seat].hand.cards[card_index], c;
	seat_t s = game.play_seat;
	int cnt;

	/* TODO: hearts must be handled differently, since
	 * you can't lead *hearts* until *points* have been broken */

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
	if ( (cnt = cards_suit_in_hand(&game.seats[s].hand, game.lead_card.suit)) ) {
		ggz_debug("Player %d, playing from seat %d, played %s when they have %d of %s.",
			  game.next_play, s, suit_names[(int)card.suit], cnt, suit_names[(int)game.lead_card.suit]);
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
			c = game.players[p2].table;
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

/* sets up for the next play
 * note that game.play_count has already been incremented */
static void game_next_play()
{
	game.next_play = (game.next_play + 1) % game.num_players;
}

/* this gets a play.  It most likely just requests one from the player, but
 * AI can be inserted to call update on a WH_EVENT_PLAY */
static void game_get_play(player_t p)
{
	switch (game.which_game) {
		case GGZ_GAME_BRIDGE:
			if (p == BRIDGE.dummy) {
				/* the declarer plays the dummy's hand */
				req_play(BRIDGE.declarer, game.players[p].seat);
				break;
			}
			/* else fall through */
		default:
			/* in almost all cases, we just want the player to play from their own hand */
			req_play(p, game.players[p].seat);
			break;
	}
}

/* this handles a play.  Just about everything is taken care of by the
 * game-independent code; all that needs to be done here is anything
 * game-specific. */
static void game_handle_play(card_t c)
{
	switch (game.which_game) {
		case GGZ_GAME_BRIDGE:
			if (game.play_count == 0 && game.trick_count == 0) {
				/* after the first play of the hand, we reveal
				 * the dummy's hand to everyone */
				player_t p;
				seat_t dummy_seat = game.players[BRIDGE.dummy].seat;
				for (p=0; p<game.num_players; p++) {
					if (p == BRIDGE.dummy) continue;
					send_hand(p, dummy_seat, 1);
				}
			}
			break;
		default:
			/* nothing needs to be done... */
			break;
	}
}

/* game_test_for_gameover
 *   called at the beginning of a new hand to determine if the game is over
 */
static int game_test_for_gameover()
{
	player_t p;
	switch (game.which_game) {
		case GGZ_GAME_LAPOCHA:
			return (game.hand_num == 29);
		case GGZ_GAME_BRIDGE:
			ggz_debug("FIXME: bridge scoring not implemented.");
			return 0;
		case GGZ_GAME_SPADES:
		case GGZ_GAME_HEARTS:
		case GGZ_GAME_SUARO:
		default:
			/* in the default case, it's just a race toward a target score */
			for (p = 0; p < game.num_players; p++)
				if (game.players[p].score >= game.target_score)
					return 1;
			return 0;
	}

	return 0;
}


/* The number of cards dealt each hand */
static int lap_card_count[] = { 1, 1, 1, 1,
			    2, 3, 4, 5, 6, 7, 8, 9,
			    10, 10, 10, 10,
			    9, 8, 7, 6, 5, 4, 3, 2,
			    1, 1, 1, 1 };

/* Generate and send out a new hand */
static int game_deal_hand(void)
{
	seat_t s;
	int result=0;

	ggz_debug("Dealing hand %d.", game.hand_num);

	switch (game.which_game) {
		case GGZ_GAME_LAPOCHA:
			/* in la pocha, there are a predetermined number of cards
			 * each hand, as defined by lap_card_count above */
			game.hand_size = lap_card_count[game.hand_num];
			game.trump = -1; /* must be determined later */
			goto regular_deal;
		case GGZ_GAME_SUARO:
			/* in suaro, players 0 and 1 (seats 0 and 2) get 9 cards each.
			 * the kitty (seat 1) also gets 9 cards, and the face card
			 * (seat 3) is just one card. */
			game.hand_size = 9;
			for(s = 0; s < 3; s++)
				cards_deal_hand(game.hand_size, &game.seats[s].hand);
			cards_deal_hand(1, &game.seats[3].hand);
			break;		
		case GGZ_GAME_SPADES:
		case GGZ_GAME_BRIDGE:
		case GGZ_GAME_HEARTS:
		default:
			game.hand_size = 52 / game.num_players;
regular_deal:
			for (s = 0; s < game.num_seats; s++)
				cards_deal_hand(game.hand_size, &game.seats[s].hand);
	}

	ggz_debug("Dealt hand with result %d.", result);

	return result;
}


/* game_send_hand
 *   Show a player a hand.
 */
static int game_send_hand(player_t p, seat_t s)
{
	switch (game.which_game) {
		case GGZ_GAME_SUARO:
			/* each player can see their own hand plus the
			 * key card */
			return send_hand(p, s,
				game.players[p].seat == s || s == 3);
		case GGZ_GAME_BRIDGE:
			/* TODO: we explicitly send out the dummy hand, but a player who
			 * joins late won't see it.  We have the same problem with Suaro. */
			/* fall through */
		default:
			return send_hand(p, s, game.players[p].seat == s);
	}
	return -1;
}

/* game_get_bid_text
 *   places text for the bid into the buffer.  Returns the length of the text (from snprintf).
 */
static int game_get_bid_text(char* buf, int buf_len, bid_t bid)
{
	switch (game.which_game) {
		case GGZ_GAME_SUARO:
			if (bid.sbid.spec == SUARO_PASS) return snprintf(buf, buf_len, "Pass");
			if (bid.sbid.spec == SUARO_DOUBLE) return snprintf(buf, buf_len, "Double"); /* TODO: differentiate redouble */
			return snprintf(buf, buf_len, "%s%d %s", (bid.sbid.spec == SUARO_KITTY) ? "K " : "", bid.sbid.val, suaro_suit_names[(int)bid.sbid.suit]);
		case GGZ_GAME_BRIDGE:
			if (bid.sbid.spec == BRIDGE_PASS) return snprintf(buf, buf_len, "Pass");
			if (bid.sbid.spec == BRIDGE_DOUBLE) return snprintf(buf, buf_len, "Double"); /* TODO: differentiate redouble */
			return snprintf(buf, buf_len, "%d %s", bid.sbid.val, bridge_suit_names[(int)bid.sbid.suit]);
		case GGZ_GAME_SPADES:
			if (bid.sbid.spec == SPADES_NIL) return snprintf(buf, buf_len, "Nil");
			return snprintf(buf, buf_len, "%d", (int)bid.sbid.val);
		case GGZ_GAME_LAPOCHA:
			return snprintf(buf, buf_len, "%d", (int)bid.bid);
		case GGZ_GAME_HEARTS:
		default:
			return snprintf(buf, buf_len, "%s", "");
	}
}

static void game_set_player_message(player_t p)
{
	seat_t s = game.players[p].seat;
	char* message = game.seats[s].message;
	int len = 0;

	/* This function is tricky.  The problem is that we're trying to assemble a single player string out of
	 * multiple units of data - score, bid, tricks, etc.  The solution here is to integrate these all into one
	 * function (this one).  The problem is that you have to check each unit of data to see if it should be
	 * shown - for instance, tricks should only be shown while the hand is being played.  Another solution
	 * would be to update things separately.  The disadvantage there is that the code is all spread out, and
	 * it's difficult to deal with multiple units of data */

	/* Note that this way depends on this function being called at the proper times -
	 * i.e. every time the player is affected, and on some game state changes.  Much
	 * of this is handled by the game-independent code */

	/* anyway, it's really ugly, but I don't see a better way... */

	switch (game.which_game) {
		case GGZ_GAME_BRIDGE:
			if (p == BRIDGE.declarer)
				len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "declarer\n");
			/* TODO */
			goto normal_message;
		case GGZ_GAME_SUARO:
			if (p == SUARO.declarer)
				len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "declarer\n");
			/* TODO: declarer really shouldn't be at the top */
			goto normal_message;
		case GGZ_GAME_LAPOCHA:
			if (game.state != WH_STATE_NEXT_BID && game.state != WH_STATE_WAIT_FOR_BID) {
				len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Contract: %d\n", (int)game.players[p].bid.bid);
			}
			goto normal_message;
		case GGZ_GAME_SPADES:
			len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Score: %d (%d)\n", game.players[p].score, GSPADES.bags[p%2]);
			if (game.state != WH_STATE_NEXT_BID && game.state != WH_STATE_WAIT_FOR_BID) {
				/* we show both the individual and team contract */
				char bid_text[game.max_bid_length];
				int contract = game.players[p].bid.sbid.val + game.players[(p+2)%4].bid.sbid.val;
				game_get_bid_text(bid_text, game.max_bid_length, game.players[p].bid);
				if (*bid_text) len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Bid: %s (%d)\n", bid_text, contract);
			}
			if (game.state == WH_STATE_WAIT_FOR_PLAY || game.state == WH_STATE_NEXT_TRICK || game.state == WH_STATE_NEXT_PLAY) {
				len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Tricks: %d (%d)\n",
					game.players[p].tricks, game.players[p].tricks + game.players[(p+2)%4].tricks);

			}
			goto bid_message_only;
		case GGZ_GAME_HEARTS:
		default:
normal_message:
			len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Score: %d\n", game.players[p].score);
			if (game.state == WH_STATE_WAIT_FOR_PLAY || game.state == WH_STATE_NEXT_TRICK || game.state == WH_STATE_NEXT_PLAY)
				len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Tricks: %d\n", game.players[p].tricks);
bid_message_only:
			if (game.state == WH_STATE_NEXT_BID || game.state == WH_STATE_WAIT_FOR_BID) {
				char bid_text[game.max_bid_length];
				game_get_bid_text(bid_text, game.max_bid_length, game.players[p].bid);
				if (*bid_text) len += snprintf(message+len, MAX_MESSAGE_LENGTH-len, "Bid: %s\n", bid_text);
			}
			break;
	}

	send_player_message_toall(s);
}

/* Figure who won the trick
 * game.winner will be announced as the winner */
static void game_end_trick(void)
{
	player_t hi_player = game.leader, p, lo_player = game.leader;
	card_t hi_card = game.lead_card, lo_card = game.lead_card;

	ggz_debug("Scoring trick.");

	/* default method of winning tricks: the winning card is the highest
	 * card of the suit lead, or the highest trump if there is trump */
	for (p=0; p<game.num_players; p++) {
		card_t card = game.players[p].table;
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
		case GGZ_GAME_SUARO:
			if (SUARO.contract_suit == SUARO_LOW) {
				/* low card wins */
				hi_player = lo_player;
				hi_card = lo_card;
			}			
			break;
		case GGZ_GAME_HEARTS:
			for(p=0; p<game.num_players; p++) {
				card_t card = game.players[p].table;
				if (card.suit == HEARTS)
					GHEARTS.points_on_hand[hi_player]++;
				if (card.suit == SPADES && card.face == QUEEN)
					GHEARTS.points_on_hand[hi_player] += 13;
			}
			break;
		case GGZ_GAME_BRIDGE:
		case GGZ_GAME_SPADES:
			/* update teammate's info as well */
			game_set_player_message((hi_player+2)%4);
			break;
		case GGZ_GAME_LAPOCHA:		
		default:
			/* no additional scoring is necessary */
			break;
	}

	game.players[hi_player].tricks++;
	game.leader = game.winner = hi_player;

	game_set_player_message(hi_player);
}


/* Calculate scores for this hand and announce */
static void game_end_hand(void)
{
	player_t p;

	switch (game.which_game) {
		case GGZ_GAME_LAPOCHA:
			for(p=0; p<game.num_players; p++) {
				ggz_debug("Player %d got %d tricks on a bid of %d", p,
					  game.players[p].tricks, (int)game.players[p].bid.bid);
				if(game.players[p].tricks == game.players[p].bid.bid)
					game.players[p].score += 10 + (5 * game.players[p].bid.bid);
				else
					game.players[p].score -= 5 * game.players[p].bid.bid;
			}
			break;
		case GGZ_GAME_BRIDGE:
			{
			int points_above, points_below, tricks;
			int winning_team, team;
			int tricks_above, tricks_below;
			tricks = game.players[BRIDGE.declarer].tricks + game.players[BRIDGE.dummy].tricks;
			if (tricks >= BRIDGE.contract) {
				winning_team = BRIDGE.declarer % 2;
				tricks_below = BRIDGE.contract;
				tricks_above = tricks - BRIDGE.contract;

			} else {
				winning_team = (BRIDGE.declarer + 1) % 2;
				tricks_below = 0;
				tricks_above = BRIDGE.contract - tricks;
			}
			switch (BRIDGE.contract_suit) {
				case CLUBS:
				case DIAMONDS:
					points_below = 20 * tricks_below;
					points_above = 20 * tricks_above;
					break;
				case HEARTS:
				case SPADES:
					points_below = 30 * tricks_below;
					points_above = 30 * tricks_above;
					break;
				case BRIDGE_NOTRUMP:
				default:
					points_below = 30 * tricks_below + (tricks_below > 0) ? 10 : 0;
					points_above = 30 * tricks_above;
					break;
			}
			points_below *= BRIDGE.bonus;
			points_above *= BRIDGE.bonus;
			BRIDGE.points_above_line[winning_team] += points_above;
			BRIDGE.points_below_line[winning_team] += points_below;
			
			if (BRIDGE.points_below_line[winning_team] >= 100) {
				/* 500 point bonus for winning a game */
				BRIDGE.points_above_line[winning_team] += 500;
				for (team=0; team<2; team++) {
					BRIDGE.points_above_line[team] += BRIDGE.points_below_line[team];
					BRIDGE.points_below_line[team] = 0;
				}
			}
			/* TODO: vulnerable, etc. */
			}

			BRIDGE.declarer = BRIDGE.dummy = -1;
			BRIDGE.contract = 0;
			break;
		case GGZ_GAME_SUARO:
			{
			int points, tricks;
			player_t winner;
			tricks = game.players[SUARO.declarer].tricks;
			if (tricks >= SUARO.contract) {
				int overtricks = tricks - SUARO.contract;
				winner = SUARO.declarer;
				/* you get the value of the contract MINUS
				 * the number of overtricks == 2 * contract - tricks */
				points = (SUARO.contract - overtricks) * SUARO.bonus;
				ggz_debug("Player %d made their bid of %d, plus %d overtricks for %d points.",
					  winner, SUARO.contract, overtricks, points);
			} else {
				winner = 1-SUARO.declarer;
				/* for setting, you just get the value of the contract */
				points = SUARO.contract * SUARO.bonus;
				ggz_debug("Player %d set the bid of %d, earning %d points.",
					  winner, SUARO.contract, points);
			}
			game.players[winner].score += points;
			SUARO.declarer = -1;
			SUARO.contract = 0;
			}
			break;
		case GGZ_GAME_SPADES:
			for (p=0; p<2; p++) {
				int tricks, bid, score;
				bid = game.players[p].bid.sbid.val + game.players[p+2].bid.sbid.val;
				tricks = game.players[p].tricks + game.players[p+2].tricks;
				if (tricks >= bid) {
					int bags = tricks-bid;
					score = 10 * bid + bags;
					GSPADES.bags[p] += bags;
					if (GSPADES.bags[p] >= 10) {
						/* you lose 100 points for 10 overtricks */
						GSPADES.bags[p] -= 10;
						score -= 100;
					}
				} else
					score = -10 * bid;
				ggz_debug("Team %d bid %d, took %d, earned %d.", (int)p, bid, tricks, score);
				game.players[p].score += score;
				game.players[p+2].score += score;
			}
			for (p=0; p<4; p++) {
				if (game.players[p].bid.sbid.spec == SPADES_NIL) {
					int score = (game.players[p].tricks == 0 ? GSPADES.nil_value : -GSPADES.nil_value);
					ggz_debug("Player %d earned %d for going nil.", (int)p, score);
					game.players[p].score += score;
					game.players[(p+2)%4].score += score;
				}
			}
			break;
		case GGZ_GAME_HEARTS:
			for (p=0; p<game.num_players; p++) {
				int points = GHEARTS.points_on_hand[p];
				int score = (points == 26 ? -26 : points);
				/* if you take all 26 points you "shoot the moon" and earn -26 instead.
				 * TODO: option of giving everyone else 26. */
				game.players[p].score += score;
			}
			break;
		default:
			ggz_debug("SERVER not implemented: game_end_hand for game %d.", game.which_game);
			break;
	}                                                         	
}

/* returns a boolean */
static int game_valid_game(int which_game)
{
	switch (which_game) {
		case GGZ_GAME_SUARO:
			return (game.num_players == 2);
		case GGZ_GAME_ROOK:
		case GGZ_GAME_EUCHRE:
			return 0;	/* not yet supported */
		case GGZ_GAME_BRIDGE:
		case GGZ_GAME_LAPOCHA:
		case GGZ_GAME_SPADES:
			return (game.num_players == 4);
		case GGZ_GAME_HEARTS:
			return (game.num_players > 2 && game.num_players <= 7);	/* 3-7 players */
		default:
			ggz_debug("SERVER BUG: game_valid_gams: unknown game %d.", which_game);
			return 0;
	}
}


