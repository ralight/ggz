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
	/* Seed the random number generator */
	srandom((unsigned)time(NULL));

	game_init();

	/* FIXME: game_handle_ggz is useless; it just calls
	 * another function right away.  That other function
	 * should be the handler instead. --JDS */
	ggzdmod_set_handler(GGZ_EVENT_LAUNCH, &game_handle_ggz);
	ggzdmod_set_handler(GGZ_EVENT_JOIN, &game_handle_ggz);
	ggzdmod_set_handler(GGZ_EVENT_LEAVE, &game_handle_ggz);
	ggzdmod_set_handler(GGZ_EVENT_QUIT, &game_handle_ggz);
	ggzdmod_set_handler(GGZ_EVENT_PLAYER, &game_handle_player);

	if (ggzdmod_main() < 0)
		return -1;

	return 0;
}
