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

#include "common.h"

#include "game.h"
#include "table.h"
#include "hand.h"
#include "layout.h"

/* hand_read_hand()
 *   Sets the cards in any player's hand based on a server message
 */
int hand_read_hand(void)
{
	int i, player;
	struct hand_t *hand;

	assert(game.players);

	/* first read the player whose hand it is */
	if (es_read_int(ggzfd, &player) < 0)
		return -1;
	assert(player >= 0 && player < game.num_players);
	hand = &game.players[player].hand;

	client_debug("     Reading the hand of player %d.", player);

	/* Zap our hand */
	/* TODO: fixme */
	for(i=0; i<game.max_hand_size; i++)
		hand->card[i] = UNKNOWN_CARD;
	hand->selected_card = -1; /* index into the array */

	/* First find out how many cards in this hand */
	if(es_read_int(ggzfd, &hand->hand_size) < 0)
		return -1;

	if (hand->hand_size > game.max_hand_size) {
		int p;
		client_debug("Expanding max_hand_size to allow for %d cards (previously max was %d).", hand->hand_size, game.max_hand_size);
		game.max_hand_size = hand->hand_size;
		while (1) {
			/* the inner table must be at least large enough.
			 * So, if it's not we make the hand sizes larger. */
			int x, y, w, h, w1, h1;
			get_table_dim(&x, &y, &w, &h);
			get_fulltable_size(&w1, &h1);
			if (w1 > w && h1 > h)
				break;
			client_debug("Increasing max hand size because the available table size (%d %d) isn't as big as what's required (%d %d).", w, h, w1, h1);
			game.max_hand_size++;
		}
		for (p = 0; p<game.num_players; p++) {
			/* TODO: figure out how this code could even fail at all.
			   In the meantime, I've disabled the call to free, conceding
			   the memory leak so that we don't have an unexplained seg fault
			   (which we would have if these two lines were included) */
/*
			if (game.players[p].hand.card != NULL)
				g_free(game.players[p].hand.card);
*/
			game.players[p].hand.card = (card_t *)g_malloc(game.max_hand_size * sizeof(card_t));
		}
		table_setup();
	}

	client_debug("     Read hand_size as %d.", game.players[player].hand.hand_size);

	/* Read in all the card values */
	for(i=0; i<hand->hand_size; i++)
		if(es_read_card(ggzfd, &hand->card[i]) < 0)
			return -1;

	table_display_hand(player);

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

