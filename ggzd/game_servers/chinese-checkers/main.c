/*
 * File: main.c
 * Author: Brent Hendricks
 * Project: GGZ Chinese Checkers game module
 * Date: 04/27/2000
 * Desc: Main loop
 * $Id: main.c 2346 2001-09-03 10:43:41Z jdorje $
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
	/* Seed the random number generator */
	srandom((unsigned)time(NULL));

	game_init();

	ggzd_set_handler(GGZ_EVENT_LAUNCH, &game_handle_ggz);
	ggzd_set_handler(GGZ_EVENT_JOIN, &game_handle_ggz);
	ggzd_set_handler(GGZ_EVENT_LEAVE, &game_handle_ggz);
	ggzd_set_handler(GGZ_EVENT_PLAYER, &game_handle_player);
	(void)ggzd_main_loop();

	return 0;
}
