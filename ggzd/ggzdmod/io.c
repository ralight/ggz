/* 
 * File: io.c
 * Author: GGZ Dev Team
 * Project: ggzdmod
 * Date: 10/14/01
 * Desc: Functions for reading/writing messages from/to game modules
 * $Id: io.c 4947 2002-10-18 22:46:42Z jdorje $
 *
 * This file contains the backend for the ggzdmod library.  This
 * library facilitates the communication between the GGZ server (ggzd)
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

#include <stdlib.h>

#include <ggz.h>

#include "ggzdmod.h"
#include "mod.h"
#include "io.h"
#include "protocol.h"


/* Private IO reading functions */
static int _io_read_req_launch(GGZdMod *ggzdmod);
static int _io_read_req_join(GGZdMod *ggzdmod);
static int _io_read_req_leave(GGZdMod *ggzdmod);
static int _io_read_req_spectator_join(GGZdMod *ggzdmod);
static int _io_read_req_spectator_leave(GGZdMod *ggzdmod);
static int _io_read_req_state(GGZdMod *ggzdmod);
static int _io_read_msg_log(GGZdMod *ggzdmod);
static int _io_read_req_seat(GGZdMod * ggzdmod);


/* Functions for sending IO messages */
int _io_send_launch(int fd, int seats, int spectators)
{
	if (ggz_write_int(fd, MSG_GAME_LAUNCH) < 0 
	    || ggz_write_int(fd, seats) < 0
		|| ggz_write_int(fd, spectators) < 0)
		return -1;
	else
		return 0;
}


int _io_send_seat_change(int fd, GGZSeat *seat)
{
	ggz_debug("GGZDMOD", "Sending seat change");
	if (ggz_write_int(fd, REQ_GAME_SEAT) < 0
	    || ggz_write_int(fd, seat->num) < 0
	    || ggz_write_int(fd, seat->type) < 0)
		return -1;

	if (seat->type == GGZ_SEAT_PLAYER 
	    || seat->type == GGZ_SEAT_RESERVED) {
		ggz_debug("GGZDMOD", "Sending seat change name");
		if (ggz_write_string(fd, seat->name) < 0)
			return -1;
	}


	if (seat->type == GGZ_SEAT_PLAYER) {
		ggz_debug("GGZDMOD", "Sending seat change fd");
		if (ggz_write_fd(fd, seat->fd) < 0)
			return -1;
	}
	return 0;
}


int _io_send_join(int fd, GGZSeat *seat)
{
	ggz_debug("GGZDMOD", "Sending join");
	if (ggz_write_int(fd, REQ_GAME_JOIN) < 0
	    || ggz_write_int(fd, seat->num) < 0
	    || ggz_write_string(fd, seat->name) < 0
	    || ggz_write_fd(fd, seat->fd) < 0)
		return -1;
	else
		return 0;
}


int _io_send_leave(int fd, GGZSeat *seat)
{
	if (ggz_write_int(fd, REQ_GAME_LEAVE) < 0
	    || ggz_write_string(fd, seat->name) < 0)
		return -1;
	else
		return 0;
}

int _io_send_spectator_join(int fd, GGZSpectator *spectator)
{
	ggz_debug("GGZDMOD", "Sending spectator join");
	if (ggz_write_int(fd, REQ_GAME_SPECTATOR_JOIN) < 0
	    || ggz_write_int(fd, spectator->num) < 0
	    || ggz_write_string(fd, spectator->name) < 0
	    || ggz_write_fd(fd, spectator->fd) < 0)
		return -1;
	else
		return 0;
}

int _io_send_spectator_leave(int fd, GGZSpectator *spectator)
{
	if (ggz_write_int(fd, REQ_GAME_SPECTATOR_LEAVE) < 0
	    || ggz_write_string(fd, spectator->name) < 0)
		return -1;
	else
		return 0;
}

int _io_send_state(int fd, GGZdModState state)
{
	if (ggz_write_int(fd, REQ_GAME_STATE) < 0
	    || ggz_write_char(fd, state) < 0)
		return -1;
	else
		return 0;
}


int _io_send_seat(int fd, GGZSeat *seat)
{
	if (ggz_write_int(fd, seat->type) < 0)
		return -1;
	
	if (seat->type == GGZ_SEAT_RESERVED) {
		if (ggz_write_string(fd, seat->name) < 0)
			return -1;
	}

	return 0;
}


int _io_send_log(int fd, char *msg)
{
	if (ggz_write_int(fd, MSG_LOG) < 0 
	    || ggz_write_string(fd, msg) < 0)
		return -1;
	else
		return 0;
}


int _io_respond_state(int fd)
{
	return ggz_write_int(fd, RSP_GAME_STATE);
}


/* Functions for reading messages */
int _io_read_data(GGZdMod * ggzdmod)
{
	int op;

	if (ggz_read_int(ggzdmod->fd, &op) < 0)
		return -1;

	if (ggzdmod->type == GGZDMOD_GAME) {
		switch ((ControlToTable)op) {
		case MSG_GAME_LAUNCH:
			return _io_read_req_launch(ggzdmod);
		case REQ_GAME_JOIN:
			return _io_read_req_join(ggzdmod);
		case REQ_GAME_LEAVE:
			return _io_read_req_leave(ggzdmod);
		case REQ_GAME_SEAT:
			return _io_read_req_seat(ggzdmod);
		case REQ_GAME_SPECTATOR_JOIN:
			return _io_read_req_spectator_join(ggzdmod);
		case REQ_GAME_SPECTATOR_LEAVE:
			return _io_read_req_spectator_leave(ggzdmod);
		case RSP_GAME_STATE:
			_ggzdmod_handle_state_response(ggzdmod);
			return 0;
		}
	} else {
		switch ((TableToControl)op) {
		case REQ_GAME_STATE:
			return _io_read_req_state(ggzdmod);
		case MSG_LOG:
			return _io_read_msg_log(ggzdmod);
		}
	}

	return -1;
}


static int _io_read_req_state(GGZdMod * ggzdmod)
{
	char state;

	if (ggz_read_char(ggzdmod->fd, &state) < 0)
		return -1;
	else
		_ggzdmod_handle_state(ggzdmod, state);
	
	return 0;
}


static int _io_read_msg_log(GGZdMod * ggzdmod)
{
	char *msg;

	if (ggz_read_string_alloc(ggzdmod->fd, &msg) < 0)
		return -1;
	else {
		_ggzdmod_handle_log(ggzdmod, msg);
		ggz_free(msg);
	}
	
	return 0;
}


static int _io_read_req_launch(GGZdMod * ggzdmod)
{
	int seats, spectators, i;
	GGZSeat seat;
	
	if (ggz_read_int(ggzdmod->fd, &seats) < 0)
		return -1;
	if (ggz_read_int(ggzdmod->fd, &spectators) < 0)
		return -1;

	_ggzdmod_handle_launch_begin(ggzdmod, seats, spectators);

	for (i = 0; i < seats; i++) {
		/* Reset seat */
		seat.num = i;
		seat.name = NULL;
		seat.fd = -1;
		
		if (ggz_read_int(ggzdmod->fd, (int*)&seat.type) < 0)
			return -1;

		if (seat.type == GGZ_SEAT_RESERVED)
			if (ggz_read_string_alloc(ggzdmod->fd, &seat.name) < 0)
				return -1;
		
		_ggzdmod_handle_launch_seat(ggzdmod, seat);

		/* Free up name (if it was allocated) */
		if (seat.name)
			ggz_free(seat.name);
	}

	_ggzdmod_handle_launch_end(ggzdmod);
	
	return 0;
}


static int _io_read_req_join(GGZdMod * ggzdmod)
{
	GGZSeat seat;
	
	/* Is it true that only human players join this way? */
	seat.type = GGZ_SEAT_PLAYER;
	
	if (ggz_read_int(ggzdmod->fd, &seat.num) < 0
	    || ggz_read_string_alloc(ggzdmod->fd, &seat.name) < 0
	    || ggz_read_fd(ggzdmod->fd, &seat.fd) < 0)
		return -1;
	else {
		_ggzdmod_handle_join(ggzdmod, seat);
		ggz_free(seat.name);
		return 0;
	}
}


static int _io_read_req_leave(GGZdMod * ggzdmod)
{
	char *name;

	if (ggz_read_string_alloc(ggzdmod->fd, &name) < 0)
		return -1;

	_ggzdmod_handle_leave(ggzdmod, name);
	ggz_free(name);

	return 0;
}

static int _io_read_req_spectator_join(GGZdMod * ggzdmod)
{
	GGZSpectator spectator;
	
	if (ggz_read_int(ggzdmod->fd, &spectator.num) < 0
	    || ggz_read_string_alloc(ggzdmod->fd, &spectator.name) < 0
	    || ggz_read_fd(ggzdmod->fd, &spectator.fd) < 0)
		return -1;
	else {
		_ggzdmod_handle_spectator_join(ggzdmod, spectator);
		ggz_free(spectator.name);
		return 0;
	}
}

static int _io_read_req_spectator_leave(GGZdMod * ggzdmod)
{
	char *name;

	if (ggz_read_string_alloc(ggzdmod->fd, &name) < 0)
		return -1;

	_ggzdmod_handle_spectator_leave(ggzdmod, name);
	ggz_free(name);

	return 0;
}


static int _io_read_req_seat(GGZdMod * ggzdmod)
{
	GGZSeat seat;
	
	/* Initialize to sane values */
	seat.name = NULL;
	seat.fd = -1;
	
	if (ggz_read_int(ggzdmod->fd, &seat.num) < 0
	    || ggz_read_int(ggzdmod->fd, (int*)&seat.type) < 0)
		return -1;

	if (seat.type == GGZ_SEAT_PLAYER || seat.type == GGZ_SEAT_RESERVED) {
		if (ggz_read_string_alloc(ggzdmod->fd, &seat.name) < 0)
			return -1;
	}
	
	if (seat.type == GGZ_SEAT_PLAYER) {
		if (ggz_read_fd(ggzdmod->fd, &seat.fd) < 0)
			return -1;
	}
	
	_ggzdmod_handle_seat(ggzdmod, seat);

	if (seat.type == GGZ_SEAT_PLAYER || seat.type == GGZ_SEAT_RESERVED)
		ggz_free(seat.name);
	
	return 0;
}





