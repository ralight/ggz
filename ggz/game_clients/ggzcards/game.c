/*
 * File: game.c
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Handles user-interaction with game screen
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

#include "main.h"
#include "game.h"
#include "easysock.h"

struct game_t game = {0};


/* game_send_bid
 *   the "bid" is the index into the list of choices sent to us by server
 */
void game_send_bid(int bid)
{
	ggz_debug("Sending bid to server: index %i.", bid);
	es_write_int(game.fd, WH_RSP_BID);
	es_write_int(game.fd, bid);
	set_game_state( WH_STATE_WAIT ); /* return to waiting */

	statusbar_message( _("Sending bid to server") );
}


void game_send_options(int options_cnt, int* options)
{
	int i;
	ggz_debug("Sending options to server.");

	es_write_int(game.fd, WH_RSP_OPTIONS);
	for (i=0; i<options_cnt; i++)
		es_write_int(game.fd, options[i]);
	set_game_state( WH_STATE_WAIT ); /* return to waiting */

	statusbar_message( _("Sending options to server") );
}


void game_play_card(card_t card)
{
	ggz_debug("Sending play to server: %i %i %i.", card.face, card.suit, card.deck);
	es_write_int(game.fd, WH_RSP_PLAY);
	es_write_card(game.fd, card);

	statusbar_message( _("Sending play to server") );
}

static char* game_states[] = {"INIT", "WAIT", "PLAY", "BID", "ANIM", "DONE", "OPTIONS"};

void set_game_state(char state)
{
	if (state == game.state)
		ggz_debug("Staying in state %d.", game.state);
	else {
		ggz_debug("Changing state from %s to %s.", game_states[(int)game.state], game_states[(int)state]);
		game.state = state;
	}
}


void ggz_debug(const char *fmt, ...)
{
#ifdef DEBUG
	char buf[4096];
	va_list ap;

	va_start(ap, fmt);
        vsprintf(buf, fmt, ap);
	fprintf(stderr, "DEBUG: %s\n", buf);
	va_end(ap);
#endif /* DEBUG */
}


