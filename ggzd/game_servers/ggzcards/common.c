/* 
 * File: common.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: Game-independent game functions
 * $Id: common.c 3356 2002-02-14 09:38:48Z jdorje $
 *
 * This file contains code that controls the flow of a general
 * trick-taking game.  Game states, event handling, etc. are all
 * handled within this code.  For specialty handling we call a
 * game-specific function from game.c or games/<game>.c.
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
#  include <config.h>			/* Site-specific config */
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "common.h"
#ifdef USE_GGZ_STATS		/* defined in common.h */
# include "../../ggzdmod/ggz_stats.h"
#endif /* USE_GGZ_STATS */


/* Global game variables */
struct game_t game = { 0 };

/* FIXME:
 *
 * This function is a temporary measure so that reserved seats behave
 * correctly.  This works as desired...the problem is that all other
 * games will need identical changes too.
 */
static int seats_full(void)
{
	return ggzdmod_count_seats(game.ggz, GGZ_SEAT_OPEN)
		+ ggzdmod_count_seats(game.ggz, GGZ_SEAT_RESERVED) == 0;
}

static const char *get_state_name(server_state_t state)
{
	switch (state) {
	case STATE_NONE:
		return "NONE";
	case STATE_PRELAUNCH:
		return "PRELAUNCH";
	case STATE_NOTPLAYING:
		return "NOTPLAYING";
	case STATE_WAITFORPLAYERS:
		return "WAITFORPLAYERS";
	case STATE_NEXT_HAND:
		return "NEXT_HAND";
	case STATE_FIRST_BID:
		return "FIRST_BID";
	case STATE_NEXT_BID:
		return "NEXT_BID";
	case STATE_FIRST_TRICK:
		return "FIRST_TRICK";
	case STATE_NEXT_TRICK:
		return "NEXT_TRICK";
	case STATE_NEXT_PLAY:
		return "NEXT_PLAY";
	case STATE_WAIT_FOR_PLAY:
		return "WAIT_FOR_PLAY";
	}
	return "[unknown state]";
}

static int try_to_start_game(void);
static void newgame(void);
static int determine_host(void);
static void set_player_name(player_t p, const char *name);

/* Changes the state of the game, printing a debugging message. */
void set_game_state(server_state_t state)
{
	if (game.state == STATE_WAITFORPLAYERS) {
		/* sometimes an event can happen that changes the state while
		   we're waiting for players, for instance a player finishing
		   their bid even though someone's left the game.  In this
		   case we wish to advance to the next game state while
		   continuing to wait for players.  However, this should be
		   handled separately.  Here we just allow for it by restoring 
		   the state if a set_game_state is called while we're in
		   waiting mode. */
		if (game.saved_state != state)
			ggzdmod_log(game.ggz, "ERROR: SERVER BUG: "
				    "Setting game saved state to %d - %s.",
				    state, get_state_name(state));
		game.saved_state = state;
	} else {
		if (game.state != state)
			ggzdmod_log(game.ggz,
				    "Setting game state to %d - %s.", state,
				    get_state_name(state));
		game.state = state;
	}
}

/* Saves the state of the game and enters waiting mode.  We wait when we
   don't have enough players which can happen at pretty much any time. */
void save_game_state()
{
	if (game.state == STATE_WAITFORPLAYERS)
		return;
	ggzdmod_log(game.ggz,
		    "Entering waiting state; old state was %d - %s.",
		    game.state, get_state_name(game.state));
	game.saved_state = game.state;
	game.state = STATE_WAITFORPLAYERS;
}

/* Restore the state of the game once we're ready to leave waiting mode. */
void restore_game_state()
{
	ggzdmod_log(game.ggz, "Ending waiting state; new state is %d - %s.",
		    game.saved_state, get_state_name(game.saved_state));
	game.state = game.saved_state;
}

static char *player_messages[] =
	{ "RSP_NEWGAME", "RSP_OPTIONS", "RSP_PLAY", "RSP_BID",
	"REQ_SYNC"
};

/* Handle message from player */
void handle_player_event(GGZdMod * ggz, GGZdModEvent event, void *data)
{
	player_t p = *(int *) data;
	int fd, op, status = 0;
	bid_t bid;

	fd = get_player_socket(p);

	if (read_opcode(fd, &op) < 0)
		return;

	if (op >= 0 && op <= REQ_SYNC)
		ggzdmod_log(game.ggz, "Received %d (%s) from player %d/%s.",
			    op, player_messages[op], p, get_player_name(p));
	else
		ggzdmod_log(game.ggz,
			    "Received unknown message %d from player %d/%s.",
			    op, p, get_player_name(p));

	switch (op) {
	case RSP_NEWGAME:
		if (game.state == STATE_NOTPLAYING) {
			status = 0;
			handle_newgame_event(p);
		} else {
			ggzdmod_log(game.ggz,
				    "SERVER/CLIENT bug?: received RSP_NEWGAME while we were in state %d (%s).",
				    game.state, get_state_name(game.state));
			status = -1;
		}
		break;
	case RSP_OPTIONS:
		if (p != game.host) {
			/* how could this happen? */
			ggzdmod_log(game.ggz,
				    "SERVER/CLIENT bug: received options from non-host player.");
			status = -1;
		} else {
			if (game.which_game == GGZ_GAME_UNKNOWN) {
				int option;
				rec_options(1, &option);
				games_handle_gametype(option);

				init_game();
				(void) send_sync_all();

				if (seats_full())
					next_play();
			} else {
				handle_options();
				try_to_start_game();
			}
		}
		break;
	case RSP_BID:
		if ((status = rec_bid(p, &bid)) == 0)
			handle_bid_event(p, bid);
		break;
	case RSP_PLAY:
		status = rec_play(p);
		break;
	case REQ_SYNC:
		status = send_sync(p);
		break;
	default:
		/* Unrecognized opcode */
		ggzdmod_log(game.ggz, "SERVER/CLIENT BUG: "
			    "game_handle_player: unrecognized opcode %d.",
			    op);
		status = -1;
		break;
	}

	if (status != 0)
		ggzdmod_log(game.ggz,
			    "ERROR: handle_player: status is %d on message from player %d/%s.",
			    status, p, get_player_name(p));
}

/* Setup game state and board.  Also initializes the _type_ of game. */
void init_ggzcards(GGZdMod * ggz, int which)
{
	/* Seed the random number generator */
	srandom((unsigned) time(NULL));

	/* TODO: we should manually initialize pointers to NULL */
	memset(&game, 0, sizeof(struct game_t));

	/* JDS: Note: the game type must have been initialized by here */
	game.which_game = which;

	game.state = STATE_PRELAUNCH;
	game.ggz = ggz;

	ggzdmod_log(game.ggz, "Game initialized as game %d.",
		    game.which_game);
}

/* Tries to start a game, requesting information from players where
   necessary.  returns 1 on successful start. */
static int try_to_start_game(void)
{
	player_t p;
	int ready = 1;

	for (p = 0; p < game.num_players; p++)
		if (!game.players[p].ready
		    && get_player_status(p) != GGZ_SEAT_BOT) {
			/* we could send another REQ_NEWGAME as a reminder,
			   but there would be no way for the client to know
			   that it was a duplicate. */
			ggzdmod_log(game.ggz, "Player %d/%s is not ready.", p,
				    get_player_name(p));
			ready = 0;
		}
	if (ready && options_set()) {
		newgame();
		return 1;
	} else
		return 0;
}

/* start a new game! */
static void newgame(void)
{
	player_t p;

	if (game.which_game == GGZ_GAME_UNKNOWN)
		fatal_error("BUG: newgame(): unknown game.");

	finalize_options();

	/* should be entered only when we're ready for a new game */
	for (p = 0; p < game.num_players; p++)
		game.players[p].ready = 0;
	if (!game.initted)
		init_game();
	game.funcs->start_game();
	init_cumulative_scores();
	for (p = 0; p < game.num_players; p++)
		set_player_message(p);
	send_newgame_all();
	game.dealer = random() % game.num_players;
	set_game_state(STATE_NEXT_HAND);

	ggzdmod_log(game.ggz, "Game start completed successfully.");

	next_play();
}

/* Do the next play */
void next_play(void)
{
	player_t p;
	seat_t s;
	/* TODO: split this up into functions */
	/* TODO: use looping instead of recursion */

	ggzdmod_log(game.ggz, "Next play called while state is %s.",
		    get_state_name(game.state));

	switch (game.state) {
	case STATE_NOTPLAYING:
		ggzdmod_log(game.ggz, "Next play: trying to start a game.");
		for (p = 0; p < game.num_players; p++)
			game.players[p].ready = 0;
		for (p = 0; p < game.num_players; p++)
			if (get_player_status(p) != GGZ_SEAT_BOT)
				(void) send_newgame_request(p);
		break;
	case STATE_NEXT_HAND:
		ggzdmod_log(game.ggz, "Next play: dealing a new hand.");
		if (game.funcs->test_for_gameover()) {
			game.funcs->handle_gameover();
			set_game_state(STATE_NOTPLAYING);
			next_play();	/* start a new game */
			return;
		}

		/* shuffle and deal a hand */
		cards_shuffle_deck(game.deck);
		for (p = 0; p < game.num_players; p++)
			game.players[p].tricks = 0;

		/* init bid list */
		game.bid_rounds = 0;
		game.prev_bid = -1;
		for (p = 0; p < game.num_players; p++)
			memset(game.players[p].allbids, 0,
			       game.max_bid_rounds * sizeof(bid_t));

		ggzdmod_log(game.ggz, "Dealing hand %d.", game.hand_num);
		if (game.funcs->deal_hand() < 0)
			return;
		ggzdmod_log(game.ggz, "Dealt hand successfully.");

		/* Now send the resulting hands to each player */
		for (p = 0; p < game.num_players; p++)
			for (s = 0; s < game.num_seats; s++)
				if (game.funcs->send_hand(p, s) < 0)
					ggzdmod_log(game.ggz,
						    "Error: game_send_hand returned -1.");

		set_game_state(STATE_FIRST_BID);
		ggzdmod_log(game.ggz,
			    "Done generating the next hand; entering bidding phase.");
		next_play();	/* recursion */
		break;
	case STATE_FIRST_BID:
		ggzdmod_log(game.ggz, "Next play: starting the bidding.");
		set_game_state(STATE_NEXT_BID);

		for (p = 0; p < game.num_players; p++) {
			game.players[p].bid.bid = 0;
			game.players[p].bid_count = 0;
			set_player_message(p);
		}
		game.bid_count = 0;

		ai_start_hand();
		game.funcs->start_bidding();
		next_play();	/* recursion */
		break;
	case STATE_NEXT_BID:
		ggzdmod_log(game.ggz, "Next play: bid %d/%d - player %d/%s.",
			    game.bid_count, game.bid_total, game.next_bid,
			    get_player_name(game.next_bid));
		game.funcs->get_bid();
		break;
	case STATE_NEXT_PLAY:
		ggzdmod_log(game.ggz,
			    "Next play: playing %d/%d - player %d/%s.",
			    game.play_count, game.play_total, game.next_play,
			    get_player_name(game.next_play));
		game.funcs->get_play(game.next_play);
		break;
	case STATE_FIRST_TRICK:
		ggzdmod_log(game.ggz, "Next play: first trick of the hand.");
		set_game_state(STATE_NEXT_TRICK);

		game.trick_total = game.hand_size;
		game.play_total = game.num_players;
		/* we also clear all cards after every trick */
		for (s = 0; s < game.num_seats; s++)
			game.seats[s].table = UNKNOWN_CARD;

		game.funcs->start_playing();
		set_all_player_messages();
		game.trump_broken = 0;
		game.next_play = game.leader;
		game.play_count = game.trick_count = 0;
		next_play();
		break;
	case STATE_NEXT_TRICK:
		ggzdmod_log(game.ggz,
			    "Next play: next trick %d/%d - leader is %d/%s.",
			    game.trick_count, game.trick_total, game.leader,
			    get_player_name(game.leader));
		game.play_count = 0;
		game.next_play = game.leader;
		set_game_state(STATE_NEXT_PLAY);
		next_play();	/* minor recursion */
		break;
	default:
		ggzdmod_log(game.ggz, "ERROR: SERVER BUG: "
			    "game_play called with unknown state: %d",
			    game.state);
		break;
	}

	return;
}

/* The oldest player becomes the host.  The oldest player is the one with the
   youngest "age". */
static int determine_host(void)
{
	player_t p, host = -1;
	int age = -1;
	for (p = 0; p < game.num_players; p++)
		if (get_player_status(p) == GGZ_SEAT_PLAYER)
			if (game.players[p].age >= 0)
				if (age == -1 || game.players[p].age < age) {
					host = p;
					age = game.players[p].age;
				}
	return host;
}

/* This handles a launch event, when ggz connects to our server for the first
   time. */
void handle_launch_event(GGZdMod * ggz, GGZdModEvent event, void *data)
{
	player_t p;

	ggzdmod_log(game.ggz, "Handling a launch event.");
	if (game.state != STATE_PRELAUNCH) {
		ggzdmod_log(game.ggz, "ERROR: "
			    "state wasn't prelaunch when handling a launch.");
		return;
	}

	/* determine number of players. */
	game.num_players = ggzdmod_get_num_seats(game.ggz);	/* ggz seats
								   == players 
								 */
	game.host = -1;		/* no host since none has joined yet */

	game.players = ggz_malloc(game.num_players * sizeof(*game.players));
	for (p = 0; p < game.num_players; p++) {
		game.players[p].seat = -1;
		game.players[p].allbids = NULL;
	}

	/* we don't yet know the number of seats */

	/* as soon as we know which game we're playing, we should init the
	   game */
	if (game.which_game != GGZ_GAME_UNKNOWN)
		init_game();

	set_game_state(STATE_NOTPLAYING);
	save_game_state();	/* no players are connected yet, so we enter
				   waiting phase */
	return;
}

/* This handles the event of a player joining. */
void handle_join_event(GGZdMod * ggz, GGZdModEvent event, void *data)
{
	player_t player = *(int *) data;
	seat_t seat = game.players[player].seat;

	/* There's a big problem here since if the players join/leave before
	   the game type is set, we won't know the seat number of the player
	   - it'll be -1.  We thus have to special-case that entirely. */

	ggzdmod_log(game.ggz,
		    "Handling a join event for player %d (seat %d).", player,
		    seat);

	if (game.state != STATE_WAITFORPLAYERS) {
		ggzdmod_log(game.ggz, "ERROR: SERVER BUG: "
			    "someone joined while we weren't waiting.");
		return;
	}

	/* get player's name */
	if (seat >= 0) {	/* see above comment about seat==-1 */
		game.seats[seat].name = get_player_name(player);
	}

	/* set the age of the player */
	game.players[player].age = game.player_count;
	game.player_count++;
	game.host = determine_host();

	/* if all seats are occupied, we restore the former state and
	   continue playing (below).  The state is restored up here so that
	   the sync will be handled correctly. */
	if (seats_full())
		restore_game_state();
		
	/* If we're already playing, we should send the client a NEWGAME
	   alert - although it's not really a NEWGAME but a game in
	   progress. */
	if (game.state > STATE_WAITFORPLAYERS)
		send_newgame(player);

	/* send all table info to joiner */
	(void) send_sync(player);

	/* We send player list to everyone.  This used to skip over the
	   player joining.  I think it only did that because the player list
	   is also sent out in the sync, but there could be a better reason
	   as well. */
	(void) broadcast_player_list();

	/* should this be in sync??? */
	if (seat >= 0 &&	/* see above comment about seat==-1 */
	    game.state != STATE_NOTPLAYING &&
	    !(game.state == STATE_WAITFORPLAYERS
	      && game.saved_state == STATE_NOTPLAYING))
		send_player_message_toall(seat);

	if (player == game.host && game.which_game == GGZ_GAME_UNKNOWN)
		games_req_gametype();

	if (seats_full()
	    && game.which_game != GGZ_GAME_UNKNOWN) {
		/* (Re)Start game play */
		if (game.state != STATE_NONE
		    && game.state != STATE_WAIT_FOR_PLAY)
			/* if we're in one of these two states, we have to
			   wait for a response anyway */
			next_play();
	}
	
	ggzdmod_log(game.ggz, "Player join successful.");
}

/* This sets not only the player name, but the name for the player's seat if
   applicable. */
static void set_player_name(player_t p, const char *name)
{
	int s = game.players[p].seat;
	GGZSeat player_seat = ggzdmod_get_seat(game.ggz, p);
	player_seat.name = (char *) name;	/* discard const */
	ggzdmod_log(game.ggz, "Changing player %d (type %d)'s name to %s.", p,
		    player_seat.type, name);
	if (ggzdmod_set_seat(game.ggz, &player_seat) >= 0) {
		/* s might be -1 if seats aren't assigned yet. */
		if (s >= 0)
			game.seats[s].name = name;
	} else {
		ggzdmod_log(game.ggz,
			    "ERROR: SERVER BUG: "
			    "Failed player name change for player %d/%s!",
			    p, name);
		game.seats[s].name = NULL;
	}
}

/* This handles the event of a player leaving */
void handle_leave_event(GGZdMod * ggz, GGZdModEvent event, void *data)
{
	player_t player = *(int *) data;
	seat_t seat = game.players[player].seat;

	ggzdmod_log(game.ggz,
		    "Handling a leave event for player %d (seat %d).", player,
		    seat);

	/* There's a big problem here since if the players join/leave before
	   the game type is set, we won't know the seat number of the player
	   - it'll be -1.  We thus have to special-case that entirely. */

	/* seat name */
	set_player_name(player, "Empty Seat");

	/* send new seat data */
	(void) broadcast_player_list();

	/* reset player's age; find new host */
	game.players[player].age = -1;
	game.host = determine_host();
	if (game.host >= 0)
		set_player_message(game.host);

	/* get rid of old player message */
	set_player_message(player);

	/* save old state and enter waiting phase */
	assert(!seats_full());
	save_game_state();
	
	ggzdmod_log(game.ggz, "Player leave successful.");
}

/* This handles the event of a player responding to a newgame request */
int handle_newgame_event(player_t player)
{
	ggzdmod_log(game.ggz, "Handling a newgame event for player %d/%s.",
		    player, get_player_name(player));
	game.players[player].ready = 1;
	if (player == game.host && !options_set())
		get_options();
	try_to_start_game();
	return 0;
}

/* This handles the event of someone playing a card */
int handle_play_event(card_t card)
{
	int i;
	hand_t *hand;

	ggzdmod_log(game.ggz, "Handling a play event.");
	/* determine the play */
	hand = &game.seats[game.play_seat].hand;

	/* send the play */
	(void) send_play(card, game.play_seat);

	/* remove the card from the player's hand by sliding it to the end. */
	/* TODO: this is quite inefficient */
	for (i = 0; i < hand->hand_size; i++)
		if (cards_equal(hand->cards[i], card))
			break;
	for (; i < hand->hand_size; i++)
		hand->cards[i] = hand->cards[i + 1];
	hand->cards[hand->hand_size - 1] = card;
	hand->hand_size--;

	/* Move the card onto the table */
	game.seats[game.play_seat].table = card;
	if (game.next_play == game.leader)
		game.lead_card = card;

	/* do extra handling */
	if (card.suit == game.trump)
		game.trump_broken = 1;
	ai_alert_play(game.next_play, card);
	game.funcs->handle_play(card);

	/* set up next move */
	game.play_count++;
	game.funcs->next_play();
	if (game.play_count != game.play_total)
		set_game_state(STATE_NEXT_PLAY);
	else {
		/* end of trick */
		ggzdmod_log(game.ggz, "End of trick; %d/%d.  Scoring it.",
			    game.trick_count, game.trick_total);
		sleep(1);
		game.funcs->end_trick();
		send_last_trick();
		(void) send_trick(game.winner);
		game.trick_count++;
		set_game_state(STATE_NEXT_TRICK);
		if (game.trick_count == game.trick_total) {
			/* end of the hand */
			ggzdmod_log(game.ggz, "End of hand number %d.",
				    game.hand_num);
			send_last_hand();
			sleep(1);
			game.funcs->end_hand();
			set_all_player_messages();
			update_cumulative_scores();
			game.dealer = (game.dealer + 1) % game.num_players;
			game.hand_num++;
			set_game_state(STATE_NEXT_HAND);
		}
	}

	/* this is the player that just finished playing */
	set_player_message(game.curr_play);

	/* do next move */
	next_play();
	return 0;
}

/* This handles the event of someone making a bid */
int handle_bid_event(player_t p, bid_t bid)
{
	int was_waiting = 0;

	game.players[p].bid_data.is_bidding = 0;
	clear_bids(p);

	ggzdmod_log(game.ggz, "Handling a bid event.");
	if (game.state == STATE_WAITFORPLAYERS) {
		/* if a player left while another player was in the middle of
		   bidding, this can happen.  The solution is to temporarily
		   return to playing, handle the bid, and then (below) return
		   to waiting. */
		restore_game_state();
		was_waiting = 1;
	}

	set_game_state(STATE_NONE);

	/* determine the bid */
	game.players[p].bid = bid;

	/* handle the bid */
	ai_alert_bid(p, bid);
	game.players[p].bid_count++;
	game.funcs->handle_bid(p, bid);

	set_player_message(p);

	/* add the bid to the "bid list" */
	/* FIXME: this needs work!!! */
	if (p <= game.prev_bid)
		game.bid_rounds++;
	if (game.bid_rounds >= game.max_bid_rounds) {
		player_t p2;
		game.max_bid_rounds += 10;
		for (p2 = 0; p2 < game.num_players; p2++) {
			game.players[p2].allbids =
				ggz_realloc(game.players[p2].allbids,
					    game.max_bid_rounds *
					    sizeof(bid_t));
			memset(&game.players[p2].
			       allbids[game.max_bid_rounds - 10], 0,
			       10 * sizeof(bid_t));
		}
	}
	game.players[p].allbids[game.bid_rounds] = bid;
	send_bid_history();

	/* set up next move */
	game.bid_count++;
	game.funcs->next_bid();

	/* This is a minor hack.  The game's next_bid function might have
	   changed the game's state.  If that happened, we don't want to
	   change it back! */
	if (game.state == STATE_NONE) {
		if (game.bid_count == game.bid_total)
			set_game_state(STATE_FIRST_TRICK);
		else
			set_game_state(STATE_NEXT_BID);
	}

	game.prev_bid = p;

	if (was_waiting)
		save_game_state();
	else
		/* do next move */
		next_play();
	return 0;
}

void set_num_seats(int num_seats)
{
	seat_t s;
	ggzdmod_log(game.ggz, "Setting number of seats to %d.", num_seats);
	game.num_seats = num_seats;
	if (game.seats != NULL)
		ggz_free(game.seats);
	game.seats = ggz_malloc(game.num_seats * sizeof(*game.seats));
	for (s = 0; s < game.num_seats; s++) {
		game.seats[s].player = -1;
		game.seats[s].table = UNKNOWN_CARD;
		game.seats[s].name = "Unclaimed Seat";	/* TODO: reserved
							   seats */
		game.seats[s].pmessage = NULL;
		game.seats[s].hand.cards = NULL;
	}
}

/* Initialize a new game type, calling game_init_game for the game-dependant
   parts */
void init_game()
{
	seat_t s;
	player_t p;

	if (!games_valid_game(game.which_game))
		fatal_error("BUG: init_game: invalid game chosen.");

	if (game.initted || game.which_game == GGZ_GAME_UNKNOWN) {
		ggzdmod_log(game.ggz, "ERROR: SERVER BUG: "
			    "game_init_game called on unknown or previously initialized game.");
		return;
	}

	game.funcs = game_data[game.which_game].funcs;

	/* default values */
	game.deck_type = GGZ_DECK_FULL;
	game.last_trick = 1;
	game.last_hand = 1;
	game.cumulative_scores = 1;
	game.bid_history = 1;
	game.name = game_data[game.which_game].full_name;

	/* now we do all the game-specific initialization... */
	game.funcs->init_game();

	game.deck = cards_create_deck(game.deck_type);
	if (game.max_hand_length == 0)
		/* note: problems if hand_length really _is_ 0 */
		game.max_hand_length =
			cards_deck_size(game.deck) / game.num_players;

	/* set the game message */
	if (game.name == NULL)
		game.name = "Unknown Game";
	set_global_message("game", "%s", game.name);

	/* set the Rules message */
	if (game.rules_url == NULL)
		/* Could we just refer people to ggz.sf.net? */
		game.rules_url = "http://pagat.com/";

	set_global_message("Rules",
			   "You can read the rules of this game at\n%s.",
			   game.rules_url);

	/* allocate hands */
	for (s = 0; s < game.num_seats; s++) {
		game.seats[s].hand.cards =
			ggz_malloc(game.max_hand_length * sizeof(card_t));
	}

	set_global_message("", "%s", "");
	/* This is no longer necessary under the put_player_message system
	   for (s = 0; s < game.num_seats; s++) game.seats[s].message[0] = 0; 
	 */

	/* set AI names */
	for (p = 0; p < game.num_players; p++)
		if (get_player_status(p) == GGZ_SEAT_BOT)
			set_player_name(p, ai_get_name(p));

	game.initted = 1;
}


void assign_seat(seat_t s, player_t p)
{
	game.seats[s].player = p;
	game.players[p].seat = s;

	/* set up name for seat */
	game.seats[s].name = get_player_name(p);
}

void empty_seat(seat_t s, char *name)
{
	game.seats[s].player = -1;
	game.seats[s].name = name;
}


const char *get_seat_name(seat_t s)
{
	if (game.seats[s].name)
		return game.seats[s].name;
	else {
		switch (get_seat_status(s)) {
		case GGZ_SEAT_PLAYER:
			return "[Player]";
		case GGZ_SEAT_BOT:
			return "[Bot]";
		case GGZ_SEAT_OPEN:
			return "[Empty Seat]";
		case GGZ_SEAT_RESERVED:
			return "[Reserved]";
		default:
			return "[Unknown]";	
		}
	}
}

GGZSeatType get_seat_status(seat_t s)
{
	if (game.seats[s].player >= 0)
		return get_player_status(game.seats[s].player);
	else
		return GGZ_SEAT_NONE;
}

/* libggz should handle this instead! */
void fatal_error(const char *msg)
{
	if (ggzdmod_log(game.ggz, "ERROR: %s", msg))
		fprintf(stderr, "Error: %s", msg);	/* What else can we
							   do? */
	abort();
}
