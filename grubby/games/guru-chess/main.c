/*
 * Chess game module for Guru
 * Copyright (C) 2004 Josef Spillner, josef@ggzgamingzone.org
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <ggz.h>
#include <ggz_common.h>

#include "ggzpassive.h"

#include "proto.h"

static void chess_ai(void)
{
	char opcode;
	int fd;
	char seat, version;
	char seats[2], names[2][32];

	fd = ggz_gamefd;

	ggz_read_char(fd, &opcode);

	switch(opcode)
	{
		case CHESS_MSG_SEAT:
			ggz_read_char(fd, &seat);
			ggz_read_char(fd, &version);
			printf("seat=%i version=%i\n", seat, version);
			break;
		case CHESS_MSG_PLAYERS:
			ggz_read_char(fd, &seats[0]);
			if(seats[0] != GGZ_SEAT_OPEN) ggz_read_string(fd, names[0], sizeof(names[0]));
			else names[0][0] = 0;
			ggz_read_char(fd, &seats[1]);
			if(seats[1] != GGZ_SEAT_OPEN) ggz_read_string(fd, names[1], sizeof(names[1]));
			else names[1][0] = 0;
			printf("player1=%s player2=%s\n", names[0], names[1]);
			break;
		case CHESS_REQ_TIME:
			/* not needed */
			break;
		case CHESS_RSP_TIME:
			break;
		case CHESS_MSG_START:
			break;
		case CHESS_REQ_MOVE:
			/* server */
			break;
		case CHESS_MSG_MOVE:
			break;
		case CHESS_MSG_GAMEOVER:
			break;
		case CHESS_REQ_UPDATE:
			/* server */
			break;
		case CHESS_RSP_UPDATE:
			break;
		case CHESS_MSG_UPDATE:
			/* server */
			break;
		case CHESS_REQ_FLAG:
			/* server */
			break;
		case CHESS_REQ_DRAW:
			break;
		default:
			/* unknown opcode */
			printf("unknown opcode %i\n", opcode);
			break;
	}
}

static void chess_init()
{
}

int main(int argc, char *argv[])
{
	chess_init();

	ggzpassive_sethandler(&chess_ai);
	ggzpassive_start();

	return 0;
}

