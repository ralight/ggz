/*
 * File: main.c
 * Author: Brent Hendricks
 * Project: GGZ La Pocha game module
 * Date: 06/29/2000
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
#include <stdlib.h>
#include <time.h>

#include "../libggzmod/ggz_server.h"

#include "game.h"

int main(void)
{
	char game_over = 0;
	int i, fd, ggz_sock, fd_max, status;
	fd_set active_fd_set, read_fd_set;
	
	/* Initialize ggz */
	if (ggzdmod_init("LaPocha") < 0)
		return -1;
	
	if ( (ggz_sock = ggzdmod_connect()) < 0)
		return -1;

	/* Seed the random number generator */
	srandom((unsigned)time(NULL));
	
	FD_ZERO(&active_fd_set);
	FD_SET(ggz_sock, &active_fd_set);

	game_init();
	while(!game_over) {
		
		read_fd_set = active_fd_set;
		fd_max = ggzdmod_fd_max();
		
		status = select((fd_max+1), &read_fd_set, NULL, NULL, NULL);
		
		if (status <= 0) {
			if (errno == EINTR)
				continue;
			else
				return -1;
		}

		/* Check for message from GGZ server */
		if (FD_ISSET(ggz_sock, &read_fd_set)) {
			status = game_handle_ggz(ggz_sock, &fd);
			switch (status) {
				
			case -1:  /* Big error!! */
				return -1;
				
			case 0: /* All ok, how boring! */
				break;

			case 1: /* A player joined */
				FD_SET(fd, &active_fd_set);
				break;
				
			case 2: /* A player left */
				FD_CLR(fd, &active_fd_set);
				break;
				
			case 3: /*Safe to exit */
				game_over = 1;
				break;
			}
		}

		/* Check for message from player */
		for (i = 0; i < ggzdmod_seats_num(); i++) {
			fd = ggz_seats[i].fd;
			if (fd != -1 && FD_ISSET(fd, &read_fd_set)) {
				status = game_handle_player(i);
				if (status == -1)
					FD_CLR(fd, &active_fd_set);
			}
		}
	}

	ggzdmod_quit();
	return 0;
}
