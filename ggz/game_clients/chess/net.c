/*
 * File: net.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 09/17/2000
 * Desc: Functions to filter input and send the events to game.c and send stuff
 * out to the server
 * $Id: net.c 6903 2005-01-25 18:57:38Z jdorje $
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include <ggz.h>
#include <ggzmod.h>
#include <ggz_common.h>

#include "game.h"
#include "chess.h"
#include "net.h"

extern struct chess_info game_info;

gboolean net_handle_input(GGZMod *mod)
{
	char op;
	char args[15];
	int a = 0;
	char *names;
	int fd = ggzmod_get_server_fd(mod);

	game_info.fd = fd;

	/* Get the opcode */
	if (ggz_read_char(fd, &op) < 0)
		return FALSE;

	switch (op) {
	case CHESS_MSG_SEAT:
		ggz_debug("main", "Received MSG_SEAT");
		/* args[0] holds the seat */
		ggz_read_char(fd, &args[0]);
		/* args[1] holds version */
		ggz_read_char(fd, &args[1]);
		/* Issue a EVENT_SEAT */
		game_update(CHESS_EVENT_SEAT, args);
		break;
	case CHESS_MSG_PLAYERS:
		ggz_debug("main", "Got an MSG_PLAYERS");
		names = (char *)malloc(40 * sizeof(char));
		memset(names, 0, 40 * sizeof(char));
		/* Get first code */
		ggz_read_char(fd, &names[0]);
		if (names[0] != GGZ_SEAT_OPEN)
			ggz_read_string(fd, names + 1, 17);
		/* Get second code */
		ggz_read_char(fd, &names[20]);
		if (names[20] != GGZ_SEAT_OPEN)
			ggz_read_string(fd, names + 21, 17);
		/* Issue an event */
		game_update(CHESS_EVENT_PLAYERS, names);
		break;
	case CHESS_REQ_TIME:
		ggz_debug("main", "Got an REQ_TIME");
		/* The server is requesting my time option */
		game_update(CHESS_EVENT_TIME_REQUEST, NULL);
		break;
	case CHESS_RSP_TIME:
		ggz_debug("main", "Got an RSP_TIME");
		/* The server is sending the time option */
		ggz_read_int(fd, &a);
		game_update(CHESS_EVENT_TIME_OPTION, &a);
		break;
	case CHESS_MSG_START:
		ggz_debug("main", "Got an MSG_START");
		/* We should start the game now */
		game_update(CHESS_EVENT_START, NULL);
		break;
	case CHESS_MSG_MOVE:
		ggz_debug("main", "Got an MSG_MOVE");
		/* We got an move! */
		ggz_read_string(fd, args, 6);
		/* Should we worry about time ? */
		if (game_info.clock_type != CHESS_CLOCK_NOCLOCK)
			ggz_read_int(fd, (gint *) args + 2);
		game_update(CHESS_EVENT_MOVE, args);
		break;
	case CHESS_MSG_GAMEOVER:
		/* The game is over */
		ggz_debug("main", "Got a MSG_GAMEOVER");
		ggz_read_char(fd, &args[0]);
		ggz_debug("main", "Gameover msg: %d", args[0]);
		game_update(CHESS_EVENT_GAMEOVER, args);
		break;
	case CHESS_REQ_DRAW:
		/* Do you want to draw the game ? */
		ggz_debug("main", "Got a REQ_DRAW");
		game_update(CHESS_EVENT_DRAW, NULL);
		break;
	case CHESS_RSP_UPDATE:
		/* We want to update the seconds */
		ggz_debug("main", "Got an RSP_UPDATE");
		ggz_read_int(fd, (int *)args);
		ggz_read_int(fd, (int *)args + 1);
		game_update(CHESS_EVENT_UPDATE_TIME, args);
		break;
	default:
		game_message("Unknown opcode: %d\n", op);
		ggz_error_msg("Unknown opcode: %d", op);
		break;
	}

	return TRUE;
}

void net_send_time(int time_option)
{
	ggz_write_char(game_info.fd, CHESS_RSP_TIME);
	ggz_write_int(game_info.fd, time_option);
}

void net_send_move(char *move, int time)
{
	ggz_write_char(game_info.fd, CHESS_REQ_MOVE);
	ggz_write_string(game_info.fd, move);
	if (time >= 0)
		ggz_write_int(game_info.fd, time);
	/*
	   ggz_write_char(game_info.fd, from);
	   ggz_write_char(game_info.fd, to);
	 */
}

void net_send_draw(void)
{
	ggz_write_char(game_info.fd, CHESS_REQ_DRAW);
}

void net_call_flag(void)
{
	ggz_write_char(game_info.fd, CHESS_REQ_FLAG);
}

void net_request_update(void)
{
	ggz_write_char(game_info.fd, CHESS_REQ_UPDATE);
}

void net_update_server(int time)
{
	ggz_write_char(game_info.fd, CHESS_MSG_UPDATE);
	ggz_write_int(game_info.fd, time);
}
