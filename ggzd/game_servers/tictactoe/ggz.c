/*
 * File: ggz.c
 * Author: Brent Hendricks
 * Project: GGZ 
 * Date: 3/35/00
 * Desc: GGZ game module functions
 *
 * Copyright (C) 2000 Brent Hendricks.
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


#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <easysock.h>

#include "ggz.h"
#include "protocols.h"

/* Connect to Unix domain socket */
int ggz_connect(void)
{
	int sock;
	char fd_name[21];
	struct sockaddr_un addr;
	
	if ( (sock = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0)
		return -1;
	
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	sprintf(fd_name, "/tmp/TacTacToe.%d", getpid());
	strcpy(addr.sun_path, fd_name);

	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		return -1;
	
	return sock;
}


int ggz_handle_msg(int fd)
{
	int op, status;

	if (es_read_int(fd, &op) < 0)
		return -1;

	switch (op) {

	case REQ_GAME_LAUNCH:
		status = ggz_game_launch(fd);
		break;
	case REQ_GAME_JOIN:
		status = ggz_game_join(fd);
		break;
	case REQ_GAME_LEAVE:
		status = ggz_game_leave(fd);
		break;
	default:
		/* Unrecognized opcode */
		status = -1;
		break;
	}

	return status;
}


int ggz_game_launch(int fd)
{

	return 0;
}


int ggz_game_join(int fd)
{

	return 0;
}


int ggz_game_leave(int fd)
{

	return 0;
}
