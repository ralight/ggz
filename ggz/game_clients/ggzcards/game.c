/* 
 * File: game.c
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Handles user-interaction with game screen
 * $Id: game.c 2383 2001-09-07 08:25:36Z jdorje $
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
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>

#include "common.h"

#include "main.h"
#include "game.h"
#include "easysock.h"


/* game_send_bid the "bid" is the index into the list of choices sent to us
   by server */
void game_send_bid(int bid)
{
	int status = client_send_bid(bid);

	statusbar_message(_("Sending bid to server"));

	assert(status == 0);
}


void game_play_card(card_t card)
{
	int status;

	status = client_send_play(card);

	statusbar_message(_("Sending play to server"));

	assert(status == 0);
}
