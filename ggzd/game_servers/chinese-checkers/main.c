/*
 * File: main.c
 * Author: Brent Hendricks
 * Project: GGZ Chinese Checkers game module
 * Date: 04/27/2000
 * Desc: Main loop
 * $Id: main.c 2807 2001-12-08 21:14:29Z jdorje $
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

#include "game.h"

int main(void)
{
	GGZdMod *ggz = ggzdmod_new(GGZDMOD_GAME);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_STATE, &game_handle_ggz_state);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_JOIN, &game_handle_ggz_join);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_LEAVE, &game_handle_ggz_leave);
	ggzdmod_set_handler(ggz, GGZDMOD_EVENT_PLAYER_DATA, &game_handle_player);
	
	/* Seed the random number generator */
	srandom((unsigned)time(NULL));
	game_init(ggz);

	(void)ggzdmod_connect(ggz);
	(void)ggzdmod_loop(ggz);
	(void)ggzdmod_disconnect(ggz);
	ggzdmod_free(ggz);

	return 0;
}
