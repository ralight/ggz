/*
 * File: ggzdmod.c
 * Author: GGZ Dev Team
 * Project: GGZ
 * Date: 8/28/01
 * Desc: GGZD game module functions
 * $Id: ggzdmod.c 2321 2001-08-29 07:01:30Z jdorje $
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

#include <easysock.h>
#include "../game_servers/libggzmod/ggz_protocols.h"

#include "ggzdmod.h"

int ggzdmod_send_launch(int fd, int num_seats, int *types, char **reserves)
{
	int i;

	/* Send number of seats */
	if (es_write_int(fd, REQ_GAME_LAUNCH) < 0
	    || es_write_int(fd, num_seats) < 0)
		return -1;

	/* Send seat assignments, names, and fd's */
	for (i = 0; i < num_seats; i++) {

		if (es_write_int(fd, types[i]) < 0)
			return -1;

		if (types[i] == GGZ_SEAT_RESV
		    && es_write_string(fd, reserves[i]) < 0)
			return -1;
	}

	return 0;
}

int ggzdmod_rsp_gameover(int fd)
{
	if (es_write_int(fd, RSP_GAME_OVER) < 0)
		return -1;
	return 0;
}

int ggzdmod_req_gamejoin(int fd, int seat, char *name, int player_fd)
{
	if (es_write_int(fd, REQ_GAME_JOIN) < 0
	    || es_write_int(fd, seat) < 0
	    || es_write_string(fd, name) < 0
	    || es_write_fd(fd, player_fd) < 0)
		return -1;
	return 0;
}


int ggzdmod_req_gameleave(int fd, char *name)
{
	if (es_write_int(fd, REQ_GAME_LEAVE) < 0
	    || es_write_string(fd, name) < 0)
		return -1;
	return 0;
}

int ggzdmod_dispatch(int fd, void *data)
{
	int status = 0, opcode;
	char stat;
	TableToControl op;

	if (es_read_int(fd, &opcode) < 0)
		return -1;
	op = opcode;

	switch (op) {
	case RSP_GAME_LAUNCH:
		if (es_read_char(fd, &stat) < 0)
			return -1;
		status = table_game_launch(data, stat);
		break;

	case RSP_GAME_JOIN:
		if (es_read_char(fd, &stat) < 0)
			return -1;
		status = table_game_join(data, stat);
		break;

	case RSP_GAME_LEAVE:
		if (es_read_char(fd, &stat) < 0)
			return -1;
		status = table_game_leave(data, stat);
		break;

	case REQ_GAME_OVER:
		table_game_over(data);
		/* Send response back to game server, allowing termination */
		(void) ggzdmod_rsp_gameover(fd);
		status = -1;
		break;

	case MSG_LOG:
		status = table_log(data, 0);
		break;

	case MSG_DBG:
		status = table_log(data, 1);
		break;

	default:
#define GGZ_DBG_PROTOCOL	(unsigned) 0x00000020
		dbg_msg(GGZ_DBG_PROTOCOL, "Table sent unimplemented op %d",
			op);
		status = -1;
		break;
	}
	return status;
}
