/*
 * File: main.c
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Date: 09/17/2000
 * Desc: Game server main loop
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

#include "game.h"
#include "combat.h"
#include "ggz.h"

#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

int main(void) {
	int game_over = 0;
	int ggz_sock, fd = -1, status, i;
	fd_set active_fd_set, read_fd_set;

	// Initialize ggz
	if (ggz_init("Combat") < 0)
		return -1;

	if ( (ggz_sock = ggz_connect()) < 0)
		return -1;

	FD_ZERO(&active_fd_set);
	FD_SET(ggz_sock, &active_fd_set);

	// Initializes game variables
	game_init();

	while (!game_over) {

		read_fd_set = active_fd_set;

		status = select(ggz_fd_max()+1, &read_fd_set, NULL, NULL, NULL);

		if (status <= 0) {
			if (errno == EINTR)
				continue;
			else
				return -1;
		}

		// Check for messages from the GGZ server
		if (FD_ISSET(ggz_sock, &read_fd_set)) {
			status = game_handle_ggz(ggz_sock, &fd);
			switch (status) {
				case CBT_SERVER_ERROR:
					ggz_debug("Error! Check if the GGZ server is up and is running the same protocol that the game\n");
					return -1;
					break;
				case CBT_SERVER_OK:
					break;
				case CBT_SERVER_JOIN: // A player joined
					ggz_debug("Adding a player to the fd_set\n");
					FD_SET(fd, &active_fd_set);
					break;
				case CBT_SERVER_LEFT: // A player left
					ggz_debug("Removing a player from the fd_set\n");
					FD_CLR(fd, &active_fd_set);
					break;
				case CBT_SERVER_QUIT: // Quit
					ggz_debug("Good bye\n");
					break;
			}
		}

		// Check for messages from the players
		for (i = 0; i < ggz_seats_num(); i++) {
			fd = ggz_seats[i].fd;
			if (fd != -1 && FD_ISSET(fd, &read_fd_set)) {
				status = game_handle_player(i);
				if (status < 0) {
					ggz_debug("This player is crazy! Status: %d", status);
					//game_send_sync(i);
				}
			}
		}

	}

	ggz_debug("Quitting now\n");
	ggz_quit();
	return 0;
}
