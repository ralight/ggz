/*
 * File: main.c
 * Author: Josef Spillner
 * Original Author: Brent Hendricks
 * Project: GGZ Hastings1066 game module
 * Date: 2001-01-08
 * Desc: Main loop
 * $Id: main.c 2824 2001-12-09 10:00:41Z jdorje $
 *
 * Copyright (C) Josef Spillner
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

/* System includes */
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "game.h"

int main(void)
{
	GGZdMod *ggz = ggzdmod_new(GGZDMOD_GAME);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_STATE, &game_handle_ggz);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_JOIN, &game_handle_ggz);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_LEAVE, &game_handle_ggz);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_PLAYER_DATA, &game_handle_player);

	game_init(ggz);
	
	if (ggzdmod_connect(ggz) < 0) {
		fprintf(stderr, "Couldn't connect to GGZ!\n");
		return -1;
	}
	(void) ggzdmod_loop(ggz);
	(void) ggzdmod_disconnect(ggz);
	ggzdmod_free(ggz);

	return 0;
}
