/*
 * File: main.c
 * Author: Ismael Orenstein
 * Project: GGZ Reversi game module
 * Date: 09/17/2000
 * Desc: main loop for the server
 * $Id: main.c 2804 2001-12-07 23:11:21Z jdorje $
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

#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

int main(void) {
	GGZdMod *ggzdmod = ggzdmod_new(GGZDMOD_GAME);

	/* game_init is called at the start of _each_ game, so we must do
	   ggz stuff here. */
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_STATE, &game_handle_ggz_state);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_JOIN, &game_handle_ggz_join);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_LEAVE, &game_handle_ggz_leave);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_PLAYER_DATA, &game_handle_player);
	
	game_init(ggzdmod);
	
	(void)ggzdmod_connect(ggzdmod);
	(void)ggzdmod_loop(ggzdmod);
	(void)ggzdmod_disconnect(ggzdmod);
	ggzdmod_free(ggzdmod);

	return 0;
}
