/* 
 * File: common.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: Game-independent game functions
 * $Id: common.c 2739 2001-11-13 21:39:00Z jdorje $
 *
 * This file contains code that controls the flow of a general
 * trick-taking game.  Game states, event handling, etc. are all
 * handled within this code.  For specialty handling we call a
 * game-specific function from game.c or games/<game>.c.
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

#include <config.h>		/* Site-specific config */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../../ggzdmod/ggz_stats.h"

#include "common.h"

/* Global game variables */
struct wh_game_t game = { 0 };

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

/* Changes the state of the game, printing a debugging message. */
void set_game_state(server_state_t state)
{
	if (game.state == STATE_WAITFORPLAYERS) {
		/* sometimes an event can happen that changes the state while 
		   we're waiting for players, for instance a player finishing 
		   their bid even though someone's left the game.  In this
		   case we wish to advance to the next game state while
		   continuing to wait for players.  However, this should be
		   handled separately.  Here we just allow for it by
		   restoring the state if a set_game_state is called while
		   we're in waiting mode. */
		if (game.saved_state != state)
			ggzd_debug("ERROR: SERVER BUG: "
				   "Setting game saved state to %d - %ws.",
				   state, get_state_name(state));
		game.saved_state = state;
	} else {
		if (game.state != state)
			ggzd_debug("Setting game state to %d - %s.", state,
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
	ggzd_debug("Entering waiting state; old state was %d - %s.",
		   game.state, get_state_name(game.state));
	game.saved_state = game.state;
	game.state = STATE_WAITFORPLAYERS;
}

/* Restore the state of the game once we're ready to leave waiting mode. */
void restore_game_state()
{
	ggzd_debug("Ending waiting state; new state is %d - %s.",
		   game.saved_state, get_state_name(game.saved_state));
	game.state = game.saved_state;
}

static char *player_messages[] =
	{ "WH_RSP_NEWGAME", "WH_RSP_OPTIONS", "WH_RSP_PLAY", "WH_RSP_BID",
	"WH_REQ_SYNC"
};

/* Handle message from player */
void handle_player_event(ggzd_event_t event, void *data)
{
	player_t p = *(int *) data;
	int fd, op, status = 0;
	bid_t bid;

	fd = ggzd_get_player_socket(p);

	if (read_opcode(fd, &op) < 0)
		return;

	if (op >= 0 && op <= WH_REQ_SYNC)
		ggzd_debug("Received %d (%s) from player %d/%s.", op,
			   player_messages[op], p, ggzd_get_player_name(p));
	else
		ggzd_debug
			("Received unknown message %d from player %d/%s.", op,
			 p, ggzd_get_player_name(p));

	switch (op) {
	case WH_RSP_NEWGAME:
		if (game.state == STATE_NOTPLAYING) {
			status = 0;
			handle_newgame_event(p);
		} else {
			ggzd_debug
				("SERVER/CLIENT bug?: received WH_RSP_NEWGAME while we were in state %d (%s).",
				 game.state, get_state_name(game.state));
			status = -1;
		}
		break;
	case WH_RSP_OPTIONS:
		if (p != game.host) {
			/* how could this happen? */
			ggzd_debug
				("SERVER/CLIENT bug: received options from non-host player.");
			status = -1;
		} else {
			if (game.which_game == GGZ_GAME_UNKNOWN) {
				int option;
				rec_options(1, &option);
				games_handle_gametype(option);

				init_game();
				(void) send_sync_all();

				if (!ggzd_seats_open())
					next_play();
			} else {
				handle_options();
				try_to_start_game();
			}
		}
		break;
	case WH_RSP_BID:
		if ((status = rec_bid(p, &bid)) == 0)
			handle_bid_event(p, bid);
		break;
	case WH_RSP_PLAY:
		status = rec_play(p);
		break;
	case WH_REQ_SYNC:
		status = send_sync(p);
		break;
	default:
		/* Unrecognized opcode */
		ggzd_debug("SERVER/CLIENT BUG: "
			   "game_handle_player: unrecognized opcode %d.", op);
		status = -1;
		break;
	}

	if (status != 0)
		ggzd_debug
			("ERROR: handle_player: status is %d on message from player %d/%s.",
			 status, p, ggzd_get_player_name(p));
}

/* Setup game state and board.  Also initializes the _type_ of game. */
void init_ggzcards(int which)
{
	/* Seed the random number generator */
	srandom((unsigned) time(NULL));

	/* TODO: we should manually initialize pointers to NULL */
	memset(&game, 0, sizeof(struct wh_game_t));

	/* JDS: Note: the game type must have been initialized by here */
	game.which_game = which;

	game.state = STATE_PRELAUNCH;

	ggzd_debug("Game initialized as game %d.", game.which_game);
}

/* Tries to start a game, requesting information from players where
   necessary.  returns 1 on successful start. */
static int try_to_start_game(void)
{
	player_t p;
	int ready = 1;

	for (p = 0; p < game.num_players; p++)
		if (!game.players[p].ready
		    && ggzd_get_seat_status(p) != GGZ_SEAT_BOT) {
			/* we could send another REQ_NEWGAME as a reminder,
			   but there would be no way for the client to know
			   that it was a duplicate. */
			ggzd_debug("Player %d/%s is not ready.", p,
				   ggzd_get_player_name(p));
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

	if (game.which_game == GGZ_GAME_UNKNOWN) {
		ggzd_debug("ERROR: SERVER BUG: "
			   "starting newgame() on unknown game.");
		exit(-1);
	}

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
	(void) send_newgame();
	game.dealer = random() % game.num_players;
	set_game_state(STATE_NEXT_HAND);

	ggzd_debug("Game start completed successfully.");

	next_play();
}

/* Do the next play */
void next_play(void)
{
	player_t p;
	seat_t s;
	/* TODO: split this up into functions */
	/* TODO: use looping instead of recursion */

	ggzd_debug("Next play called while state is %s.",
		   get_state_name(game.state));

	switch (game.state) {
	case STATE_NOTPLAYING:
		ggzd_debug("Next play: trying to start a game.");
		for (p = 0; p < game.num_players; p++)
			game.players[p].ready = 0;
		for (p = 0; p < game.num_players; p++)
			if (ggzd_get_seat_status(p) != GGZ_SEAT_BOT)
				(void) send_newgame_request(p);
		break;
	case STATE_NEXT_HAND:
		ggzd_debug("Next play: dealing a new hand.");
		if (game.funcs->test_for_gameover()) {
			game.funcs->handle_gameover();
			set_game_state(STATE_NOTPLAYING);
			next_play();	/* start a new game */
			return;
		}

		/* shuffle and deal a hand */
		cards_shuffle_deck();
		for (p = 0; p < game.num_players; p++)
			game.players[p].tricks = 0;

		/* init bid list */
		game.bid_rounds = 0;
		game.prev_bid = -1;
		for (p = 0; p < game.num_players; p++)
			memset(game.players[p].allbids, 0,
			       game.max_bid_rounds * sizeof(bid_t));

		ggzd_debug("Dealing hand %d.", game.hand_num);
		if (game.funcs->deal_hand() < 0)
			return;
		ggzd_debug("Dealt hand successfully.");

		/* Now send the resulting hands to each player */
		for (p = 0; p < game.num_players; p++)
			for (s = 0; s < game.num_seats; s++)
				if (game.funcs->send_hand(p, s) < 0)
					ggzd_debug
						("Error: game_send_hand returned -1.");

		set_game_state(STATE_FIRST_BID);
		ggzd_debug
			("Done generating the next hand; entering bidding phase.");
		next_play();	/* recursion */
		break;
	case STATE_FIRST_BID:
		ggzd_debug("Next play: starting the bidding.");
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
		ggzd_debug("Next play: bid %d/%d - player %d/%s.",
			   game.bid_count, game.bid_total, game.next_bid,
			   ggzd_get_player_name(game.next_bid));
		game.funcs->get_bid();
		break;
	case STATE_NEXT_PLAY:
		ggzd_debug("Next play: playing %d/%d - player %d/%s.",
			   game.play_count, game.play_total,
			   game.next_play,
			   ggzd_get_player_name(game.next_play));
		game.funcs->get_play(game.next_play);
		break;
	case STATE_FIRST_TRICK:
		ggzd_debug("Next play: first trick of the hand.");
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
		ggzd_debug
			("Next play: next trick %d/%d - leader is %d/%s.",
			 game.trick_count, game.trick_total, game.leader,
			 ggzd_get_player_name(game.leader));
		game.play_count = 0;
		game.next_play = game.leader;
		set_game_state(STATE_NEXT_PLAY);
		next_play();	/* minor recursion */
		break;
	default:
		ggzd_debug("ERROR: SERVER BUG: "
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
		if (ggzd_get_seat_status(p) == GGZ_SEAT_PLAYER)
			if (game.players[p].age >= 0)
				if (age == -1 || game.players[p].age < age) {
					host = p;
					age = game.players[p].age;
				}
	return host;
}

/* This handles a launch event, when ggz connects to our server for the first
   time. */
void handle_launch_event(ggzd_event_t event, void *data)
{
	player_t p;

	ggzd_debug("Handling a launch event.");
	if (game.state != STATE_PRELAUNCH) {
		ggzd_debug("ERROR: "
			   "state wasn't prelaunch when handling a launch.");
		return;
	}

	/* determine number of players. */
	game.num_players = ggzd_seats_num();	/* ggz seats == players */
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
void handle_join_event(ggzd_event_t event, void *data)
{
	player_t player = *(int *) data;
	seat_t seat = game.players[player].seat;

	ggzd_debug("Handling a join event for player %d (seat %d).", player,
		   seat);
	if (game.state != STATE_WAITFORPLAYERS) {
		ggzd_debug("ERROR: SERVER BUG: "
			   "someone joined while we weren't waiting.");
		return;
	}

	/* get player's name */
	if (seat >= 0) {
		game.seats[seat].name = ggzd_get_player_name(player);
		if (!game.seats[seat].name)
			game.seats[seat].name = "[unknown]";
	}

	/* set the age of the player */
	game.players[player].age = game.player_count;
	game.player_count++;
	game.host = determine_host();

	/* if all seats are occupied, we restore the former state and
	   continue playing (below).  The state is restored up here so that
	   the sync will be handled correctly. */
	if (!ggzd_seats_open())
		restore_game_state();

	/* send all table info to joiner */
	(void) send_sync(player);

	/* We send player list to everyone.  This used to skip over the
	   player joining.  I think it only did that because the player list
	   is also sent out in the sync, but there could be a better reason
	   as well. */
	(void) broadcast_player_list();

	/* should this be in sync??? */
	if (game.state != STATE_NOTPLAYING &&
	    !(game.state == STATE_WAITFORPLAYERS
	      && game.saved_state == STATE_NOTPLAYING))
		send_player_message_toall(game.players[player].seat);

	if (player == game.host && game.which_game == GGZ_GAME_UNKNOWN)
		games_req_gametype();

	if (!ggzd_seats_open() && game.which_game != GGZ_GAME_UNKNOWN) {
		/* (Re)Start game play */
		if (game.state != STATE_NONE
		    && game.state != STATE_WAIT_FOR_PLAY)
			/* if we're in one of these two states, we have to
			   wait for a response anyway */
			next_play();
	}

	return;
}

/* This handles the event of a player leaving */
void handle_leave_event(ggzd_event_t event, void *data)
{
	player_t player = *(int *) data;
	seat_t seat = game.players[player].seat;
	char *name = "Empty Seat";

	ggzd_debug("Handling a leave event.");

	/* seat name */
	ggzd_set_player_name(player, name);
	game.seats[seat].name = "Empty Seat";

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
	if (ggzd_seats_open() > 0)	/* should be a given... */
		save_game_state();

	return;
}

/* This handles the event of a player responding to a newgame request */
int handle_newgame_event(player_t player)
{
	ggzd_debug("Handling a newgame event for player %d/%s.", player,
		   ggzd_get_player_name(player));
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

	ggzd_debug("Handling a play event.");
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
		ggzd_debug("End of trick; %d/%d.  Scoring it.",
			   game.trick_count, game.trick_total);
		sleep(1);
		game.funcs->end_trick();
		send_last_trick();
		(void) send_trick(game.winner);
		game.trick_count++;
		set_game_state(STATE_NEXT_TRICK);
		if (game.trick_count == game.trick_total) {
			/* end of the hand */
			ggzd_debug("End of hand number %d.", game.hand_num);
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

	ggzd_debug("Handling a bid event.");
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
	ggzd_debug("Setting number of seats to %d.", num_seats);
	game.num_seats = num_seats;
	if (game.seats != NULL)
		ggz_free(game.seats);
	game.seats = ggz_malloc(game.num_seats * sizeof(*game.seats));
	for (s = 0; s < game.num_seats; s++) {
		game.seats[s].player = -1;
		game.seats[s].name = "Unclaimed Seat";	/* TODO: reserved
							   seats */
		game.seats[s].pmessage = NULL;
	}
}

/* Initialize a new game type, calling game_init_game for the game-dependant
   parts */
void init_game()
{
	seat_t s;
	player_t p;

	if (!games_valid_game(game.which_game)) {
		ggzd_debug("ERROR: SERVER BUG: "
			   "game_init_game: invalid game %d chosen.",
			   game.which_game);
		exit(-1);
	}

	if (game.initted || game.which_game == GGZ_GAME_UNKNOWN) {
		ggzd_debug("ERROR: SERVER BUG: "
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

	cards_create_deck(game.deck_type);
	if (game.max_hand_length == 0)
		/* note: problems if hand_length really _is_ 0 */
		game.max_hand_length = cards_deck_size() / game.num_players;

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
		if (ggzd_get_seat_status(p) == GGZ_SEAT_BOT) {
			int s = game.players[p].seat;
			char *name = (char *) ai_get_name(p);
			game.seats[s].name = name;
			ggzd_set_player_name(game.seats[s].player, name);
			ggzd_debug("Setting AI name to %s.", name);
		}

	game.initted = 1;
}


void assign_seat(seat_t s, player_t p)
{
	const char *name;
	game.seats[s].player = p;
	game.players[p].seat = s;

	/* set up name for seat */
	name = ggzd_get_player_name(p);
	if (name)
		game.seats[s].name = name;
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
	else
		return "[null seat name]";
}

ggzd_assign_t get_seat_status(seat_t s)
{
	if (game.seats[s].player >= 0)
		return ggzd_get_seat_status(game.seats[s].player);
	else
		return GGZ_SEAT_NONE;
}
