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


#include <ggzdmod.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include "game.h"


int main(void)
{
	/* ggzdmod initializations */
	GGZdMod *ggzdmod = ggzdmod_new(GGZDMOD_GAME);
	game_init(ggzdmod);
	
	if (ggzdmod_connect(ggzdmod) < 0) {
		fprintf(stderr, "Could not connect to ggz server.\n");
		ggzdmod_free(ggzdmod);
		return -1;
	}
	
	(void)ggzdmod_log(ggzdmod, "Starting game of Tic-Tac-Toe");
	(void)ggzdmod_loop(ggzdmod);
	(void)ggzdmod_log(ggzdmod, "Ending game of Tic-Tac-Toe");
	(void)ggzdmod_disconnect(ggzdmod);
	ggzdmod_free(ggzdmod);
	
	return 0;
}




