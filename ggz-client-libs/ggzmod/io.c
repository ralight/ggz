/* 
 * File: io.c
 * Author: GGZ Dev Team
 * Project: ggzmod
 * Date: 10/14/01
 * Desc: Functions for reading/writing messages from/to game modules
 * $Id: io.c 4927 2002-10-15 01:57:43Z jdorje $
 *
 * This file contains the backend for the ggzmod library.  This
 * library facilitates the communication between the GGZ server (ggz)
 * and game servers.  This file provides backend code that can be
 * used at both ends.
 *
 * Copyright (C) 2001 GGZ Dev Team.
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

#include <assert.h>
#include <stdlib.h>

#include <ggz.h>

#include "ggzmod.h"
#include "mod.h"
#include "io.h"
#include "protocol.h"

/* Private IO reading functions */
static int _io_read_msg_launch(GGZMod *ggzmod);
static int _io_read_msg_state(GGZMod *ggzmod);
static int _io_read_msg_server(GGZMod *ggzmod);
static int _io_read_msg_player(GGZMod *ggzmod);
static int _io_read_msg_seat(GGZMod *ggzmod);
static int _io_read_msg_spectator_seat(GGZMod *ggzmod);


/* Functions for sending IO messages */
int _io_send_launch(int fd)
{
	if (ggz_write_int(fd, MSG_GAME_LAUNCH) < 0)
		return -1;

	return 0;
}


int _io_send_server(int fd, int server_fd)
{
	if (ggz_write_int(fd, MSG_GAME_SERVER) < 0
	    || ggz_write_fd(fd, server_fd) < 0)
		return -1;
	else
		return 0;
}


int _io_send_state(int fd, GGZModState state)
{
	if (ggz_write_int(fd, MSG_GAME_STATE) < 0
	    || ggz_write_char(fd, state) < 0)
		return -1;
	else
		return 0;
}

int _io_send_player(int fd, const char *name, int is_spectator, int seat_num)
{
	if (ggz_write_int(fd, MSG_GAME_PLAYER) < 0
	    || ggz_write_string(fd, name ? name : "") < 0
	    || ggz_write_int(fd, is_spectator) < 0
	    || ggz_write_int(fd, seat_num) < 0)
		return -1;

	return 0;
}

int _io_send_seat(int fd, GGZSeat *seat)
{
	if (ggz_write_int(fd, MSG_GAME_SEAT) < 0
	    || ggz_write_int(fd, seat->num) < 0
	    || ggz_write_int(fd, seat->type) < 0
	    || ggz_write_string(fd, seat->name ? seat->name : "") < 0)
		return -1;

	return 0;
}

int _io_send_spectator_seat(int fd, GGZSpectatorSeat *seat)
{
	char *name = seat->name ? seat->name : "";

	if (ggz_write_int(fd, MSG_GAME_SPECTATOR_SEAT) < 0
	    || ggz_write_int(fd, seat->num) < 0
	    || ggz_write_string(fd, name) < 0)
		return -1;

	return 0;
}


/* Functions for reading messages */
int _io_read_data(GGZMod *ggzmod)
{
	int op;

	if (ggz_read_int(ggzmod->fd, &op) < 0)
		return -1;

	if (ggzmod->type == GGZMOD_GAME) {
		switch ((ControlToTable)op) {
		case MSG_GAME_LAUNCH:
			return _io_read_msg_launch(ggzmod);
		case MSG_GAME_SERVER:
			return _io_read_msg_server(ggzmod);
		case MSG_GAME_PLAYER:
			return _io_read_msg_player(ggzmod);
		case MSG_GAME_SEAT:
			return _io_read_msg_seat(ggzmod);
		case MSG_GAME_SPECTATOR_SEAT:
			return _io_read_msg_spectator_seat(ggzmod);
		}
	} else {
		switch ((TableToControl)op) {
		case MSG_GAME_STATE:
			return _io_read_msg_state(ggzmod);
		}
	}

	return -2;
}


static int _io_read_msg_state(GGZMod *ggzmod)
{
	char state;

	if (ggz_read_char(ggzmod->fd, &state) < 0)
		return -1;
	else
		_ggzmod_handle_state(ggzmod, state);
	
	return 0;
}



static int _io_read_msg_launch(GGZMod *ggzmod)
{
	_ggzmod_handle_launch(ggzmod);

	return 0;
}



static int _io_read_msg_server(GGZMod *ggzmod)
{
	int fd;

	if (ggz_read_fd(ggzmod->fd, &fd) < 0)
		return -1;
	else
		_ggzmod_handle_server(ggzmod, fd);
	
	return 0;
}

static int _io_read_msg_player(GGZMod *ggzmod)
{
	int is_spectator, seat_num;
	char *name;

	if (ggz_read_string_alloc(ggzmod->fd, &name) < 0
	    || ggz_read_int(ggzmod->fd, &is_spectator) < 0
	    || ggz_read_int(ggzmod->fd, &seat_num) < 0)
		return -1;

	_ggzmod_handle_player(ggzmod,
			      name[0] == '\0' ? NULL : name,
			      is_spectator, seat_num);

	ggz_free(name);
	return 0;
}

static int _io_read_msg_seat(GGZMod *ggzmod)
{
	GGZSeat seat = {name: NULL};

	if (ggz_read_int(ggzmod->fd, &seat.num) < 0
	    || ggz_read_int(ggzmod->fd, (int*)&seat.type) < 0
	    || ggz_read_string_alloc(ggzmod->fd, &seat.name) < 0)
		  return -1;

	/* Detect no-name case */
	if (seat.name[0] == '\0') {
		ggz_free(seat.name);
		seat.name = NULL;
	}

	_ggzmod_handle_seat(ggzmod, &seat);

	if (seat.name)
		ggz_free(seat.name);

	return 0;
}

static int _io_read_msg_spectator_seat(GGZMod *ggzmod)
{
	GGZSpectatorSeat seat;

	if (ggz_read_int(ggzmod->fd, &seat.num) < 0
	    || ggz_read_string_alloc(ggzmod->fd, &seat.name) < 0)
		return -1;

	/* Detect empty seat case */
	if (seat.name[0] == '\0') {
		ggz_free(seat.name);
		seat.name = NULL;
	}

	_ggzmod_handle_spectator_seat(ggzmod, &seat);

	if (seat.name)
		ggz_free(seat.name);


	return 0;
}
