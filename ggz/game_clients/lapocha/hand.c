/*
 * File: hand.c
 * Author: Rich Gade
 * Project: GGZ La Pocha Client
 * Date: 08/14/2000
 * Desc: Functions to get cards from server
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

#include <easysock.h>

#include "game.h"
#include "table.h"
#include "hand.h"

struct hand_t hand;

/* hand_read_hand()
 *   Sets the cards in our hand based on a server message
 */
int hand_read_hand(void)
{
	int i;

	/* Zap our hand */
	for(i=0; i<10; i++)
		hand.card[i] = -1;
	hand.selected_card = -1;

	/* First find out how many cards in this hand */
	if(es_read_char(game.fd, &hand.hand_size) < 0)
		return -1;

	/* Read in all the card values */
	for(i=0; i<hand.hand_size; i++)
		if(es_read_char(game.fd, &hand.card[i]) < 0)
			return -1;

	/* Everyone got dealt this many cards */
	for(i=0; i<4; i++)
		game.num_cards[i] = hand.hand_size;

	table_display_hand();

	return 0;
}
