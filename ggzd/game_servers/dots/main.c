/*
 * File: main.c
 * Author: Brent Hendricks
 * Project: GGZ Connect The Dots game module
 * Date: 04/27/2000
 * Desc: Main loop
 * $Id: main.c 2922 2001-12-17 22:27:22Z jdorje $
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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "game.h"

int main(void)
{
	/* Init ggz */
	GGZdMod *ggz = ggzdmod_new(GGZDMOD_GAME);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_STATE, &game_handle_ggz_state);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_JOIN, &game_handle_ggz_join);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_LEAVE, &game_handle_ggz_leave);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_PLAYER_DATA, &game_handle_player_data);
	
	/* Seed the random number generator */
	srandom((unsigned)time(NULL));
	game_init(ggz);

	/* Connect to GGZ server; main loop */
	if (ggzdmod_connect(ggz) < 0)
		return -1;
	(void) ggzdmod_loop(ggz);
	(void) ggzdmod_disconnect(ggz);
	ggzdmod_free(ggz);

	return 0;
}
