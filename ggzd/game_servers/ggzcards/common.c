/* 
 * File: common.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: Game-independent game functions
 * $Id: common.c 8454 2006-08-02 01:47:14Z jdorje $
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
#include "score.h"
#include "team.h"


/* Global game variables */
game_t game = {
	rated: TRUE
};

bool seats_full(void)
{
	/* This calculation is a bit inefficient, but that's OK */
	return ggzdmod_count_seats(game.ggz, GGZ_SEAT_OPEN) == 0
	  && ggzdmod_count_seats(game.ggz, GGZ_SEAT_RESERVED) == 0
	  && ggzdmod_count_seats(game.ggz, GGZ_SEAT_ABANDONED) == 0;
}

bool seats_empty(void)
{
	/* This calculation is a bit inefficient, but that's OK */
	return ggzdmod_count_seats(game.ggz, GGZ_SEAT_PLAYER) == 0
		&& ggzdmod_count_spectators(game.ggz) == 0;
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

/* Changes the state of the game, printing a debugging message. */
void set_game_state(server_state_t state)
{
	if (game.state == state)
		return;
		
	ggz_debug(DBG_MISC,
		    "Setting game state to %d - %s.", state,
		    get_state_name(state));
	game.state = state;
}

/* Handle message from player.  */
void handle_ggz_player_data_event(GGZdMod * ggz,
				  GGZdModEvent event, const void *data)
{
	const int *player = data;
	handle_player_data_event(*player);
}

/* Handle message from spectator */
void handle_ggz_spectator_data_event(GGZdMod *ggz,
				     GGZdModEvent event, const void *data)
{
	const int *spectator = data;
	handle_player_data_event(SPECTATOR_TO_PLAYER(*spectator));
}

/* Handle message from player/spectator. */
void handle_player_data_event(player_t p)
{
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

	ggz_debug(DBG_MISC, "Game initialized as %s.",
		    game_data ? game_data->name : "");
}

/* Tries to start a game, requesting information from players where
   necessary.  returns TRUE on successful start. */
bool try_to_start_game(void)
{
	bool ready = TRUE;

	assert(are_options_set());

	players_iterate(p) {
		if (!game.players[p].ready) {
			/* we could send another REQ_NEWGAME as a reminder,
			   but there would be no way for the client to know
			   that it was a duplicate. */
			ggz_debug(DBG_MISC, "Player %d/%s is not ready.", p,
				    get_player_name(p));
			ready = FALSE;
		}
	} players_iterate_end;

	if (ready)
		newgame();
	return ready;
}

/* start a new game! */
static void newgame(void)
{
	assert(game.data);

	finalize_options();

	/* should be entered only when we're ready for a new game */
	players_iterate(p) {
		game.players[p].ready = 0;
	} players_iterate_end;
	if (!game.initted)
		init_game();
		
	init_cumulative_scores();
	set_global_message("", "%s", "");
	
	game.data->start_game();
	
	players_iterate(p) {
		set_player_message(p);
	} players_iterate_end;

	assert(get_cardset_type() != UNKNOWN_CARDSET);
	net_broadcast_newgame();
	game.dealer = random() % game.num_players;
	game.hand_num = 0;
	set_game_state(STATE_NEXT_HAND);

	/* Set the table state to playing - indicating a game is in
	   progress. */
	ggzdmod_set_state(game.ggz, GGZDMOD_STATE_PLAYING);

	ggz_debug(DBG_MISC, "Game start completed successfully.");

	next_move();
}

/* Do the next play, only if all seats are full. */
void next_move(void)
{
	seat_t s;
	/* TODO: split this up into functions */
	/* TODO: use looping instead of recursion */
		
	if (!seats_full())
		return;

	ggz_debug(DBG_MISC, "Next play called while state is %s.",
		    get_state_name(game.state));

	switch (game.state) {
	case STATE_NOTPLAYING:
		ggz_debug(DBG_MISC, "Next play: trying to start a game.");
		players_iterate(p) {
			if (get_player_status(p) == GGZ_SEAT_PLAYER)
				game.players[p].ready = FALSE;
			else {
				assert(get_player_status(p) == GGZ_SEAT_BOT);
				game.players[p].ready = TRUE;
			}
		} players_iterate_end;
		players_iterate(p) {
			if (get_player_status(p) == GGZ_SEAT_PLAYER)
				net_send_newgame_request(p);
		} players_iterate_end;
		break;
	case STATE_NEXT_HAND:
		ggz_debug(DBG_MISC, "Next play: dealing a new hand.");
		if (game.data->test_for_gameover()) {
			game.data->handle_gameover();
			next_move();	/* start a new game */
			return;
		}
		
		net_broadcast_newhand();

		/* shuffle and deal a hand */
		shuffle_deck(game.deck);
		players_iterate(p) {
			game.players[p].tricks = 0;
		} players_iterate_end;

		/* init bid list */
		game.bid_rounds = 0;
		game.prev_bid = -1;
		players_iterate(p) {
			memset(game.players[p].allbids, 0,
			       game.max_bid_rounds * sizeof(bid_t));
		} players_iterate_end;

		ggz_debug(DBG_MISC, "Dealing hand %d.", game.hand_num);
		game.data->deal_hand();
		ggz_debug(DBG_MISC, "Dealt hand successfully.");

		/* Now send the resulting hands to each player */
		allplayers_iterate(p) {
			for (s = 0; s < game.num_seats; s++)
				game.data->send_hand(p, s);
		} allplayers_iterate_end;

		set_game_state(STATE_FIRST_BID);
		ggz_debug(DBG_MISC,
			    "Done generating the next hand; entering bidding phase.");
		next_move();	/* recursion */
		break;
	case STATE_FIRST_BID:
		ggz_debug(DBG_MISC, "Next play: starting the bidding.");
		set_game_state(STATE_NEXT_BID);

		players_iterate(p) {
			game.players[p].bid.bid = 0;
			game.players[p].bid_count = 0;
			set_player_message(p);
		} players_iterate_end;
		game.bid_count = 0;

		game.data->start_bidding();
		next_move();	/* recursion */
		break;
	case STATE_NEXT_BID:
		ggz_debug(DBG_MISC, "Next play: bid %d/%d - player %d/%s.",
			    game.bid_count, game.bid_total, game.next_bid,
			    get_player_name(game.next_bid));
		game.data->get_bid();
		break;
	case STATE_NEXT_PLAY:
		ggz_debug(DBG_MISC,
			    "Next play: playing %d/%d - player %d/%s.",
			    game.play_count, game.play_total, game.next_play,
			    get_player_name(game.next_play));
		game.data->get_play(game.next_play);
		break;
	case STATE_FIRST_TRICK:
		ggz_debug(DBG_MISC, "Next play: first trick of the hand.");
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
		next_move();	/* recursion */
		break;
	case STATE_NEXT_TRICK:
		ggz_debug(DBG_MISC,
			    "Next play: next trick %d/%d - leader is %d/%s.",
			    game.trick_count, game.trick_total, game.leader,
			    get_player_name(game.leader));
		game.play_count = 0;
		game.next_play = game.leader;
		set_game_state(STATE_NEXT_PLAY);
		next_move();	/* recursion */
		break;
	case STATE_PRELAUNCH:
	case STATE_WAIT_FOR_BID:
	case STATE_WAIT_FOR_PLAY:
		assert(FALSE);
		break;
	}
}

/* The oldest player becomes the host.  The oldest player is the one with the
   youngest "age". */
static player_t determine_host(void)
{
	player_t host = -1;
	int age = -1;
	players_iterate(p) {
		if (get_player_status(p) == GGZ_SEAT_PLAYER)
			if (game.players[p].age >= 0)
				if (age == -1 || game.players[p].age < age) {
					host = p;
					age = game.players[p].age;
				}
	} players_iterate_end;
	return host;
}

/* This handles a launch event, when GGZ connects to us for the first time. */
static void handle_launch_event(void)
{
	ggz_debug(DBG_MISC, "Table launch.");
	
	assert(game.state == STATE_PRELAUNCH);
	
	/* determine number of players. */
	/* ggz seats == players */
	game.num_players = ggzdmod_get_num_seats(game.ggz);	
	game.host = -1;		/* no host since none has joined yet */

	set_num_teams(game.num_players);

	game.players = ggz_malloc(game.num_players * sizeof(*game.players));
	players_iterate(p) {
		game.players[p].seat = -1;
		assign_team(p, p);
		game.players[p].allbids = NULL;
		game.players[p].dio = NULL;
#ifdef DEBUG
		game.players[p].err_fd = -1;
#endif
		game.players[p].pid = -1;
	} players_iterate_end;

	game.spectators = NULL;
	game.spectator_count = 0;

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
	players_iterate(p) {
		if (get_player_status(p) == GGZ_SEAT_BOT)
			stop_ai(p);
	} players_iterate_end;

	/* Anything else to shut down? */
}

/* This handles a state change event, when the table changes state. */
void handle_ggz_state_event(GGZdMod * ggz,
			    GGZdModEvent event, const void *data)
{
	const GGZdModState *old_state = data;
	GGZdModState new_state = ggzdmod_get_state(ggz);
	
	if (*old_state == GGZDMOD_STATE_CREATED) {
		assert(new_state == GGZDMOD_STATE_WAITING);
		handle_launch_event();
	}
	
	switch (new_state) {
	case GGZDMOD_STATE_CREATED:
	case GGZDMOD_STATE_WAITING:
	case GGZDMOD_STATE_PLAYING:
		/* Nothing needs to be done.  Other changes may already
		   have been done in the join event. */
		break;
	case GGZDMOD_STATE_DONE:
		/* Close down. */
		handle_done_event();
		break;
	}

	/* Otherwise do nothing (yet...) */
}

/* This monolithic function handles any change to a seat.  It might be
   a player join or a player leave.  Eventually, other types of seat
   changes will be possible (although these are not yet tested).  A lot
   of the work is the same for any kind of seat change, though. */
void handle_ggz_seat_event(GGZdMod *ggz, GGZdModEvent event, const void *data)
{
	const GGZSeat *old_seat = data;
	player_t player = old_seat->num, p;
	/* seat_t seat = game.players[player].seat; */
	GGZSeat new_seat = ggzdmod_get_seat(game.ggz, player);
	bool is_join = (new_seat.type == GGZ_SEAT_PLAYER
	                || new_seat.type == GGZ_SEAT_BOT)
	               && (old_seat->type != new_seat.type
	                   || strcmp(old_seat->name, new_seat.name));
	bool is_leave = (old_seat->type == GGZ_SEAT_PLAYER
			 || old_seat->type == GGZ_SEAT_BOT)
	                && (new_seat.type != old_seat->type
	                    || strcmp(old_seat->name, new_seat.name));
	int new_fd = new_seat.fd, old_fd = old_seat->fd;

	/* There's a big problem here since if the players join/leave before
	   the game type is set, we won't know the seat number of the player
	   - it'll be -1.  We thus have to special-case that entirely. */
	
	ggz_debug(DBG_MISC, "Handling a seat-change event for player %d "
		  "(%d => %d).",
	          player, old_fd, new_fd);

	/* Start or stop any AI on the seat, and create the DIO structure
	   for human players.  This needs to be done first. */
	if (is_leave && old_seat->type == GGZ_SEAT_BOT)
		stop_ai(player);
	{
		GGZDataIO **dio = &game.players[old_seat->num].dio;

		if (*dio) {
			ggz_dio_flush(*dio);
			assert(!ggz_dio_is_write_pending(*dio));
			ggz_dio_free(*dio);
			*dio = NULL;
		}
		if (new_fd != -1) {
			*dio = ggz_dio_new(new_fd);

			/* HACK: To avoid split packets. */
			ggz_dio_set_auto_flush(*dio, TRUE);
		}
	}
	if (is_join && new_seat.type == GGZ_SEAT_BOT)
		start_ai(player, game.ai_module);

	/* We send player list to everyone.  This used to skip over the
	   player joining.  I think it only did that because the player list
	   is also sent out in the sync, but there could be a better reason
	   as well. */
	net_broadcast_player_list();

	if (is_join && new_seat.type == GGZ_SEAT_PLAYER) {
		/* set the age of the player */
		game.players[player].age = game.player_count;
		game.player_count++;
	} else
		game.players[player].age = -1;
		
	/* If we're already playing, we should send the client a NEWGAME
	   alert - although it's not really a NEWGAME but a game in
	   progress. */
	if (is_join && game.state != STATE_NOTPLAYING) {
		assert(game.state != STATE_PRELAUNCH);
		net_send_newgame(player);
	}
	
	if (is_join || is_leave) {
		int new_host = determine_host();
		if (new_host != game.host) {
			game.host = new_host;
			if (IS_REAL_PLAYER(game.host))
				set_player_message(game.host);
			
			/* This happens when the host leaves the table before choosing
			   a game.  Now the new host must choose.  We don't have this
			   problem when choosing game options because the host leaving
			   will delay the start of the game anyway. */
			if (game.data == NULL)
				request_client_gametype();
		}
	}
	
	/* If someone leaves, we must abort the game-starting process. */
	if (is_leave)
		for (p = 0; p < game.num_players; p++)
			game.players[p].ready = FALSE;
	
	/* Figure out what state to move to.  The state is changed up here
	   so that the sync and other actions (below) are handled correctly. */
	if (seats_empty()) {
		/* When all seats are empty the table is finished. */
		ggzdmod_set_state(game.ggz, GGZDMOD_STATE_DONE);
	}

	if (is_join) {
		/* Send all table info to joiner.  This will also make any new
		   options requests, if necessary. */
		send_sync(player);
	}

	/* get rid of old player message.  This used to call
	   send_player_message_toall(); I'm not sure what the difference is. */
	set_player_message(player);

	if (seats_full()
	    && game.data != NULL) {
		/* (Re)Start game play */
		if (game.state != STATE_WAIT_FOR_BID
		    && game.state != STATE_WAIT_FOR_PLAY)
			/* if we're in one of these two states, we have to
			   wait for a response anyway. */
			next_move();
	}
	
	ggz_debug(DBG_MISC, "Seat change successful.");
}

void handle_ggz_spectator_seat_event(GGZdMod *ggz,
				     GGZdModEvent event, const void *data)
{
	const GGZSpectator *old = data;
	const int spectator = old->num;
	const int nspectators = ggzdmod_get_max_num_spectators(ggz);
	player_t player = SPECTATOR_TO_PLAYER(spectator);
	GGZSpectator new = ggzdmod_get_spectator(ggz, spectator);
	const int new_fd = new.fd;
#if 0
	int old_fd = old->fd;
#endif

	assert(spectator < nspectators);

	if (nspectators > game.spectator_count) {
		int i;

		game.spectators = ggz_realloc(game.spectators,
					      nspectators
					      * sizeof(*game.spectators));
		for (i = game.spectator_count; i < nspectators; i++) {
			game.spectators[i].dio = NULL;
		}
		game.spectator_count = nspectators;
	}

	{
		GGZDataIO **dio = &game.spectators[old->num].dio;

		if (*dio) {
			ggz_dio_flush(*dio);
			assert(!ggz_dio_is_write_pending(*dio));
			ggz_dio_free(*dio);
			*dio = NULL;
		}
		if (new_fd != -1) {
			*dio = ggz_dio_new(new_fd);

			/* HACK: To avoid split packets. */
			ggz_dio_set_auto_flush(*dio, TRUE);
		}
	}

	if (new.name) {
		/* The spectator is not yet visible to other players; so
		   his joining does not affect anyone else. */

		ggz_debug(DBG_MISC, "%s joined as spectator on seat %d.",
			  new.name, spectator);

		assert(!old->name && old->fd < 0);
		assert(new.name && new.fd >= 0);

		/* Send newgame alert, if we're already playing. */
		if (game.state != STATE_NOTPLAYING)
			net_send_newgame(player);

		send_sync(player);
	} else {
		ggz_debug(DBG_MISC, "%s left as spectator on seat %d.",
			  old->name, spectator);

		assert(!new.name && new.fd < 0 && old->name);

		if (seats_empty())
			if (ggzdmod_set_state(game.ggz,
					      GGZDMOD_STATE_DONE) < 0)
				assert(FALSE);
	}
}

/* This handles the event of a player responding to a newgame request */
void handle_newgame_event(player_t player)
{
	ggz_debug(DBG_MISC, "Handling a newgame event for player %d/%s.",
		    player, get_player_name(player));
	game.players[player].ready = TRUE;
	if (!are_options_set()) {
		if (game.host >= 0 && player == game.host)
			request_client_options();
	} else
		(void) try_to_start_game();
}

void handle_trick_event(player_t winner)
{
	seat_t s;
	
	ggz_debug(DBG_MISC,
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

	ggz_debug(DBG_MISC, "Handling gameover event.");

	{
		int teams[game.num_players];
		GGZGameResult results[game.num_players];
		int i;

		for (p = 0; p < game.num_players; p++) {
			teams[p] = game.players[p].team;
			results[p] = GGZ_GAME_LOSS;
		}

		for (i = 0; i < winner_cnt; i++)
			results[winners[i]] = GGZ_GAME_WIN;

		ggzdmod_report_game(game.ggz, teams, results, NULL);
	}

	set_game_state(STATE_NOTPLAYING);

	/* Set table state to waiting - indicating there is no game in
	   progress. Note that if there is a game but all players aren't
	   present, the game will wait but that's still the PLAYING
	   state. */
	ggzdmod_set_state(game.ggz, GGZDMOD_STATE_WAITING);

	for (p = 0; p < game.num_players; p++)
		set_player_message(p);	/* some data could have changed */
		
	net_broadcast_gameover(winner_cnt, winners);
}

void handle_neterror_event(player_t p)
{	
	ggz_debug(DBG_MISC, "Network error for player %d.", p);
	if (IS_REAL_PLAYER(p)
	    && get_player_status(p) == GGZ_SEAT_BOT) {
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
	ggz_debug(DBG_MISC, "Player %d set their language to %s.", p, lang);
}

void handle_client_newgame(player_t p)
{
	/* Don't need newgames from spectators or bots. */
	if (get_player_status(p) != GGZ_SEAT_PLAYER)
		return;

	/* Make sure we're waiting for a newgame. */
	if (game.state != STATE_NOTPLAYING || !seats_full())
		return;

	handle_newgame_event(p);
}

/* Send out current game hand, score, etc.  Doesn't use its own protocol, but
   calls on others */
void send_sync(player_t p)
{
	seat_t s;

	ggz_debug(DBG_MISC, "Sending sync to player %d/%s.", p,
		    get_player_name(p));

	net_send_player_list(p);

	/* send out messages again */
	send_all_global_messages(p);
	for (s = 0; s < game.num_seats; s++)
		send_player_message(s, p);

	/* Send out hands */
	if (game.state != STATE_NOTPLAYING) {
		for (s = 0; s < game.num_seats; s++)
			game.data->send_hand(p, s);

		/* Send out table cards */
		net_send_table(p);

		players_iterate(p2) {
			net_send_bid(p, p2, game.players[p2].bid);
		} players_iterate_end;
	}

	/* request bid/play again, if necessary */
	if (IS_REAL_PLAYER(p)
	    && game.players[p].bid_data.is_bidding) {
		/* We can't call req_bid, because it has side effects (like
		   changing the game's state). */
		net_send_bid_request(p,
		                     game.players[p].bid_data.bid_count,
		                     game.players[p].bid_data.bids);
	}
	if (IS_REAL_PLAYER(p)
	    && game.state == STATE_WAIT_FOR_PLAY
	    && game.players[p].is_playing)
		net_send_play_request(p, game.players[p].play_seat);
		
	if (game.host >= 0 && p == game.host) {
		if (game.data == NULL)
			request_client_gametype();
		else if (game.players[game.host].ready &&
		         !are_options_set())
			request_client_options();
	}

	game.data->sync_player(p);
}


void broadcast_sync(void)
{
	allplayers_iterate(p) {
		send_sync(p);
	} allplayers_iterate_end;
}

void handle_client_sync(player_t p)
{
	send_sync(p);
}

void send_hand(const player_t p, const seat_t s,
               bool show_fronts, bool show_backs)
{
	/* We used to refuse to send hands of size 0, but some games may need
	   to do this! */
	if (game.state == STATE_NOTPLAYING) {
		assert(FALSE);
		return;
	}

	/* The open_hands option causes everyone's hand to always be
	   revealed. */
	if (game.open_hands
	    || (p < 0 && !game.blind_spectators))
		show_fronts = show_backs = TRUE;
		
	net_send_hand(p, s, show_fronts, show_backs);
}

void set_num_seats(int num_seats)
{
	seat_t s;
	ggz_debug(DBG_MISC, "Setting number of seats to %d.", num_seats);
	game.num_seats = num_seats;
	if (game.seats != NULL)
		ggz_free(game.seats);
	game.seats = ggz_malloc(game.num_seats * sizeof(*game.seats));
	for (s = 0; s < game.num_seats; s++) {
		game.seats[s].player = -1;
		game.seats[s].table = UNKNOWN_CARD;
		/* TODO: reserved seats */
		game.seats[s].name = ggz_strdup("Unclaimed Seat");
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
	game.rated = TRUE;
	game.deck_type = GGZ_DECK_FULL;
	game.last_trick = TRUE;
	game.last_hand = TRUE;
	game.cumulative_scores = TRUE;
	game.bid_history = TRUE;

	/* now we do all the game-specific initialization... */
	game.ai_module = choose_ai_module();
	game.data->init_game();
	game.data->get_options();
	
	for (p = 0; p < game.num_players; p++)
		if (get_player_status(p) == GGZ_SEAT_BOT)
			start_ai(p, game.ai_module);

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
	player_t p = game.seats[s].player;
	if (IS_REAL_PLAYER(p))
		return get_player_name(p);
	if (game.seats[s].name)
		return game.seats[s].name;
	return "Unknown Seat";
}

GGZSeatType get_seat_status(seat_t s)
{
	if (IS_REAL_PLAYER(game.seats[s].player))
		return get_player_status(game.seats[s].player);
	else
		return GGZ_SEAT_NONE;
}

const char* get_player_name(player_t p)
{
	if (IS_SPECTATOR(p)) {
		int sp = PLAYER_TO_SPECTATOR(p);
		return ggzdmod_get_spectator(game.ggz, sp).name;
	} else {
		GGZSeat seat = ggzdmod_get_seat(game.ggz, p);
		struct game_seat_t *player_seat
		  = &game.seats[game.players[p].seat];
		char name[128];
	
		switch (seat.type) {
		case GGZ_SEAT_OPEN:
			return "Empty Seat";
		case GGZ_SEAT_PLAYER:
			if (seat.name) return seat.name;
			return "Unknown Seat";
		case GGZ_SEAT_RESERVED:
			if (!seat.name) return "Reserved Seat";
			/* HACK: newlines are not allowed by the client.
			   But putting this on one line may be too long. */
			snprintf(name, sizeof(name), "Rsrv: %s",
				 seat.name);
			if (player_seat->name) ggz_free(player_seat->name);
			player_seat->name = ggz_strdup(name);
			return player_seat->name;
		case GGZ_SEAT_ABANDONED:
			if (!seat.name) return "Abandoned Seat";
			/* HACK: newlines are not allowed by the client.
			   But putting this on one line may be too long. */
			snprintf(name, sizeof(name), "Abnd: %s",
				 seat.name);
			if (player_seat->name) ggz_free(player_seat->name);
			player_seat->name = ggz_strdup(name);
			return player_seat->name;
		case GGZ_SEAT_BOT:
			if (!seat.name) return "Bot";
			return seat.name;
		case GGZ_SEAT_NONE:
			break;
		}
	}
	return "Unknown Seat";
}

GGZSeatType get_player_status(player_t p)
{
	return ggzdmod_get_seat(game.ggz, p).type;
}

GGZDataIO *get_player_dio(int p)
{
	if (IS_SPECTATOR(p)) {
		int sp = PLAYER_TO_SPECTATOR(p);

		if (sp >= game.spectator_count) return NULL;
		return game.spectators[sp].dio;
	} else {
		return game.players[p].dio;
	}
}

/* libggz should handle this instead! */
void fatal_error(const char *msg)
{
	ggz_error_msg_exit("%s", msg);
}
