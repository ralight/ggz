/*
 * File: game.c
 * Author: Rich Gade
 * Project: GGZ La Pocha Client
 * Date: 08/14/2000
 * Desc: Handles user-interaction with game screen
 * $Id: game.c 3174 2002-01-21 08:09:42Z jdorje $
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

#include <ggz.h>

#include "main.h"
#include "game.h"

struct game_t game;


void game_send_bid(int bid)
{
	ggz_write_int(game.fd, LP_SND_BID);
	ggz_write_char(game.fd, bid);

	game.bid[game.me] = bid;

	statusbar_message("Sending bid to server");
}


void game_play_card(int card)
{
	ggz_write_int(game.fd, LP_SND_PLAY);
	ggz_write_char(game.fd, (char)card);

	statusbar_message("Sending play to server");
}


void game_handle_table_click(char card)
{
	char trump;

	/* Do we care if they clicked a card? */
	if(game.state != LP_STATE_TRUMP)
		return;

	/* Since we drew the cards out of order to make them look
	 * better on the screen, switch 2 and 3 */
	if((trump = card) == 3)
		trump = 2;
	else if(trump == 2)
		trump = 3;

	/* Send out our choice of trump */
	ggz_write_int(game.fd, LP_SND_TRUMP);
	ggz_write_char(game.fd, trump);

	statusbar_message("Sending trump selection to server");
}
