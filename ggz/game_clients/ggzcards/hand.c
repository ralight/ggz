/*
 * File: hand.c
 * Author: Rich Gade
 * Project: GGZCards Client
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
#include <stdio.h>

#include <easysock.h>

#include "game.h"
#include "table.h"
#include "hand.h"

/* hand_read_hand()
 *   Sets the cards in any player's hand based on a server message
 */
int hand_read_hand(void)
{
	int i;
	int p;
	struct hand_t *hand;

	/* first read the player whose hand it is */
	if (es_read_int(game.fd, &p) < 0)
		return -1;
	hand = &game.players[p].hand;

	ggz_debug("     Reading the hand of player %d.", p);

	/* Zap our hand */
	/* TODO: fixme */
	for(i=0; i<MAX_HAND_SIZE; i++)
		hand->card[i] = UNKNOWN_CARD;
	hand->selected_card = -1; /* index into the array */

	/* First find out how many cards in this hand */
	if(es_read_int(game.fd, &hand->hand_size) < 0)
		return -1;

	if (hand->hand_size > MAX_HAND_SIZE) {
		ggz_debug("CLIENT BUG: can't handle hands with more than %d cards.", MAX_HAND_SIZE);
		return -1;
	}

	ggz_debug("     Read hand_size as %d.", game.players[p].hand.hand_size);

	/* Read in all the card values */
	for(i=0; i<hand->hand_size; i++)
		if(es_read_card(game.fd, &hand->card[i]) < 0)
			return -1;

	table_display_hand(p);

	return 0;
}


/* none of this should go here, but that's okay for now */
#include "easysock.h"

int es_read_card(int fd, card_t *card)
{
 	if (es_read_char(fd, &card->face) < 0) return -1;
	if (es_read_char(fd, &card->suit) < 0) return -1;
	if (es_read_char(fd, &card->deck) < 0) return -1;
	return 0;
}

int es_write_card(int fd, card_t card)
{
 	if (es_write_char(fd, card.face) < 0) return -1;
	if (es_write_char(fd, card.suit) < 0) return -1;
	if (es_write_char(fd, card.deck) < 0) return -1;
	return 0;
}

