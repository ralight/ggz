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
	int status = 0;
	ggz_debug("Sending bid to server: index %i.", bid);
	if (es_write_int(game.fd, WH_RSP_BID) < 0 ||
	    es_write_int(game.fd, bid) < 0)
		status = -1;
	set_game_state( WH_STATE_WAIT ); /* return to waiting */

	statusbar_message( _("Sending bid to server") );

	assert(status == 0);
}


void game_send_options(int options_cnt, int* options)
{
	int i, status = 0;
	ggz_debug("Sending options to server.");

	es_write_int(game.fd, WH_RSP_OPTIONS);
	for (i=0; i<options_cnt; i++)
		if (es_write_int(game.fd, options[i]) < 0)
			status = -1;
	set_game_state( WH_STATE_WAIT ); /* return to waiting */

	statusbar_message( _("Sending options to server") );

	assert(status == 0);
}


void game_play_card(card_t card)
{
	int status = 0;

	ggz_debug("Sending play to server: %i %i %i.", card.face, card.suit, card.deck);
	if (es_write_int(game.fd, WH_RSP_PLAY) < 0 ||
	    es_write_card(game.fd, card) < 0)
		status = -1;

	statusbar_message( _("Sending play to server") );

	assert(status == 0);
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
	char buf[512];
	va_list ap;

	va_start(ap, fmt);
        vsnprintf(buf, sizeof(buf), fmt, ap);
	fprintf(stderr, "DEBUG: %s\n", buf);
	va_end(ap);
#endif /* DEBUG */
}

int ggz_snprintf(char* buf, int buf_sz, char *fmt, ...)
{
	int result;
	va_list ap;

	assert( buf );
	assert( buf_sz > 0 );
	assert( fmt );

	va_start(ap, fmt);
	result = vsnprintf(buf, buf_sz, fmt, ap);
	va_end(ap);

	if (result == -1 || result >= buf_sz) {
		/* on failure, snprintf will return either -1 or the number of
		 * butes that would have been written */
		ggz_debug("Buffer overrun in snprintf.  String is %s.", buf);
		return buf_sz-1;
	}
	return result;
}
