/*
 * File: main.c
 * Author: Brent Hendricks
 * Project: GGZ Chinese Checkers game module
 * Date: 04/27/2000
 * Desc: Main loop
 * $Id: main.c 5034 2002-10-25 23:56:41Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"

int main(void)
{
	GGZdMod *ggz = ggzdmod_new(GGZDMOD_GAME);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_STATE, &game_handle_ggz_state);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_JOIN, &game_handle_ggz_seat);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_LEAVE, &game_handle_ggz_seat);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_SEAT, &game_handle_ggz_seat);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_PLAYER_DATA, &game_handle_player);
	
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
