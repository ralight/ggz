/* 
 * File: common.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: Game-independent game functions
 * $Id: common.c 4091 2002-04-27 21:09:21Z jdorje $
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "net_common.h"

#include "ai.h"
#include "bid.h"
#include "common.h"
#include "message.h"
#include "net.h"
#include "options.h"

#ifdef USE_GGZ_STATS		/* defined in common.h */
# include "../../ggzdmod/ggz_stats.h"
#endif /* USE_GGZ_STATS */


/* Global game variables */
game_t game = { 0 };

/* FIXME:
 *
 * This function is a temporary measure so that reserved seats behave
 * correctly.  This works as desired...the problem is that all other
 * games will need identical changes too.
 */
bool seats_full(void)
{
	return ggzdmod_count_seats(game.ggz, GGZ_SEAT_OPEN)
		+ ggzdmod_count_seats(game.ggz, GGZ_SEAT_RESERVED) == 0;
}

const char *get_state_name(server_state_t state)
{
	switch (state) {
	case STATE_WAIT_FOR_BID:
		return "WAIT_FOR_BID";
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

/* Handle message from player */
void handle_player_event(GGZdMod * ggz, GGZdModEvent event, void *data)
{
	player_t p = *(int *) data;
	net_read_player_data(p);
}

/* Setup game state and board.  Also initializes the _type_ of game. */
void init_ggzcards(GGZdMod * ggz, game_data_t *game_data)
{
	/* Seed the random number generator */
	srandom((unsigned) time(NULL));

	/* TODO: we should manually initialize pointers to NULL */
	memset(&game, 0, sizeof(game_t));

	/* Note: the game type need not have been initialized by here;
	   game_data can be NULL and we'll query for the game later */
	game.data = game_data;

	game.state = STATE_PRELAUNCH;
	game.ggz = ggz;

	ggzdmod_log(game.ggz, "Game initialized as %s.",
		    game_data ? game_data->name : "");
}

/* Tries to start a game, requesting information from players where
   necessary.  returns TRUE on successful start. */
bool try_to_start_game(void)
{
	player_t p;
	bool ready = TRUE;

	assert(are_options_set());

	for (p = 0; p < game.num_players; p++)
		if (!game.players[p].ready) {
			/* we could send another REQ_NEWGAME as a reminder,
			   but there would be no way for the client to know
			   that it was a duplicate. */
			ggzdmod_log(game.ggz, "Player %d/%s is not ready.", p,
				    get_player_name(p));
			ready = FALSE;
		}

	if (ready)
		newgame();
	return ready;
}

/* start a new game! */
static void newgame(void)
{
	player_t p;

	assert(game.data);

	finalize_options();

	/* should be entered only when we're ready for a new game */
	for (p = 0; p < game.num_players; p++)
		game.players[p].ready = 0;
	if (!game.initted)
		init_game();
		
	init_cumulative_scores();
	set_global_message("", "%s", "");
	
	game.data->start_game();
	
	for (p = 0; p < game.num_players; p++)
		set_player_message(p);
	assert(get_cardset_type() != UNKNOWN_CARDSET);
	net_broadcast_newgame();
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
			net_send_newgame_request(p);
		break;
	case STATE_NEXT_HAND:
		ggzdmod_log(game.ggz, "Next play: dealing a new hand.");
		if (game.data->test_for_gameover()) {
			game.data->handle_gameover();
			set_game_state(STATE_NOTPLAYING);
			next_play();	/* start a new game */
			return;
		}
		
		net_broadcast_newhand();

		/* shuffle and deal a hand */
		shuffle_deck(game.deck);
		for (p = 0; p < game.num_players; p++)
			game.players[p].tricks = 0;

		/* init bid list */
		game.bid_rounds = 0;
		game.prev_bid = -1;
		for (p = 0; p < game.num_players; p++)
			memset(game.players[p].allbids, 0,
			       game.max_bid_rounds * sizeof(bid_t));

		ggzdmod_log(game.ggz, "Dealing hand %d.", game.hand_num);
		game.data->deal_hand();
		ggzdmod_log(game.ggz, "Dealt hand successfully.");

		/* Now send the resulting hands to each player */
		for (p = 0; p < game.num_players; p++)
			for (s = 0; s < game.num_seats; s++)
				game.data->send_hand(p, s);

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

		game.data->start_bidding();
		next_play();	/* recursion */
		break;
	case STATE_NEXT_BID:
		ggzdmod_log(game.ggz, "Next play: bid %d/%d - player %d/%s.",
			    game.bid_count, game.bid_total, game.next_bid,
			    get_player_name(game.next_bid));
		game.data->get_bid();
		break;
	case STATE_NEXT_PLAY:
		ggzdmod_log(game.ggz,
			    "Next play: playing %d/%d - player %d/%s.",
			    game.play_count, game.play_total, game.next_play,
			    get_player_name(game.next_play));
		game.data->get_play(game.next_play);
		break;
	case STATE_FIRST_TRICK:
		ggzdmod_log(game.ggz, "Next play: first trick of the hand.");
		set_game_state(STATE_NEXT_TRICK);

		game.trick_total = game.hand_size;
		game.play_total = game.num_players;
		/* we also clear all cards after every trick */
		for (s = 0; s < game.num_seats; s++)
			game.seats[s].table = UNKNOWN_CARD;

		game.data->start_playing();
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
static player_t determine_host(void)
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

/* This handles a launch event, when GGZ connects to us for the first time. */
static void handle_launch_event(void)
{
	player_t p;
	
	ggzdmod_log(game.ggz, "Table launch.");
	
	assert(game.state == STATE_PRELAUNCH);
	
	/* determine number of players. */
	/* ggz seats == players */
	game.num_players = ggzdmod_get_num_seats(game.ggz);	
	game.host = -1;		/* no host since none has joined yet */

	game.players = ggz_malloc(game.num_players * sizeof(*game.players));
	for (p = 0; p < game.num_players; p++) {
		game.players[p].seat = -1;
		game.players[p].team = -1;
		game.players[p].allbids = NULL;
		game.players[p].fd = -1;
#ifdef DEBUG
		game.players[p].err_fd = -1;
#endif
		game.players[p].pid = -1;
	}

	/* we don't yet know the number of seats */

	/* As soon as we know which game we're playing, we should init the
	   game.  But it may have been initted already if the game was
	   specified in the command-line options. */
	if (game.data && !game.initted)
		init_game();

	set_game_state(STATE_NOTPLAYING);
}

/* This handles a "done event", when our state is changed to DONE. */
static void handle_done_event(void)
{
	player_t p;
	
	for (p = 0; p < game.num_players; p++)
		if (get_player_status(p) == GGZ_SEAT_BOT)
			stop_ai(p);

	/* Anything else to shut down? */
}

/* This handles a state change event, when the table changes state. */
void handle_state_event(GGZdMod * ggz, GGZdModEvent event, void *data)
{
	GGZdModState old_state = *(GGZdModState*)data;
	GGZdModState new_state = ggzdmod_get_state(ggz);
	
	if (old_state == GGZDMOD_STATE_CREATED) {
		assert(new_state == GGZDMOD_STATE_WAITING);
		handle_launch_event();
	}
	
	switch (new_state) {
	case GGZDMOD_STATE_CREATED:
		break;
	case GGZDMOD_STATE_DONE:
		/* Close down. */
		handle_done_event();
		break;
	case GGZDMOD_STATE_WAITING:
		/* Enter waiting phase. */
		save_game_state();
		break;
	case GGZDMOD_STATE_PLAYING:
		/* All other changes are done in the join event. */
		assert(seats_full());
		restore_game_state();
		break;
	}

	/* Otherwise do nothing (yet...) */
}

/* This handles the event of a player joining. */
void handle_join_event(GGZdMod * ggz, GGZdModEvent event, void *data)
{
	player_t player = ((GGZSeat*)data)->num;
	seat_t seat = game.players[player].seat;

	/* There's a big problem here since if the players join/leave before
	   the game type is set, we won't know the seat number of the player
	   - it'll be -1.  We thus have to special-case that entirely. */

	ggzdmod_log(game.ggz,
		    "Handling a join event for player %d (seat %d).", player,
		    seat);

	assert(game.state == STATE_WAITFORPLAYERS);

	/* set the age of the player */
	game.players[player].age = game.player_count;
	game.player_count++;
	game.host = determine_host();

	/* if all seats are occupied, we restore the former state and
	   continue playing (below).  The state is restored up here so that
	   the sync will be handled correctly. */
	if (seats_full())
		ggzdmod_set_state(ggz, GGZDMOD_STATE_PLAYING);
		
	/* If we're already playing, we should send the client a NEWGAME
	   alert - although it's not really a NEWGAME but a game in
	   progress. */
	if (game.state > STATE_WAITFORPLAYERS)
		net_send_newgame(player);

	/* Send all table info to joiner.  This will also make any new
	   options requests, if necessary. */
	send_sync(player);

	/* We send player list to everyone.  This used to skip over the
	   player joining.  I think it only did that because the player list
	   is also sent out in the sync, but there could be a better reason
	   as well. */
	net_broadcast_player_list();

	if (seat >= 0 &&	/* see above comment about seat==-1 */
	    game.state != STATE_NOTPLAYING &&
	    !(game.state == STATE_WAITFORPLAYERS
	      && game.saved_state == STATE_NOTPLAYING))
		send_player_message_toall(seat);

	if (seats_full()
	    && game.data != NULL) {
		/* (Re)Start game play */
		if (game.state != STATE_WAIT_FOR_BID
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
	GGZSeat player_seat = ggzdmod_get_seat(game.ggz, p);
	player_seat.name = (char *) name;	/* discard const */
	
	ggzdmod_log(game.ggz, "Changing player %d (type %d)'s name to %s.", p,
		    player_seat.type, name);
	if (ggzdmod_set_seat(game.ggz, &player_seat) < 0) {
		ggzdmod_log(game.ggz,
			    "ERROR: SERVER BUG: "
			    "Failed player name change for player %d/%s!",
			    p, name);
		name = NULL;
	}
}

/* This handles the event of a player leaving */
void handle_leave_event(GGZdMod * ggz, GGZdModEvent event, void *data)
{
	player_t player = ((GGZSeat*)data)->num;
	seat_t seat = game.players[player].seat;
	GGZdModState new_state;

	ggzdmod_log(game.ggz,
		    "Handling a leave event for player %d (seat %d).", player,
		    seat);

	/* There's a big problem here since if the players join/leave before
	   the game type is set, we won't know the seat number of the player
	   - it'll be -1.  We thus have to special-case that entirely. */

	/* send new seat data */
	net_broadcast_player_list();

	/* reset player's age; find new host */
	game.players[player].age = -1;
	if (game.host == player) {
		game.host = determine_host();
		if (game.host >= 0)
			set_player_message(game.host);
			
		/* This happens when the host leaves the table before choosing
		   a game.  Now the new host must choose. */
		if (game.data == NULL)
			request_client_gametype();		
	}

	/* get rid of old player message */
	set_player_message(player);
	
	/* Figure out what state to move to. */
	if (ggzdmod_count_seats(game.ggz, GGZ_SEAT_PLAYER) == 0)
		new_state = GGZDMOD_STATE_DONE;
	else
		new_state = GGZDMOD_STATE_WAITING;
		
	/* Change the table (and hence game) state. */
	if (ggzdmod_set_state(game.ggz, new_state) < 0)
		assert(FALSE);
	
	ggzdmod_log(game.ggz, "Player leave successful.");
}

/* This handles the event of a player responding to a newgame request */
void handle_newgame_event(player_t player)
{
	ggzdmod_log(game.ggz, "Handling a newgame event for player %d/%s.",
		    player, get_player_name(player));
	game.players[player].ready = TRUE;
	if (!are_options_set()) {
		if (player == game.host)
			request_client_options();
	} else
		(void) try_to_start_game();
}

/* This handles the event of someone playing a card */
void handle_play_event(player_t p, card_t card)
{
	seat_t s = game.players[p].play_seat;
	int i, still_playing;
	hand_t *hand;
	player_t p2;
	
	assert(game.players[p].is_playing);
	
	/* is this the right place for this? */
	game.players[p].is_playing = FALSE;

	ggzdmod_log(game.ggz, "Handling a play event.");
	/* determine the play */
	hand = &game.seats[s].hand;

	/* send the play */
	net_broadcast_play(s, card);
	
	/* FIXME: what happens if someone sends a card not even in their hand?? */

	/* remove the card from the player's hand by sliding it to the end. */
	/* TODO: this is quite inefficient */
	for (i = 0; i < hand->hand_size; i++)
		if (are_cards_equal(hand->cards[i], card))
			break;
	assert(i < hand->hand_size);
	for (; i < hand->hand_size - 1; i++)
		hand->cards[i] = hand->cards[i + 1];
	hand->hand_size--;
	hand->cards[hand->hand_size] = card;

	/* Move the card onto the table */
	game.seats[s].table = card;
	if (game.next_play == game.leader)
		game.lead_card = card;

	/* do extra handling */
	if (card.suit == game.trump)
		game.trump_broken = TRUE;
	game.data->handle_play(p, s, card);
	
	for (p2 = 0, still_playing = 0; p2 < game.num_players; p2++)
		if (game.players[p2].is_playing)
			still_playing++;

	/* this is the player that just finished playing */
	set_player_message(p);
	
	if (still_playing > 0) {
		assert(game.state == STATE_WAIT_FOR_PLAY);
	}

	/* set up next move */
	game.play_count++;
	game.data->next_play();
	if (game.play_count != game.play_total)
		set_game_state(STATE_NEXT_PLAY);
	else {
		/* end of trick */
		ggzdmod_log(game.ggz, "End of trick; %d/%d.  Scoring it.",
			    game.trick_count, game.trick_total);
		game.data->end_trick();
		send_last_trick();
		handle_trick_event(game.winner);
		game.trick_count++;
		set_game_state(STATE_NEXT_TRICK);
		if (game.trick_count == game.trick_total) {
			/* end of the hand */
			ggzdmod_log(game.ggz, "End of hand number %d.",
				    game.hand_num);
			send_last_hand();
			game.data->end_hand();
			set_all_player_messages();
			update_cumulative_scores();
			game.dealer = (game.dealer + 1) % game.num_players;
			game.hand_num++;
			set_game_state(STATE_NEXT_HAND);
		}
	}

	/* do next move */
	next_play();
}

void handle_badplay_event(player_t p, char *msg)
{
	set_game_state(STATE_WAIT_FOR_PLAY);
	net_send_badplay(p, msg);
	/* Now we'll wait for them to play again. */
}

/* This handles the event of someone making a bid */
void handle_bid_event(player_t p, bid_t bid)
{
	int was_waiting = 0;
	
	net_broadcast_bid(p, bid);

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

	set_game_state(STATE_WAIT_FOR_BID);

	/* determine the bid */
	game.players[p].bid = bid;

	/* handle the bid */
	game.players[p].bid_count++;
	game.data->handle_bid(p, bid);

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
	game.data->next_bid();

	/* This is a minor hack.  The game's next_bid function might have
	   changed the game's state.  If that happened, we don't want to
	   change it back! */
	if (game.state == STATE_WAIT_FOR_BID) {
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
}

void handle_trick_event(player_t winner)
{
	seat_t s;
	
	ggzdmod_log(game.ggz,
		    "Player %d/%s won the trick.",
		    winner, get_player_name(winner));

	for (s = 0; s < game.num_seats; s++)
		/* note: we also clear the table at the beginning of every
		   hand */
		game.seats[s].table = UNKNOWN_CARD;
		
	net_broadcast_trick(winner);
}

void handle_gameover_event(int winner_cnt, player_t * winners)
{
	player_t p;
	ggzdmod_log(game.ggz, "Handling gameover event.");


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

	for (p = 0; p < game.num_players; p++)
		set_player_message(p);	/* some data could have changed */
		
	net_broadcast_gameover(winner_cnt, winners);
}

void handle_neterror_event(player_t p)
{	
	ggzdmod_log(game.ggz, "Network error for player %d.", p);
	if (get_player_status(p) == GGZ_SEAT_BOT) {
		if (game.initted) {
			/* FIXME: AI players aren't spawned until the game is
			   initialized.  But we will stil try to send data to
			   them...a problem. */
			restart_ai(p);
		}
	}
}

void handle_client_language(player_t p, const char* lang)
{
	/* FIXME: store the language and use it to translate messages */
	ggzdmod_log(game.ggz, "Player %d set their language to %s.", p, lang);
}

void handle_client_newgame(player_t p)
{
	if (game.state == STATE_NOTPLAYING) {
		handle_newgame_event(p);
	} else {
		ggzdmod_log(game.ggz,  "ERROR: received RSP_NEWGAME while we were in state %d (%s).",
				   game.state, get_state_name(game.state));
	}
}

/* Send out current game hand, score, etc.  Doesn't use its own protocol, but
   calls on others */
void send_sync(player_t p)
{
	seat_t s;

	ggzdmod_log(game.ggz, "Sending sync to player %d/%s.", p,
		    get_player_name(p));

	net_send_player_list(p);

	/* send out messages again */
	send_all_global_messages(p);
	for (s = 0; s < game.num_seats; s++)
		send_player_message(s, p);

	/* Send out hands */
	if (game.state != STATE_NOTPLAYING)
		for (s = 0; s < game.num_seats; s++)
			game.data->send_hand(p, s);

	/* Send out table cards */
	net_send_table(p);

	/* request bid/play again, if necessary */
	if (game.players[p].bid_data.is_bidding) {
		/* We can't call req_bid, because it has side effects (like
		   changing the game's state). */
		net_send_bid_request(p,
		                     game.players[p].bid_data.bid_count,
		                     game.players[p].bid_data.bids);
	}
	if (game.state == STATE_WAIT_FOR_PLAY && game.players[p].is_playing)
		net_send_play_request(p, game.players[p].play_seat);
		
	if (p == game.host) {
		if (game.data == NULL)
			request_client_gametype();
		else if (game.players[game.host].ready &&
		         !are_options_set())
			request_client_options();
	}
}


void broadcast_sync(void)
{
	player_t p;
	for (p = 0; p < game.num_players; p++)
		send_sync(p);
}

void handle_client_sync(player_t p)
{
	send_sync(p);
}

void send_hand(const player_t p, const seat_t s, int reveal)
{
	/* We used to refuse to send hands of size 0, but some games may need
	   to do this! */
	assert(game.state != STATE_NOTPLAYING);

	/* The open_hands option causes everyone's hand to always be
	   revealed. */
	if (game.open_hands)
		reveal = 1;
		
	net_send_hand(p, s, reveal);
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
	
	assert(game.data != NULL);
	assert(game.data->is_valid_game());
	assert(!game.initted);

	/* default values */
	game.deck_type = GGZ_DECK_FULL;
	game.last_trick = TRUE;
	game.last_hand = TRUE;
	game.cumulative_scores = TRUE;
	game.bid_history = TRUE;

	/* now we do all the game-specific initialization... */
	assert(game.ai_type == NULL);
	game.data->init_game();
	
	for (p = 0; p < game.num_players; p++)
		if (get_player_status(p) == GGZ_SEAT_BOT)
			start_ai(p, game.ai_type);

	game.deck = create_deck(game.deck_type);
	if (game.max_hand_length == 0)
		/* note: problems if hand_length really _is_ 0 */
		game.max_hand_length =
			get_deck_size(game.deck) / game.num_players;

	/* set the game message */
	set_global_message("game", "%s", game.data->full_name);

	set_global_message("Rules",
			   "You can read the rules of this game at\n%s.",
			   game.data->rules_url);

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

	game.initted = TRUE;
}


void assign_seat(seat_t s, player_t p)
{
	game.seats[s].player = p;
	game.players[p].seat = s;
}

void empty_seat(seat_t s, char *name)
{
	game.seats[s].player = -1;
	game.seats[s].name = name;
}


const char *get_seat_name(seat_t s)
{
	switch (get_seat_status(s)) {
	case GGZ_SEAT_PLAYER:
	case GGZ_SEAT_BOT:
	case GGZ_SEAT_RESERVED:
		return ggzdmod_get_seat(game.ggz, game.seats[s].player).name;
		break;
	case GGZ_SEAT_OPEN:
		return "Empty Seat"; /* Not really necessary. */
		break;
	case GGZ_SEAT_NONE:
		return game.seats[s].name;
		break;
	}
	
	return "Unknown Seat";
}

GGZSeatType get_seat_status(seat_t s)
{
	if (game.seats[s].player >= 0)
		return get_player_status(game.seats[s].player);
	else
		return GGZ_SEAT_NONE;
}


const char* get_player_name(player_t p)
{
	return ggzdmod_get_seat(game.ggz, p).name;	
}

GGZSeatType get_player_status(player_t p)
{
	return ggzdmod_get_seat(game.ggz, p).type;
}

int get_player_socket(int p)
{
	GGZSeat seat = ggzdmod_get_seat(game.ggz, p);
	int fd;
	
	switch (seat.type) {
	case GGZ_SEAT_PLAYER:
		fd = seat.fd;
		break;
	case GGZ_SEAT_BOT:
		fd = game.players[p].fd;
		break;
	default:
		fd = -1;
		break;
	}
	return fd;
}

/* libggz should handle this instead! */
void fatal_error(const char *msg)
{
	if (ggzdmod_log(game.ggz, "ERROR: %s", msg))
		fprintf(stderr, "Error: %s", msg);	/* What else can we
							   do? */
	abort();
}
