/*
 * File: game.c
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Date: 09/17/2000
 * Desc: Combat server functions
 * $Id: game.c 6900 2005-01-25 09:04:11Z jdorje $
 *
 * Copyright (C) 2000 Ismael Orenstein.
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

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <ggz.h>

#include "game.h"
#include "combat.h"

#define SET(OPTION) (cbt_game.options & OPTION)
#define SIG(NUMBER) (NUMBER >= 0 ? 1 : -1)

// Global game variables
combat_game cbt_game;

// Who is the host?
static int host = -1;

// How many have done setup?
static int done_setup = 0;

// Odds of winning in random attack
static int odd[9][2] =
    { {30, 70}, {35, 74}, {41, 78}, {49, 82}, {56, 85}, {64, 88}, {71, 91},
    {77, 92}, {84, 95} };

void game_init(GGZdMod * ggz)
{
	struct timeval tv;

	cbt_game.ggz = ggz;
	cbt_game.map = NULL;
	cbt_game.width = 0;
	cbt_game.height = 0;
	cbt_game.state = CBT_STATE_INIT;
	cbt_game.turn = 0;
	cbt_game.options = 0;
	cbt_game.name = NULL;
	// Intializes the random number generator w/ the current time
	gettimeofday(&tv, NULL);
	ggzdmod_log(cbt_game.ggz,
		    "Random number generator initialized w/ %d",
		    (int)tv.tv_usec);
	srandom((int)tv.tv_usec);
}

static void game_handle_launch(void)
{
	if (cbt_game.state == CBT_STATE_INIT) {
		cbt_game.state = CBT_STATE_WAIT;
		ggzdmod_log(cbt_game.ggz, "Waiting for players");
	}
	// Now we know how many players we have!
	cbt_game.number = ggzdmod_get_num_seats(cbt_game.ggz);
}

/* This handles a "state" event from GGZ. */
void game_handle_ggz_state(GGZdMod * ggz, GGZdModEvent event,
			   const void *data)
{
	const GGZdModState *old_state = data;

	if (*old_state == GGZDMOD_STATE_CREATED)
		game_handle_launch();

	/* State changes between waiting/playing/done are handled by us
	   (the game), and we do them before we call for the state change
	   rather than here (which is bad, but...). */
}

static int seats_full(void)
{
	return ggzdmod_count_seats(cbt_game.ggz, GGZ_SEAT_OPEN)
	    + ggzdmod_count_seats(cbt_game.ggz, GGZ_SEAT_RESERVED) == 0;
}

/* This handles a player "join" event from GGZ. */
void game_handle_ggz_join(GGZdMod * ggz, GGZdModEvent event,
			  const void *data)
{
	const GGZSeat *old_seat = data;
	const int seat = old_seat->num;

	game_send_seat(seat);
	game_send_players();
	if (cbt_game.map)	// Options already setup
		game_send_options(seat);
	ggzdmod_log(cbt_game.ggz, "Joining player %d at state %d", seat,
		    cbt_game.state);
	switch (cbt_game.state) {
	case CBT_STATE_WAIT:
		if (host < 0) {
			// First player!
			host = seat;
			game_request_options(host);
		}
		if (cbt_game.map && seats_full()) {
			// Request setup!
			cbt_game.state = CBT_STATE_SETUP;
			game_request_setup(-1);
			if (SET(OPT_RANDOM_SETUP))
				game_start();
		}
		break;
	case CBT_STATE_SETUP:
		game_request_setup(seat);
		break;
	case CBT_STATE_PLAYING:
		game_send_sync(seat, 0);
		game_start();
		break;
	case CBT_STATE_DONE:
		game_send_sync(seat, 1);
		// Ugly hack, but why to create another variable?
		game_send_gameover(done_setup);
		break;
	}
}

/* This handles a player "leave" event from GGZ. */
void game_handle_ggz_leave(GGZdMod * ggz, GGZdModEvent event,
			   const void *data)
{
	/* User left */
	const GGZSeat *old_seat = data;
	int seat = old_seat->num, a, done = 0;

	ggzdmod_log(cbt_game.ggz, "Leaving player %d at state %d", seat,
		    cbt_game.state);
	game_send_players();
	switch (cbt_game.state) {
	case CBT_STATE_WAIT:
		if (host == seat) {
			for (a = 0;
			     a < ggzdmod_get_num_seats(cbt_game.ggz);
			     a++) {
				if (ggzdmod_get_seat(cbt_game.ggz, a).fd >=
				    0) {
					host = a;
					break;
				}
			}
			game_request_options(host);
		}
		break;
	case CBT_STATE_SETUP:
		for (a = 0; a < cbt_game.width * cbt_game.height; a++) {
			if (GET_OWNER(cbt_game.map[a].unit) == seat
			    && LAST(cbt_game.map[a].unit) != U_EMPTY) {
				cbt_game.map[a].unit = U_EMPTY;
				done = 1;
			}
		}
		if (done)
			done_setup--;
		break;
	}
}

void game_handle_player_data(GGZdMod * ggz, GGZdModEvent event,
			     const void *data)
{
	const int *seat_ptr = data;
	const int seat = *seat_ptr;
	int fd, op, a, status = CBT_SERVER_OK, b;

	fd = ggzdmod_get_seat(cbt_game.ggz, seat).fd;

	if (ggz_read_int(fd, &op) < 0)
		return;	/* FIXME: handle error --JDS */

	ggzdmod_log(cbt_game.ggz, "Got message %d from player %d\n", op,
		    seat);

	switch (op) {

	case CBT_MSG_OPTIONS:
		if ((b = game_get_options(seat)) == 0) {
			// Sends options to everyone
			for (a = 0; a < cbt_game.number; a++) {
				if (ggzdmod_get_seat(cbt_game.ggz, a).fd >=
				    0)
					game_send_options(a);
			}
			// Check if must start the game
			if (seats_full() && cbt_game.map) {
				cbt_game.state = CBT_STATE_SETUP;
				game_request_setup(-1);
				if (SET(OPT_RANDOM_SETUP))
					game_start();
			}
			// b == -2 means that a player tried to cheat
			// just ignore him
		} else if (b != -2) {
			// Free memory
			if (cbt_game.map)
				free(cbt_game.map);
			for (a = 0; a < cbt_game.number; a++) {
				if (cbt_game.army && cbt_game.army[a])
					free(cbt_game.army[a]);
			}
			if (cbt_game.army)
				free(cbt_game.army);
			// Init them again
			game_init(cbt_game.ggz);
			// We are at the WAIT turn
			cbt_game.state = CBT_STATE_WAIT;
			game_request_options(host);
		}
		break;

	case CBT_MSG_SETUP:
		// Get what the player sent
		// (if it is invalid, request if for him again)
		if (!game_get_setup(seat))
			game_request_setup(seat);
		break;

	case CBT_REQ_MOVE:
		// Get the player move
		// Handle it and send a answer
		a = game_get_move(seat);
		if (a < 0)
			game_send_move_error(seat, a);
		else
			cbt_game.turn =
			    NEXT(cbt_game.turn, cbt_game.number);
		// Check if the game is over
		a = game_check_over();
		if (a <= 0)
			game_send_gameover(-a);
		break;

	case CBT_REQ_SYNC:
		game_send_sync(seat, 0);
		break;

	case CBT_REQ_OPTIONS:
		game_send_options(seat);
		break;

	default:
		status = CBT_SERVER_ERROR;
		break;

	}

	/* FIXME: handle errors --JDS */
}

void game_send_sync(int seat, int cheated)
{
	int fd = ggzdmod_get_seat(cbt_game.ggz, seat).fd;
	char *syncstr;
	int a, len;

	ggzdmod_log(cbt_game.ggz, "Sending sync to player %d (%d)", seat,
		    cheated);

	if (fd < 0)
		return;

	len = 1 + 1 + 1 + cbt_game.width * cbt_game.height;

	syncstr = (char *)malloc(sizeof(char) * len);

	syncstr[0] = cbt_game.turn;
	syncstr[1] = cbt_game.state;

	for (a = 0; a < cbt_game.width * cbt_game.height; a++) {
		// If I own it, or it is cheated send all the info to me
		if (cheated || GET_OWNER(cbt_game.map[a].unit) == seat)
			syncstr[a + 2] = cbt_game.map[a].unit;
		else if (LAST(cbt_game.map[a].unit) != U_EMPTY)
			// It is a unit, although not mine
			syncstr[a + 2] =
			    FIRST(cbt_game.map[a].unit) + U_UNKNOWN;
		else
			// It is not a unit
			syncstr[a + 2] = U_EMPTY;
	}

	syncstr[len - 1] = 0;

	ggzdmod_log(cbt_game.ggz, "Sync string player %d", seat);
	for (a = 0; a < cbt_game.width * cbt_game.height + 2; a++)
		ggzdmod_log(cbt_game.ggz, "%d", syncstr[a]);

	// Increase one of each character
	for (a = 0; a < len - 1; a++)
		syncstr[a]++;

	// Send the string
	if (ggz_write_int(fd, CBT_MSG_SYNC)
	    || ggz_write_string(fd, syncstr) < 0) {
		ggzdmod_log(cbt_game.ggz, "Can't send sync string");
		return;
	}

	return;
}

void game_send_seat(int seat)
{
	int fd = ggzdmod_get_seat(cbt_game.ggz, seat).fd;

	if (ggz_write_int(fd, CBT_MSG_SEAT) < 0
	    || ggz_write_int(fd, seat) < 0
	    || ggz_write_int(fd, cbt_game.number) < 0
	    || ggz_write_int(fd, PROTOCOL_VERSION) < 0) {
		ggzdmod_log(cbt_game.ggz, "Couldn't send seat!\n");
		return;
	}

	return;
}

void game_request_options(int seat)
{
	int fd = ggzdmod_get_seat(cbt_game.ggz, seat).fd;

	ggzdmod_log(cbt_game.ggz, "Asking player %d for the options",
		    seat);
	if (ggz_write_int(fd, CBT_REQ_OPTIONS) < 0)
		return;

	return;
}

int game_get_options(int seat)
{
	int fd = ggzdmod_get_seat(cbt_game.ggz, seat).fd;
	char *optstr = NULL;
	int a;
	int error;

	ggzdmod_log(cbt_game.ggz, "Getting options");

	if (ggz_read_string_alloc(fd, &optstr) < 0)
		return -1;

	// Check if this player should send the options
	if (seat != host)
		return -2;

	if (*optstr == 0) {
		ggzdmod_log(cbt_game.ggz, "changing host");
		for (a = 0; a < ggzdmod_get_num_seats(cbt_game.ggz); a++) {
			if (ggzdmod_get_seat(cbt_game.ggz, a).fd >= 0
			    && a != seat) {
				host = a;
				break;
			}
		}
		if (a == ggzdmod_get_num_seats(cbt_game.ggz)) {
			host = seat;
		}
		return -1;
	}

	error = combat_options_string_read(optstr, &cbt_game);

	ggzdmod_log(cbt_game.ggz, "Wrong options: %d\n", error);

	return combat_options_check(&cbt_game);
}

void game_send_options(int seat)
{
	int fd = ggzdmod_get_seat(cbt_game.ggz, seat).fd;
	char *opt;

	if (fd < 0) {
		ggzdmod_log(cbt_game.ggz, "This fd doesn't exists!");
		return;
	}

	opt = combat_options_string_write(&cbt_game, 0);
	if (ggz_write_int(fd, CBT_MSG_OPTIONS) < 0
	    || ggz_write_string(fd, opt) < 0) {
		free(opt);
		return;
	}
	free(opt);
}

void game_send_players(void)
{
	int i, j, fd;

	ggzdmod_log(cbt_game.ggz, "Sending player list");

	for (j = 0; j < cbt_game.number; j++) {
		if ((fd = ggzdmod_get_seat(cbt_game.ggz, j).fd) <= -1) {
			continue;
		}

		if (ggz_write_int(fd, CBT_MSG_PLAYERS) < 0) {
			ggzdmod_log(cbt_game.ggz,
				    "Can't send player list!\n");
			return;
		}

		for (i = 0; i < cbt_game.number; i++) {
			GGZSeat seat = ggzdmod_get_seat(cbt_game.ggz, i);
			if (ggz_write_int(fd, seat.type) < 0)
				return;
			if (seat.type != GGZ_SEAT_OPEN &&
			    /* FIXME: seat.name can be NULL! */
			    ggz_write_string(fd, seat.name) < 0) {
				ggzdmod_log(cbt_game.ggz,
					    "Can't send player name!\n");
				return;
			}
		}
	}

	return;
}

void game_request_setup(int seat)
{
	int a, fd;
	if (SET(OPT_RANDOM_SETUP))
		return;
	if (seat < 0) {
		for (a = 0; a < cbt_game.number; a++) {
			fd = ggzdmod_get_seat(cbt_game.ggz, a).fd;
			if (fd < 0)
				continue;
			if (ggz_write_int(fd, CBT_REQ_SETUP) < 0)
				ggzdmod_log(cbt_game.ggz,
					    "Can't send request for setup to player %d",
					    a);
		}
	} else {
		fd = ggzdmod_get_seat(cbt_game.ggz, seat).fd;
		if (fd >= 0) {
			if (ggz_write_int(fd, CBT_REQ_SETUP) < 0)
				ggzdmod_log(cbt_game.ggz,
					    "Can't send request for setup to player %d",
					    seat);
		}
	}
	return;
}

int game_get_setup(int seat)
{
	int a, b = 0;
	int used[12];
	int msg_size;
	int fd = ggzdmod_get_seat(cbt_game.ggz, seat).fd;
	char *setup;

	if (ggz_read_string_alloc(fd, &setup) < 0)
		return 0;

	// Reduce one from the string
	msg_size = strlen(setup);
	for (b = 0; b < msg_size; b++)
		setup[b]--;
	b = 0;


	// Check if he has already sent a setup
	for (a = 0; a < cbt_game.width * cbt_game.height; a++) {
		if (GET_OWNER(cbt_game.map[a].unit) == seat
		    && LAST(cbt_game.map[a].unit) != U_EMPTY) {
			ggzdmod_log(cbt_game.ggz,
				    "Setup error: Already sent setup");
			return 0;
		}
	}

	// Check if player used the correct number of units
	for (a = 0; a < 12; a++)
		used[a] = 0;

	for (a = 0; a < msg_size; a++)
		used[LAST(setup[a])]++;

	for (a = 0; a < 12; a++) {
		if (used[a] != cbt_game.army[cbt_game.number][a])
			return 0;
	}

	// Update game data
	for (a = 0; a < cbt_game.width * cbt_game.height; a++) {
		if (GET_OWNER(cbt_game.map[a].type) == seat) {
			cbt_game.map[a].unit = setup[b];
			b++;
		}
	}

	done_setup++;

	if (done_setup == cbt_game.number)
		game_start();

	return 1;
}

void game_start(void)
{
	int a, fd;

	if (SET(OPT_RANDOM_SETUP) && cbt_game.state != CBT_STATE_PLAYING) {
		for (a = 0; a < cbt_game.number; a++) {
			ggzdmod_log(cbt_game.ggz,
				    "Doing random setup for %d", a);
			combat_random_setup(&cbt_game, a);
		}
		// Setup everything !
		// Now send it
		for (a = 0; a < cbt_game.number; a++)
			game_send_sync(a, SET(OPT_OPEN_MAP));
	}

	for (a = 0; a < cbt_game.number; a++) {
		fd = ggzdmod_get_seat(cbt_game.ggz, a).fd;
		if (fd < 0)
			continue;
		if (SET(OPT_OPEN_MAP))
			game_send_sync(a, 1);
		if (ggz_write_int(fd, CBT_MSG_START) < 0)
			ggzdmod_log(cbt_game.ggz,
				    "Can't send start message to player %d",
				    a);
	}

	cbt_game.state = CBT_STATE_PLAYING;

}

void game_send_move_error(int seat, int error)
{
	int fd, a;

	for (a = 0; a < cbt_game.number; a++) {
		fd = ggzdmod_get_seat(cbt_game.ggz, a).fd;
		if (fd < 0)
			continue;
		if (ggz_write_int(fd, CBT_MSG_MOVE) < 0 ||
		    ggz_write_int(fd, error) < 0 ||
		    ggz_write_int(fd, seat) < 0)
			ggzdmod_log(cbt_game.ggz,
				    "Can`t send error message to player %d",
				    a);
	}
}

int game_get_move(int seat)
{
	int from, to;
	int fd = ggzdmod_get_seat(cbt_game.ggz, seat).fd;
	int a;

	if (ggz_read_int(fd, &from) < 0 || ggz_read_int(fd, &to))
		return CBT_ERROR_SOCKET;

	a = combat_check_move(&cbt_game, from, to);

	// Now checks if its a attack or a move
	if (a == CBT_CHECK_MOVE)
		return game_handle_move(seat, from, to);

	if (a == CBT_CHECK_ATTACK)
		return game_handle_attack(seat, from, to, 0);

	if (a == CBT_CHECK_ATTACK_RUSH)
		return game_handle_attack(seat, from, to, 1);

	// An error ocurred
	return a;
}

int game_handle_move(int seat, int from, int to)
{
	int fd, a;

	// Makes the move!
	cbt_game.map[to].unit =
	    OWNER(seat) + LAST(cbt_game.map[from].unit);
	cbt_game.map[from].unit = U_EMPTY;

	// Ok... now send it!
	for (a = 0; a < cbt_game.number; a++) {
		fd = ggzdmod_get_seat(cbt_game.ggz, a).fd;
		if (fd < 0)
			continue;
		if (ggz_write_int(fd, CBT_MSG_MOVE) < 0 ||
		    ggz_write_int(fd, from) < 0 ||
		    ggz_write_int(fd, to) < 0)
			return CBT_ERROR_SOCKET;
	}

	return 1;
}


int game_handle_attack(int f_s, int from, int to, int is_rushing)
{
	int fd, a, t_u, f_u, t_s;

	// Gets variables
	f_u = LAST(cbt_game.map[from].unit);
	t_u = LAST(cbt_game.map[to].unit);
	t_s = GET_OWNER(cbt_game.map[to].unit);

	switch (t_u) {
	case U_FLAG:
		// The f_u always wins
		f_u *= -1;
		break;
	case U_BOMB:
		// The f_u is a Miner?
		if (f_u == U_MINER)
			f_u *= -1;
		else {
			// The bomb wins!
			t_u *= -1;
			// But we can use the variant that
			// makes the f_u also wins
			if (SET(OPT_ONE_TIME_BOMB))
				f_u *= -1;
		}
		break;
	case U_SPY:
	case U_SCOUT:
	case U_MINER:
	case U_SERGEANT:
	case U_LIEUTENANT:
	case U_CAPTAIN:
	case U_MAJOR:
	case U_COLONEL:
	case U_GENERAL:
		if (SET(OPT_TERRORIST_SPY) && f_u == U_SPY) {
			// Both dies
			f_u *= -1;
			t_u *= -1;
			break;
		}
		// Random attack?
		if (SET(OPT_RANDOM_OUTCOME) && f_u != U_SPY
		    && t_u != U_SPY) {
			a = RANDOM(100);
			ggzdmod_log(cbt_game.ggz, "Random: %d\n", a);
			if (f_u >= t_u) {
				// Attacker is better then defender
				if (a <= odd[f_u - t_u][0]) {
					// Attacker won!
					f_u *= -1;
				} else if (a <= odd[f_u - t_u][1]) {
					// Both died
					f_u *= -1;
					t_u *= -1;
				} else {
					// Defender won!
					t_u *= -1;
				}
			} else {
				// Defender is better!
				if (a <= odd[t_u - f_u][0]) {
					// Defender won!
					t_u *= -1;
				} else if (a <= odd[t_u - f_u][1]) {
					// Both died
					f_u *= -1;
					t_u *= -1;
				} else {
					// Attacker won!
					f_u *= -1;
				}
			}
			break;
		}
		// Normal attack
		if (f_u > t_u)
			f_u *= -1;
		else if (t_u > f_u)
			t_u *= -1;
		else if (t_u == f_u) {
			t_u *= -1;
			f_u *= -1;
		}
		break;
	case U_MARSHALL:
		if (SET(OPT_RANDOM_OUTCOME) && f_u != U_SPY) {
			a = RANDOM(100);
			ggzdmod_log(cbt_game.ggz, "Random: %d\n", a);
			if (a <= odd[t_u - f_u][0]) {
				// Marshall won!
				t_u *= -1;
			} else if (a <= odd[t_u - f_u][1]) {
				// Both died
				f_u *= -1;
				t_u *= -1;
			} else {
				// Attacker won!
				f_u *= -1;
			}
			break;
		}
		if (f_u == U_SPY) {
			if (SET(OPT_TERRORIST_SPY))
				t_u *= -1;
			f_u *= -1;
		} else if (f_u == U_MARSHALL) {
			f_u *= -1;
			t_u *= -1;
		} else
			t_u *= -1;
		break;
	default:
		return CBT_ERROR_CRAZY;
	}

	// If the f_u is rushing, then it always dies!
	if (is_rushing)
		t_u = -1 * abs(t_u);

	// Now do the tough work
	if (f_u < 0 && t_u >= 0) {
		// The from unit won!
		cbt_game.map[from].unit = U_EMPTY;
		cbt_game.map[to].unit = OWNER(f_s) - f_u;
		cbt_game.army[t_s][t_u]--;
	} else if (f_u >= 0 && t_u < 0) {
		// The to unit won!
		cbt_game.map[from].unit = U_EMPTY;
		cbt_game.map[to].unit = OWNER(t_s) - t_u;
		cbt_game.army[f_s][f_u]--;
	} else if (f_u < 0 && t_u < 0) {
		// Both won (or lost, whatever!)
		cbt_game.map[from].unit = U_EMPTY;
		cbt_game.map[to].unit = U_EMPTY;
		cbt_game.army[f_s][-f_u]--;
		cbt_game.army[t_s][-t_u]--;
	} else {
		ggzdmod_log(cbt_game.ggz, "Problems in the attack logic!");
		return CBT_ERROR_CRAZY;
	}


	// Send messages
	for (a = 0; a < cbt_game.number; a++) {
		fd = ggzdmod_get_seat(cbt_game.ggz, a).fd;
		if (fd < 0)
			continue;
		// Write part that everyone knows
		ggz_write_int(fd, CBT_MSG_ATTACK);
		ggz_write_int(fd, from);
		// f_u
		// Send UNKNOWN only if
		// f_s is != a (ie, i'm not the owner of this unit)
		// Unknown victor is set and f_u is a victor
		// Silent offense is set
		if (f_s != a
		    && (SET(OPT_SILENT_OFFENSE)
			|| (SET(OPT_UNKNOWN_VICTOR) && f_u < 0)))
			ggz_write_int(fd, U_UNKNOWN * (SIG(f_u)));
		else
			ggz_write_int(fd, f_u);
		// Write public part
		ggz_write_int(fd, to);
		// t_u
		// Send Unknown only if
		// t_s is != a (ie, i'm not the owner of this unit)
		// Unknown victor is set and t_u < 0
		// Silent defense is set
		if (t_s != a
		    && (SET(OPT_SILENT_DEFENSE)
			|| (SET(OPT_UNKNOWN_VICTOR) && t_u < 0)))
			ggz_write_int(fd, U_UNKNOWN * (SIG(t_u)));
		else
			ggz_write_int(fd, t_u);
	}

	return 1;
}

int game_check_over(void)
{
	int *alive, a, b, no_alives = 0;

	// TODO: Check if the player has no way to move his units

	alive = (int *)malloc(cbt_game.number * sizeof(int));

	for (b = 0; b < cbt_game.number; b++) {
		// Sees if the player has flags left
		if (cbt_game.army[b][U_FLAG] <= 0) {
			alive[b] = 0;
			continue;
		}
		// Checks if the player has no moving units
		alive[b] = 0;
		for (a = U_SPY; a < 12; a++) {
			if (cbt_game.army[b][a] > 0)
				alive[b] = 1;
		}
	}

	// Now checks if it is only one alive
	for (a = 0; a < cbt_game.number; a++) {
		if (alive[a]) {
			b = a;
			no_alives++;
		}
	}
	if (no_alives == 1)
		return -b;
	else
		return 1;
}

void game_send_gameover(int winner)
{
	int a, fd;

	for (a = 0; a < cbt_game.number; a++) {
		fd = ggzdmod_get_seat(cbt_game.ggz, a).fd;
		if (fd < 0)
			continue;
		if (ggz_write_int(fd, CBT_MSG_GAMEOVER) < 0 ||
		    ggz_write_int(fd, winner) < 0)
			ggzdmod_log(cbt_game.ggz,
				    "Can't send ending message to player %d",
				    a);
		// Send all the map data
		game_send_sync(a, 1);
	}

	cbt_game.state = CBT_STATE_DONE;
	// Ugly hack, but why to create another variable?
	done_setup = winner;
}
