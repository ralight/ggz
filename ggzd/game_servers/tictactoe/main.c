/*
 * File: main.c
 * Author: Brent Hendricks
 * Project: GGZ Tic-Tac-Toe game module
 * Date: 3/35/00
 * Desc: Main loop
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


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "ggz.h"


int main(void)
{
	int ggz_sock, fd_max, status;
	char game_over = 0;
	fd_set active_fd_set, read_fd_set;
	
	
	if ( (ggz_sock = ggz_connect()) < 0)
		return -1;
	
	FD_ZERO(&active_fd_set);
	FD_SET(ggz_sock, &active_fd_set);

	fd_max = ggz_sock;
		
	while(!game_over) {

		read_fd_set = active_fd_set;
		status = select(fd_max, &read_fd_set, NULL, NULL, NULL);
		
		if (status <= 0) {
			if (errno == EINTR)
				continue;
			else
				return -1;
		}

		/* Check for message from player */
		if (FD_ISSET(ggz_sock, &read_fd_set)) {
			ggz_handle_msg(ggz_sock);
		}
	}


	return 0;
}




