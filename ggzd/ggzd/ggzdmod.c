/* 
 * File: ggzdmod.c
 * Author: GGZ Dev Team
 * Project: GGZ
 * Date: 8/28/01
 * Desc: GGZD game module functions
 * $Id: ggzdmod.c 2599 2001-10-24 00:36:12Z jdorje $
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

#include <stdlib.h>

#include <easysock.h>
#include "../game_servers/libggzmod/ggz_protocols.h"

#include "ggzdmod.h"

/* Really we should #include "err_func.h", but that doesn't work for some
   reason. --JDS */
#define GGZ_DBG_TABLE		(unsigned) 0x00000010
#define GGZ_DBG_PROTOCOL	(unsigned) 0x00000020

GGZdmod *ggzdmod_new(int fd, void *table_data)
{
	GGZdmod *ggzdmod;

	ggzdmod = malloc(sizeof(*ggzdmod));
	if (!ggzdmod)
		return NULL;

	ggzdmod->fd = fd;
	ggzdmod->table_data = table_data;
	return ggzdmod;
}

void ggzdmod_free(GGZdmod * ggzdmod)
{
	/* anything else? */
	free(ggzdmod);
}

int ggzdmod_launch_game(GGZdmod * ggzdmod, int num_seats, int *types,
			char **reserves)
{
	int i;

	/* Send number of seats */
	if (es_write_int(ggzdmod->fd, REQ_GAME_LAUNCH) < 0
	    || es_write_int(ggzdmod->fd, num_seats) < 0)
		return -1;

	/* Send seat assignments, names, and fd's */
	for (i = 0; i < num_seats; i++) {

		if (es_write_int(ggzdmod->fd, types[i]) < 0)
			return -1;

		if (types[i] == GGZ_SEAT_RESV
		    && es_write_string(ggzdmod->fd, reserves[i]) < 0)
			return -1;
	}

	return 0;
}

static int ggzdmod_rsp_gameover(GGZdmod * ggzdmod)
{
	if (es_write_int(ggzdmod->fd, RSP_GAME_OVER) < 0)
		return -1;
	return 0;
}

int ggzdmod_player_join(GGZdmod * ggzdmod, int seat, char *name,
			int player_fd)
{
	if (es_write_int(ggzdmod->fd, REQ_GAME_JOIN) < 0
	    || es_write_int(ggzdmod->fd, seat) < 0
	    || es_write_string(ggzdmod->fd, name) < 0
	    || es_write_fd(ggzdmod->fd, player_fd) < 0)
		return -1;
	return 0;
}


int ggzdmod_player_leave(GGZdmod * ggzdmod, char *name)
{
	if (es_write_int(ggzdmod->fd, REQ_GAME_LEAVE) < 0
	    || es_write_string(ggzdmod->fd, name) < 0)
		return -1;
	return 0;
}

int ggzdmod_handle_log(GGZdmod * ggzdmod)
{
	char *msg;
	int status;

	if (es_read_string_alloc(ggzdmod->fd, &msg) < 0)
		return -1;

	status = table_log(ggzdmod, msg, GGZ_DBG_TABLE, 1);
	free(msg);
	return status;
}

int ggzdmod_handle_seat_change(GGZdmod * ggzdmod)
{
	int seat, status;

	/* read the request */
	if (es_read_int(ggzdmod->fd, &seat) < 0 ||
	    es_read_int(ggzdmod->fd, &status) < 0)
		return -1;

	/* FIXME: handle the request rather than always responding "no". */
	if (es_write_int(ggzdmod->fd, RSP_SEAT_CHANGE) < 0 ||
	    es_write_int(ggzdmod->fd, -1) < 0)
		return -1;

	return 0;
}

int ggzdmod_dispatch(GGZdmod * ggzdmod)
{
	int status = 0, opcode;
	GGZdmod *ggz = ggzdmod;
	char stat;
	TableToControl op;

	if (es_read_int(ggz->fd, &opcode) < 0)
		return -1;
	op = opcode;

	switch (op) {
	case RSP_GAME_LAUNCH:
		if (es_read_char(ggz->fd, &stat) < 0)
			return -1;
		status = table_game_launch(ggzdmod, stat);
		break;

	case RSP_GAME_JOIN:
		if (es_read_char(ggz->fd, &stat) < 0)
			return -1;
		status = table_game_join(ggzdmod, stat);
		break;

	case RSP_GAME_LEAVE:
		if (es_read_char(ggz->fd, &stat) < 0)
			return -1;
		status = table_game_leave(ggzdmod, stat);
		break;

	case REQ_GAME_OVER:
		table_game_over(ggzdmod);
		/* Send response back to game server, allowing termination */
		(void) ggzdmod_rsp_gameover(ggzdmod);
		status = -1;
		break;

	case MSG_LOG:
		status = ggzdmod_handle_log(ggzdmod);
		break;

	case REQ_SEAT_CHANGE:
		status = ggzdmod_handle_seat_change(ggzdmod);
		break;

		/* case MSG_STATS: */
	default:
		dbg_msg(GGZ_DBG_PROTOCOL, "Table sent unimplemented op %d",
			op);
		status = -1;
		break;
	}
	return status;
}
