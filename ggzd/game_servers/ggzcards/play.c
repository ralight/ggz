/*
 * File: play.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 02/21/2002
 * Desc: Functions and data for playing system
 * $Id: play.c 3437 2002-02-21 10:05:18Z jdorje $
 *
 * Copyright (C) 2001-2002 Brent Hendricks.
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-130
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include "common.h"
#include "net.h"
#include "play.h"

void req_play(player_t p, seat_t s)
{
	/* although the game_* functions probably track this data themselves,
	   we track it here as well just in case. */
	game.players[p].is_playing = TRUE;
	game.players[p].play_seat = s;

	set_game_state(STATE_WAIT_FOR_PLAY);
	set_player_message(p);
	
	(void) send_play_request(p, s);
}
