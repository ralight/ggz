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
  cbt_game.name = NULL;
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
      // Now we know how many players we have!
      cbt_game.number = ggz_seats_num();
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
				if (!ggz_seats_open() && cbt_game.map) { 
					// Request setup!
					cbt_game.state = CBT_STATE_SETUP;
					game_request_setup(-1);
				}
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

	fd = ggz_seats[seat].fd;

	if (es_read_int(fd, &op) < 0)
		return -1;

	ggz_debug("Got message %d from player %d\n", op, seat);

	switch (op) {

		case CBT_MSG_OPTIONS:
			if (game_get_options(seat)) {
				// Sends options to everyone
				for (a = 0; a < cbt_game.number; a++) {
					if (ggz_seats[a].fd >= 0)
						game_send_options(a);
				}
				// Check if must start the game
				if (!ggz_seats_open() && cbt_game.map)
					game_request_setup(-1);
			} else {
				// Free memory
				free(cbt_game.map);
				for (a = 0; a < cbt_game.number; a++)
					free(cbt_game.army[a]);
				free(cbt_game.army);
				// Init them again
				game_init();
				// We are at the WAIT turn
				cbt_game.state = CBT_STATE_WAIT;
				game_request_options(seat);
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
				cbt_game.turn = NEXT(cbt_game.turn, cbt_game.number);
			// Check if the game is over
			a = game_check_over();
			if (a <= 0)
				game_send_gameover(-a);
			break;

		case CBT_REQ_SYNC:
			ggz_debug("Got sync message");
			game_send_sync(seat, 0);
			break;

		case CBT_REQ_OPTIONS:
			game_send_options(seat);
			break;

		default:
			status = CBT_SERVER_ERROR;
			break;

	}

	return status;

}

void game_send_sync(int seat, int cheated) {
	int fd = ggz_seats[seat].fd;
	char *syncstr;
	int a, len;

	ggz_debug("Sending sync to player %d (%d)", seat, cheated);

	if (fd < 0)
		return;

	len = 1+1+1+cbt_game.width*cbt_game.height;

	syncstr = (char *)malloc(sizeof(char)*len);

	syncstr[0] = cbt_game.turn;
	syncstr[1] = cbt_game.state;

	for (a = 0; a < cbt_game.width*cbt_game.height; a++) {
		// If I own it, or it is cheated send all the info to me
		if (cheated || GET_OWNER(cbt_game.map[a].unit) == seat)
			syncstr[a+2] = cbt_game.map[a].unit;
		else if (LAST(cbt_game.map[a].unit) != U_EMPTY)
			// It is a unit, although not mine
			syncstr[a+2] = FIRST(cbt_game.map[a].unit) + U_UNKNOWN;
		else
			// It is not a unit
			syncstr[a+2] = U_EMPTY;
	}

	syncstr[len-1] = 0;

	// Increase one of each character
	for (a = 0; a < len-1; a++)
		syncstr[a]++;

	ggz_debug("Sent a sync string of size %d. Expected %d", strlen(syncstr), len-1);

	// Send the string
	if (es_write_int(fd, CBT_MSG_SYNC) || es_write_string(fd, syncstr) < 0) {
		ggz_debug("Can't send sync string");
		return;
	}

	return;
}

void game_send_seat(int seat) {
	int fd = ggz_seats[seat].fd;

	ggz_debug("Sending player %d his seat numnber\n", seat);
  ggz_debug("Number of players: %d\n", cbt_game.number);

	if (es_write_int(fd, CBT_MSG_SEAT) < 0 || es_write_int(fd, seat) < 0 || es_write_int(fd, cbt_game.number) < 0 || es_write_int(fd, PROTOCOL_VERSION) < 0) {
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
	int a, b, size = 0, cur_size = 0;

	ggz_debug("Getting options");

	if (es_read_string_alloc(fd, &optstr) < 0)
		return 0;

	ggz_debug("Len: %d", strlen(optstr));

	combat_options_string_read(optstr, &cbt_game);

	if (cbt_game.army[cbt_game.number][U_FLAG] <= 0) {
		ggz_debug("Not enough flags");
		return 0;
	}

	for (a = 2; a < 13; a++) {
		if (a == 12) {
			ggz_debug("No moving units");
			return 0;
		}
		if (cbt_game.army[cbt_game.number][a] > 0)
			break;
	}

	for (a = 0; a < 12; a++)
		size+=cbt_game.army[cbt_game.number][a];

	// Checks for number of starting positions
	for (a = 0; a < cbt_game.number; a++) {	
		for (b = 0; b < cbt_game.width * cbt_game.height; b++) {
			if (GET_OWNER(cbt_game.map[b].type) == a)
				cur_size++;
		}
		if (cur_size < size) {
			ggz_debug("Not enough space");
			return 0;
		}
		cur_size = 0;
	}

	return 1;
}

void game_send_options(int seat) {
	int fd = ggz_seats[seat].fd;

	if (fd < 0) {
		ggz_debug("This fd doesn't exists!");
		return;
	}

	if (es_write_int(fd, CBT_MSG_OPTIONS) < 0 || es_write_string(fd, combat_options_string_write(&cbt_game, 0)) < 0)
			return;
	
	return;
}

void game_send_players() {
	int i, j, fd;

	for (j = 0; j < cbt_game.number; j++) {
		if ( (fd = ggz_seats[j].fd) == -1 ) {
			ggz_debug("Bot seat\n");
			continue;
		}
		ggz_debug("Sending player list to player %d", j);

		if (es_write_int(fd, CBT_MSG_PLAYERS) < 0) {
			ggz_debug("Can't send player list!\n");
			return;
		}
	
		for (i = 0; i < cbt_game.number; i++) {
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
		for (a = 0; a < cbt_game.number; a++) {
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
	int used[12];
	static int done_setup = 0;
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
	

	// Check if he has already sent a setup
	for (a = 0; a < cbt_game.width*cbt_game.height; a++) {
		if (GET_OWNER(cbt_game.map[a].unit) == seat && LAST(cbt_game.map[a].unit) != U_EMPTY) {
			ggz_debug("Setup error: Already sent setup");
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

void game_start() {
	int a, fd;

	for (a = 0; a < cbt_game.number; a++) {
		fd = ggz_seats[a].fd;
		if (fd < 0)
			continue;
		if (es_write_int(fd, CBT_MSG_START) < 0)
			ggz_debug("Can't send start message to player %d", a);
	}

	cbt_game.state = CBT_STATE_PLAYING;
}

void game_send_move_error(int seat, int error) {
	int fd, a;

	for (a = 0; a < cbt_game.number; a++) {
		fd = ggz_seats[a].fd;
		if (fd < 0)
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
	int a;

	if (es_read_int(fd, &from) < 0 || es_read_int(fd, &to) )
		return CBT_ERROR_SOCKET;

	a = combat_check_move(&cbt_game, from, to);

	// Now checks if its a attack or a move
	if (a == CBT_CHECK_MOVE)
		return game_handle_move(seat, from, to);

	if (a == CBT_CHECK_ATTACK)
		return game_handle_attack(seat, from, to);

	// An error ocurred
	return a;
}

int game_handle_move(int seat, int from, int to) {
	int fd, a;

	// Makes the move!
	cbt_game.map[to].unit = OWNER(seat) + LAST(cbt_game.map[from].unit);
	cbt_game.map[from].unit = U_EMPTY;
	
	// Ok... now send it!
	for (a = 0; a < cbt_game.number; a++) {
		fd = ggz_seats[a].fd;
		if (fd < 0)
			continue;
		if (es_write_int(fd, CBT_MSG_MOVE) < 0 ||
				es_write_int(fd, from) < 0 ||
				es_write_int(fd, to) < 0)
			return CBT_ERROR_SOCKET;
	}

	return 1;
}
	
	
int game_handle_attack(int f_s, int from, int to) {
	int fd, a, t_u, f_u, t_s;

	// Gets variables
	f_u = LAST(cbt_game.map[from].unit);
	t_u = LAST(cbt_game.map[to].unit);
	t_s = GET_OWNER(cbt_game.map[to].unit);

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
			return CBT_ERROR_CRAZY;
	}

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
		ggz_debug("Problems in the attack logic!");
		return CBT_ERROR_CRAZY;
	}

	// Send messages
	for (a = 0; a < cbt_game.number; a++) {
		fd = ggz_seats[a].fd;
		if (fd < 0)
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

void game_send_gameover(int winner) {
	int a, fd;

	for (a = 0; a < cbt_game.number; a++) {
		fd = ggz_seats[a].fd;
		if (fd < 0)
			continue;
		if (es_write_int(fd, CBT_MSG_GAMEOVER) < 0 ||
				es_write_int(fd, winner) < 0)
			ggz_debug("Can't send ending message to player %d", a);
		// Send all the map data
		game_send_sync(a, 1);
	}

	cbt_game.state = CBT_STATE_DONE;
}
