/*
 * File: main.c
 * Author: Brent Hendricks
 * Project: GGZ Escape game module
 * Date: 22/06/2001
 * Desc: Main loop
 * $Id: main.c 2649 2001-11-04 17:33:57Z jdorje $
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

#include "../../ggzdmod/ggz_server.h"

#include "game.h"

int main(void)
{
	/* Seed the random number generator */
	srandom((unsigned)time(NULL));

	game_init();

	ggzd_set_handler(GGZ_EVENT_LAUNCH, &ggz_update);
	ggzd_set_handler(GGZ_EVENT_JOIN, &ggz_update);
	ggzd_set_handler(GGZ_EVENT_LEAVE, &ggz_update);
	/* ggzd_set_handler(GGZ_EVENT_QUIT, &ggz_update); */
	ggzd_set_handler(GGZ_EVENT_PLAYER, &game_handle_player);

	(void)ggzd_main_loop();

	return 0;
}

