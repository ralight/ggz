/*
 * File: game.c
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Date: 09/17/2000
 * Desc: Combat server functions
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

#include "ggz.h"
#include "game.h"
#include "combat.h"
#include "protocols.h"
#include <easysock.h>

#include <stdlib.h>

// Global game variables
combat_game cbt_game;

// Who is the host?
int host = -1;


void game_init() {
	cbt_game.map = NULL;
	cbt_game.width = 0;
	cbt_game.height = 0;
	cbt_game.state = CBT_STATE_INIT;
	cbt_game.turn = 0;
}
	
int game_handle_ggz(int ggz_fd, int *p_fd) {
	int op, seat, status = CBT_SERVER_ERROR;

	if (es_read_int(ggz_fd, &op) < 0)
		return CBT_SERVER_ERROR;

	switch (op) {
		case REQ_GAME_LAUNCH:
			if (cbt_game.state == CBT_STATE_INIT && ggz_game_launch() == 0) {
				cbt_game.state = CBT_STATE_WAIT;
				ggz_debug("Waiting for players");
			}
			status = CBT_SERVER_OK;
			break;
		case REQ_GAME_JOIN:
			if (cbt_game.state == CBT_STATE_WAIT && ggz_player_join(&seat, p_fd) == 0) {
				game_send_seat(seat);
				ggz_debug("Human seats: %d\n", ggz_seats_human());
				if (host < 0) {
					// First player!
					host = seat;
					game_request_options(seat);
				}
				if (cbt_game.map) // Options already setup
					game_send_options(seat);
				game_send_players();
				if (!ggz_seats_open() && cbt_game.map)
					game_request_setup(-1);
			}
			status = CBT_SERVER_JOIN;
			break;
		case REQ_GAME_LEAVE:
			// User left
			if (ggz_player_leave(&seat, p_fd) == 0) {
				if (cbt_game.state == CBT_STATE_PLAYING || cbt_game.state == CBT_STATE_SETUP)
					cbt_game.state = CBT_STATE_WAIT;
				game_send_players();
			}
			status = CBT_SERVER_LEFT;
			break;
		case RSP_GAME_OVER:
			// This game is over
			status = CBT_SERVER_QUIT;
			break;
		default:
			// Problems!
			status = CBT_SERVER_ERROR;
			ggz_debug("The GGZ server is out of his mind!\n");
			break;
	}
	return status;
}

int game_handle_player(int seat) {
	int fd, op, a, status = CBT_SERVER_OK;
	
	ggz_debug("Got message from player %d\n", seat);

	fd = ggz_seats[seat].fd;

	if (es_read_int(fd, &op) < 0)
		return -1;

	switch (op) {

		case CBT_MSG_OPTIONS:
			game_get_options(seat);
			// Sends options to everyone
			for (a = 0; a < ggz_seats_num(); a++) {
				if (ggz_seats[a].fd >= 0)
					game_send_options(a);
			}
			// Check if must start the game
			if (!ggz_seats_open() && cbt_game.map)
				game_request_setup(-1);
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
				cbt_game.turn = NEXT(cbt_game.turn, ggz_seats_num());
			// Check if the game is over
			a = game_check_over();
			if (a <= 0)
				game_send_gameover(-a);
			break;

		default:
			status = CBT_SERVER_ERROR;
			break;

	}

	return status;

}

void game_send_sync(int seat) {
	// FIXME: Must add this!
	return;
}

void game_send_seat(int seat) {
	int fd = ggz_seats[seat].fd;

	ggz_debug("Sending player %d his seat numnber\n", seat);

	if (es_write_int(fd, CBT_MSG_SEAT) < 0 || es_write_int(fd, seat) < 0 || es_write_int(fd, ggz_seats_num()) < 0 || es_write_int(fd, PROTOCOL_VERSION) < 0) {
		ggz_debug("Couldn't send seat!\n");
		return;
	}

	return;
}

void game_request_options(int seat) {
	int fd = ggz_seats[seat].fd;

	if (es_write_int(fd, CBT_REQ_OPTIONS) < 0)
		return;

	return;
}

int game_get_options(int seat) {
	int fd = ggz_seats[seat].fd;
	char *optstr = NULL;

	ggz_debug("Getting options");

	if (es_read_string_alloc(fd, &optstr) < 0)
		return 0;

	ggz_debug("Len: %d", strlen(optstr));

	// FIXME: Check for validity
	combat_options_string_read(optstr, &cbt_game, ggz_seats_num());

	

	return 1;
}

void game_send_options(int seat) {
	int fd = ggz_seats[seat].fd;

	if (fd < 0) {
		ggz_debug("This fd doesn't exists!");
		return;
	}

	if (es_write_int(fd, CBT_MSG_OPTIONS) < 0 || es_write_string(fd, combat_options_string_write(NULL, &cbt_game)) < 0)
			return;
	
	return;
}

void game_send_players() {
	int i, j, fd;

	for (j = 0; j < ggz_seats_num(); j++) {
		if ( (fd = ggz_seats[j].fd) == -1 ) {
			ggz_debug("Bot seat\n");
			continue;
		}
		ggz_debug("Sending player list to player %d", j);

		if (es_write_int(fd, CBT_MSG_PLAYERS) < 0) {
			ggz_debug("Can't send player list!\n");
			return;
		}
	
		for (i = 0; i < ggz_seats_num(); i++) {
			if (es_write_int(fd, ggz_seats[i].assign) < 0)
				return;
			if (ggz_seats[i].assign != GGZ_SEAT_OPEN && es_write_string(fd, ggz_seats[i].name) < 0) {
				ggz_debug("Can't send player name!\n");
				return;
			}
		}
	}

	return;
}

void game_request_setup(int seat) {
	int a, fd;
	if (seat < 0) {
		for (a = 0; a < ggz_seats_num(); a++) {
			fd = ggz_seats[a].fd;
			if (fd < 0)
				continue;
			if (es_write_int(fd, CBT_REQ_SETUP) < 0)
				ggz_debug("Can't send request for setup to player %d", a);
		}
	} else {
		fd = ggz_seats[seat].fd;
		if (fd >= 0) {
			if (es_write_int(fd, CBT_REQ_SETUP) < 0)
				ggz_debug("Can't send request for setup to player %d", seat);
		}
	}
	return;
}

int game_get_setup(int seat) {
	int a, b = 0;
	static int done_setup = 0;
	int setup_size = 0;
	int msg_size;
	int fd = ggz_seats[seat].fd;
	char *setup;

	if (es_read_string_alloc(fd, &setup) < 0)
		return 0;

	// Reduce one from the string
	msg_size = strlen(setup);
	for (b = 0; b < msg_size; b++)
		setup[b]--;
	b = 0;
	

	// TODO: Check for validity!!
	
	// Check if he has already sent a setup and get the number of setup tiles
	for (a = 0; a < cbt_game.width*cbt_game.height; a++) {
		if (GET_OWNER(cbt_game.map[a].type) == seat)
			setup_size++;
		if (GET_OWNER(cbt_game.map[a].unit) == seat && LAST(cbt_game.map[a].unit) != U_EMPTY) {
			ggz_debug("Setup error: Already sent setup");
			return 0;
		}
	}

	// Check if he sent the right number of tiles
	if (setup_size != msg_size) {
		ggz_debug("Setup error: Wrong message size");
		return 0;
	}

	// Now checks if he hasn't sent less than the expected
	setup_size = 0;
	for (a = 0; a < 12; a++)
		setup_size+=cbt_game.army[seat][a];
	ggz_debug("Setup size: %d", setup_size);
	for (a = 0; a < msg_size; a++) {
		if (LAST(setup[a]) != U_EMPTY)
			setup_size--;
	}
	if (setup_size != 0) {
		ggz_debug("Setup error: setup_size = %d", setup_size);
		return 0;
	}

	for (a = 0; a < cbt_game.width * cbt_game.height; a++) {
		if (GET_OWNER(cbt_game.map[a].type) == seat) {
			cbt_game.map[a].unit = setup[b];
			b++;
		}
	}

	done_setup++;

	if (done_setup == ggz_seats_num())
		game_start();

	return 1;
}

void game_start() {
	int a, fd;

	for (a = 0; a < ggz_seats_num(); a++) {
		fd = ggz_seats[a].fd;
		if (fd <= 0)
			continue;
		if (es_write_int(fd, CBT_MSG_START) < 0)
			ggz_debug("Can't send start message to player %d", a);
	}
}

void game_send_move_error(int seat, int error) {
	int fd, a;

	for (a = 0; a < ggz_seats_num(); a++) {
		fd = ggz_seats[a].fd;
		if (fd <= 0)
			continue;
		if (es_write_int(fd, CBT_MSG_MOVE) < 0 ||
				es_write_int(fd, error) < 0 ||
				es_write_int(fd, seat) < 0)
			ggz_debug("Can`t send error message to player %d", a);
	}
}

int game_get_move(int seat) {
	int from, to;
	int fd = ggz_seats[seat].fd;

	if (es_read_int(fd, &from) < 0 || es_read_int(fd, &to) )
		return CBT_ERROR_SOCKET;

	// Check if they are on the range
	if (from < 0 || from >= cbt_game.width*cbt_game.height || to < 0 || to >= cbt_game.width*cbt_game.height)
		return CBT_ERROR_OUTRANGE;

	ggz_debug("Seat: %d\tOwner: %d\n", seat, GET_OWNER(cbt_game.map[from].unit));
	// Check if the FROM belongs to the current player
	if (GET_OWNER(cbt_game.map[from].unit) != seat) {
		ggz_debug("Owner: %d, Seat: %d", GET_OWNER(cbt_game.map[from].unit), seat);
		return CBT_ERROR_INVALIDMOVE;
	}

	ggz_debug("Turn: %d");
	if (seat != cbt_game.turn)
		return CBT_ERROR_WRONGTURN;

	// Check if the FROM unit is a moving one
	// (!= BOMB, != FLAG)
	if (LAST(cbt_game.map[from].unit) <= U_BOMB)
		return CBT_ERROR_NOTMOVING;

	// Check if the TO doesn't belong to the current player
	if (GET_OWNER(cbt_game.map[to].unit) == seat)
		return CBT_ERROR_SUICIDAL;

	// Check if the TO is empty (not lake || null)
	if (LAST(cbt_game.map[to].type) != T_OPEN)
		return CBT_ERROR_NOTOPEN;

	// Now checks if its a attack or a move
	if (LAST(cbt_game.map[to].unit) == U_EMPTY)
		return game_handle_move(seat, from, to);

	if (GET_OWNER(cbt_game.map[to].unit) != seat && LAST(cbt_game.map[to].unit) != U_EMPTY)
		return game_handle_attack(seat, from, to);

	return CBT_ERROR_CRAZY;
}

int game_handle_move(int seat, int from, int to) {
	int fd, a, x1, x2, y1, y2, dx, dy, dir = 0;

	x1 = X(from, cbt_game.width);
	x2 = X(to, cbt_game.width);
	y1 = Y(from, cbt_game.width);
	y2 = Y(to, cbt_game.width);

	// Check if its not a diagonal one
	if (x1 != x2 && y1 != y2) {
		MOVE_ERROR("diagonal move");
		return CBT_ERROR_INVALIDMOVE;
	}

	dx = x2 - x1;
	dy = y2 - y1;

	// Check if only of distance 1
	if (LAST(cbt_game.map[from].unit) != U_SCOUT && abs(dx + dy) != 1) {
		MOVE_ERROR("distance > 1 and not a scout");
		return CBT_ERROR_INVALIDMOVE;
	} else if (LAST(cbt_game.map[from].unit) == U_SCOUT) {
		// It is a scout!
		// Checks everything between
		if (dx != 0)
			dir = dx/abs(dx);
		if (dy != 0)
			dir = dy/abs(dy) * cbt_game.width;
		for (a = from; a != to; a += dir) {
			if (a != from && (LAST(cbt_game.map[a].type) != T_OPEN ||
					LAST(cbt_game.map[a].unit) != U_EMPTY)) {
				MOVE_ERROR("invalid scout move");
				return CBT_ERROR_INVALIDMOVE;
			}
		}
	}

	// Makes the move!
	cbt_game.map[to].unit = OWNER(seat) + LAST(cbt_game.map[from].unit);
	cbt_game.map[from].unit = U_EMPTY;
	
	// Ok... now send it!
	for (a = 0; a < ggz_seats_num(); a++) {
		fd = ggz_seats[a].fd;
		if (fd <= 0)
			continue;
		if (es_write_int(fd, CBT_MSG_MOVE) < 0 ||
				es_write_int(fd, from) < 0 ||
				es_write_int(fd, to) < 0)
			return CBT_ERROR_SOCKET;
	}

	return 1;
}
	
	
int game_handle_attack(int seat, int from, int to) {
	int fd, a, x1, x2, y1, y2, dx, dy, t_u, f_u, seat2, dir = 0;

	x1 = X(from, cbt_game.width);
	x2 = X(to, cbt_game.width);
	y1 = Y(from, cbt_game.width);
	y2 = Y(to, cbt_game.width);

	// Check if its not a diagonal one
	if (x1 != x2 && y1 != y2) {
		MOVE_ERROR("diagonal move");
		return CBT_ERROR_INVALIDMOVE;
	}

	dx = x2 - x1;
	dy = y2 - y1;

	// Check if only of distance 1
	if (LAST(cbt_game.map[from].unit) != U_SCOUT && abs(dx + dy) != 1) {
		MOVE_ERROR("dist > 1 and not a scout");
		return CBT_ERROR_INVALIDMOVE;
	} else if (LAST(cbt_game.map[from].unit) == U_SCOUT) {
		// It is a scout!
		// Checks everything between
		if (dx != 0)
			dir = dx/abs(dx);
		if (dy != 0)
			dir = dy/abs(dy) * cbt_game.width;
		for (a = from; a != to; a += dir) {
			if (a != from && (LAST(cbt_game.map[a].type) != T_OPEN ||
					LAST(cbt_game.map[a].unit) != U_EMPTY)) {
				MOVE_ERROR("invaolid scout attack");
				return CBT_ERROR_INVALIDMOVE;
			}
		}
		// Sanity check (can't move and attack at the same time!)
		if (abs(dx+dy) != 1 && LAST(cbt_game.map[to].unit) != U_EMPTY) {
			MOVE_ERROR("super scout?");
			return  CBT_ERROR_INVALIDMOVE;
		}
	}

	// Makes the move!
	t_u = LAST(cbt_game.map[to].unit);
	seat2 = GET_OWNER(cbt_game.map[to].unit);
	f_u = LAST(cbt_game.map[from].unit);

	switch (t_u) {
		case U_FLAG:
			// The f_u always wins
			f_u*=-1;
			break;
		case U_BOMB:
			// The f_u is a Miner?
			if (f_u == U_MINER)
				f_u*=-1;
			else
				t_u*=-1;
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
			if (f_u == U_SPY)
				f_u *= -1;
			else if (f_u == U_MARSHALL) {
				f_u *= -1;
				t_u *= -1;
			} else
				t_u *= -1;
			break;
		default:
			MOVE_ERROR("peaceful attack");
			return CBT_ERROR_INVALIDMOVE;
	}

	// Now do the tough work
	if (f_u < 0 && t_u >= 0) {
		// The from unit won!
		cbt_game.map[from].unit = U_EMPTY;
		cbt_game.map[to].unit = OWNER(seat) - f_u;
		cbt_game.army[seat2][t_u]--;
	} else if (f_u >= 0 && t_u < 0) {
		// The to unit won!
		cbt_game.map[from].unit = U_EMPTY;
		cbt_game.map[to].unit = OWNER(seat2) - t_u;
		cbt_game.army[seat][f_u]--;
	} else if (f_u < 0 && t_u < 0) {
		// Both won (or lost, whatever!)
		cbt_game.map[from].unit = U_EMPTY;
		cbt_game.map[to].unit = U_EMPTY;
		cbt_game.army[seat][-f_u]--;
		cbt_game.army[seat2][-t_u]--;
	} else {
		ggz_debug("Problems in the attack logic!");
		return CBT_ERROR_CRAZY;
	}

	// Send messages
	for (a = 0; a < ggz_seats_num(); a++) {
		fd = ggz_seats[a].fd;
		if (fd <= 0)
			continue;
		if (es_write_int(fd, CBT_MSG_ATTACK) < 0 ||
				es_write_int(fd, from) < 0 ||
				es_write_int(fd, f_u) < 0 ||
				es_write_int(fd, to) < 0 ||
				es_write_int(fd, t_u) < 0)
			return CBT_ERROR_SOCKET;
	}

	return 1;
}

int game_check_over() {
	int *alive, a, b, no_alives = 0;

	// TODO: Check if the player has no way to move his units
	
	alive = (int *)malloc(ggz_seats_num() * sizeof(int));
	
	for (b = 0; b < ggz_seats_num(); b++) {
		// Sees if the player has flags left
		if (cbt_game.army[b][U_FLAG] <= 0) {
			alive[b] = 0;
			continue;
		}
		// Checks if the player has no moving units
		alive[b] = 0;
		for (a = U_SCOUT; a < 12; a++) {
			if (cbt_game.army[b][a] > 0)
				alive[b] = 1;
		}
	}

	// Now checks if it is only one alive
	for (a = 0; a < ggz_seats_num(); a++) {
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

void game_send_gameover(int winner) {
	int a, fd;

	for (a = 0; a < ggz_seats_num(); a++) {
		fd = ggz_seats[a].fd;
		if (fd <= 0)
			continue;
		if (es_write_int(fd, CBT_MSG_GAMEOVER) < 0 ||
				es_write_int(fd, winner) < 0)
			ggz_debug("Can't send ending message to player %d", a);
	}

	cbt_game.state = CBT_STATE_DONE;
}
