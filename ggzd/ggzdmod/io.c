/* 
 * File: io.c
 * Author: GGZ Dev Team
 * Project: ggzdmod
 * Date: 10/14/01
 * Desc: Functions for reading/writing messages from/to game modules
 * $Id: io.c 3105 2002-01-13 19:02:42Z jdorje $
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

#include <config.h>		/* site-specific config */
#include <easysock.h>
#include <ggz.h>
#include <stdlib.h>

#include "ggzdmod.h"
#include "mod.h"
#include "io.h"
#include "protocol.h"


/* Private IO reading functions */
static int _io_read_req_launch(GGZdMod *ggzdmod);
static int _io_read_req_join(GGZdMod *ggzdmod);
static int _io_read_rsp_join(GGZdMod *ggzdmod);
static int _io_read_req_leave(GGZdMod *ggzdmod);
static int _io_read_rsp_leave(GGZdMod *ggzdmod);
static int _io_read_req_state(GGZdMod *ggzdmod);
static int _io_read_msg_log(GGZdMod *ggzdmod);


/* Functions for sending IO messages */
int _io_send_launch(int fd, int seats)
{
	if (es_write_int(fd, MSG_GAME_LAUNCH) < 0 
	    || es_write_int(fd, seats) < 0)
		return -1;
	else
		return 0;
}
	

int _io_send_join(int fd, GGZSeat *seat)
{
	ggz_debug("GGZDMOD", "Sending seat data");
	if (es_write_int(fd, REQ_GAME_JOIN) < 0
	    || es_write_int(fd, seat->num) < 0
	    || es_write_string(fd, seat->name) < 0
	    || es_write_fd(fd, seat->fd) < 0)
		return -1;
	else
		return 0;
}


int _io_send_leave(int fd, GGZSeat *seat)
{
	if (es_write_int(fd, REQ_GAME_LEAVE) < 0
	    || es_write_string(fd, seat->name) < 0)
		return -1;
	else
		return 0;
}


int _io_send_state(int fd, GGZdModState state)
{
	if (es_write_int(fd, REQ_GAME_STATE) < 0
	    || es_write_char(fd, state) < 0)
		return -1;
	else
		return 0;
}


int _io_send_seat(int fd, GGZSeat *seat)
{
	if (es_write_int(fd, seat->type) < 0)
		return -1;
	
	if (seat->type == GGZ_SEAT_RESV) {
		if (es_write_string(fd, seat->name) < 0)
			return -1;
	}

	return 0;
}


int _io_send_log(int fd, char *msg)
{
	if (es_write_int(fd, MSG_LOG) < 0 
	    || es_write_string(fd, msg) < 0)
		return -1;
	else
		return 0;
}


/* Functions for sending repsonses */
int _io_respond_join(int fd)
{
	if (es_write_int(fd, RSP_GAME_JOIN) < 0
	    || es_write_char(fd, 0) < 0)
		return -1;
	else
		return 0;
}


int _io_respond_leave(int fd, int status)
{
	if (es_write_int(fd, RSP_GAME_LEAVE) < 0 ||
	    es_write_char(fd, status) < 0)
		return -1;
	else
		return 0;
}


int _io_respond_state(int fd)
{
	return es_write_int(fd, RSP_GAME_STATE);
}


/* Functions for reading messages */
int _io_read_data(GGZdMod * ggzdmod)
{
	int op, status = 0;

	if (es_read_int(ggzdmod->fd, &op) < 0)
		return -1;

	if (ggzdmod->type == GGZDMOD_GAME) {
		switch (op) {
		case MSG_GAME_LAUNCH:
			status = _io_read_req_launch(ggzdmod);
			break;
		case REQ_GAME_JOIN:
			status = _io_read_req_join(ggzdmod);
			break;
		case REQ_GAME_LEAVE:
			status = _io_read_req_leave(ggzdmod);
			break;
		case RSP_GAME_STATE:
			_ggzdmod_handle_state_response(ggzdmod);
			break;
		default:
			/* FIXME: unknown protocol error */
			break;
		}
	} else {
		switch (op) {
		case RSP_GAME_JOIN:
			status = _io_read_rsp_join(ggzdmod);
			break;
		case RSP_GAME_LEAVE:
			status = _io_read_rsp_leave(ggzdmod);
			break;
		case REQ_GAME_STATE:
			status = _io_read_req_state(ggzdmod);
			break;
		case MSG_LOG:
			status = _io_read_msg_log(ggzdmod);
		default:
			break;
		}
	}

	return status;
}


static int _io_read_rsp_join(GGZdMod *ggzdmod)
{
	char status;

	if (es_read_char(ggzdmod->fd, &status) < 0)
		return -1;
	else
		_ggzdmod_handle_join_response(ggzdmod, status);
	
	return 0;
}


static int _io_read_rsp_leave(GGZdMod * ggzdmod)
{
	char status;

	if (es_read_char(ggzdmod->fd, &status) < 0)
		return -1;
	else
		_ggzdmod_handle_leave_response(ggzdmod, status);
	
	return 0;
}


static int _io_read_req_state(GGZdMod * ggzdmod)
{
	char state;

	if (es_read_char(ggzdmod->fd, &state) < 0)
		return -1;
	else
		_ggzdmod_handle_state(ggzdmod, state);
	
	return 0;
}



static int _io_read_msg_log(GGZdMod * ggzdmod)
{
	char *msg;

	if (es_read_string_alloc(ggzdmod->fd, &msg) < 0)
		return -1;
	else {
		_ggzdmod_handle_log(ggzdmod, msg);
		free(msg);
	}
	
	return 0;
}


static int _io_read_req_launch(GGZdMod * ggzdmod)
{
	int seats, i;
	GGZSeat seat;
	
	if (es_read_int(ggzdmod->fd, &seats) < 0)
		return -1;

	_ggzdmod_handle_launch_begin(ggzdmod, seats);

	for (i = 0; i < seats; i++) {
		/* Reset seat */
		seat.num = i;
		seat.name = NULL;
		seat.fd = -1;
		
		if (es_read_int(ggzdmod->fd, &seat.type) < 0)
			return -1;

		if (seat.type == GGZ_SEAT_RESV)
			if (es_read_string_alloc(ggzdmod->fd, &seat.name) < 0)
				return -1;
		
		_ggzdmod_handle_launch_seat(ggzdmod, seat);

		/* Free up name (if it was allocated) */
		if (seat.name)
			free(seat.name);
	}

	_ggzdmod_handle_launch_end(ggzdmod);
	
	return 0;
}


static int _io_read_req_join(GGZdMod * ggzdmod)
{
	GGZSeat seat;
	
	/* Is it true that only human players join this way? */
	seat.type = GGZ_SEAT_PLAYER;
	
	if (es_read_int(ggzdmod->fd, &seat.num) < 0
	    || es_read_string_alloc(ggzdmod->fd, &seat.name) < 0
	    || es_read_fd(ggzdmod->fd, &seat.fd) < 0)
		return -1;
	else {
		_ggzdmod_handle_join(ggzdmod, seat);
		free(seat.name);
		return 0;
	}
}


static int _io_read_req_leave(GGZdMod * ggzdmod)
{
	char *name;

	if (es_read_string_alloc(ggzdmod->fd, &name) < 0)
		return -1;

	_ggzdmod_handle_leave(ggzdmod, name);
	free(name);

	return 0;
}
