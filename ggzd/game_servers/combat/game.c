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

// Global game variable
combat_game cbt_game;

// Who is the host?
int host = -1;


void game_init() {
	cbt_game.map = NULL;
	cbt_game.width = 0;
	cbt_game.height = 0;
	cbt_game.state = CBT_STATE_INIT;
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
					game_start();
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
	int fd, op, a, status;
	
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
				game_start();
			break;

		default:
			status = CBT_SERVER_ERROR;
			break;

	}

	return 0;

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

void game_get_options(int seat) {
	int fd = ggz_seats[seat].fd;
	char *optstr = NULL;

	ggz_debug("Getting options");

	if (es_read_string_alloc(fd, &optstr) < 0)
		return;

	ggz_debug("Len: %d", strlen(optstr));

	// FIXME: Check for validity
	combat_options_string_read(optstr, &cbt_game, ggz_seats_num());

	

	return;
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

void game_start() {
	// FIXME: Must add this!
	return;
}
