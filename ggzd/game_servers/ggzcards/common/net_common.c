/*
 * File: net_common.c
 * Author: Jason Short
 * Project: GGZCards Server/Client
 * Date: 04/16/2002
 * Desc: GGZCards network common code
 * $Id: net_common.c 8427 2006-07-31 22:50:50Z jdorje $
 *
 * Contains common networking functions.
 *
 * It should be identical between the server and all clients, and
 * the server contains the master copy.
 *
 * Copyright (C) 2002 GGZ Development Team.
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
#  include <config.h>	/* Site-specific config */
#endif

#include <assert.h>

#include <ggz.h>	/* for easysock */

#include "net_common.h"

void read_card(GGZDataIO * dio, card_t * card)
{
	ggz_dio_get_char(dio, &card->face);
	ggz_dio_get_char(dio, &card->suit);
	ggz_dio_get_char(dio, &card->deck);

	/* We go ahead and check the card for validity. */
	if (is_valid_card(*card))
		return;

#if 0	/* This could be dangerous - anyone could crash us! */
	assert(FALSE);
#endif

#if 0	/* This probably makes the most sense, but... */
	return;
#endif

	*card = UNKNOWN_CARD;
}

void write_card(GGZDataIO * dio, card_t card)
{
	/* Check for validity. */
	assert(is_valid_card(card));

	ggz_dio_put_char(dio, card.face);
	ggz_dio_put_char(dio, card.suit);
	ggz_dio_put_char(dio, card.deck);
}

void read_bid(GGZDataIO * dio, bid_t * bid)
{
	ggz_dio_get_char(dio, &bid->sbid.val);
	ggz_dio_get_char(dio, &bid->sbid.suit);
	ggz_dio_get_char(dio, &bid->sbid.spec);
	ggz_dio_get_char(dio, &bid->sbid.spec2);
}

/** @brief Writes a bid to the socket.
 *  @param dio The file descriptor to which to read.
 *  @param bid A pointer to the bid data. */
void write_bid(GGZDataIO * dio, bid_t bid)
{
	ggz_dio_put_char(dio, bid.sbid.val);
	ggz_dio_put_char(dio, bid.sbid.suit);
	ggz_dio_put_char(dio, bid.sbid.spec);
	ggz_dio_put_char(dio, bid.sbid.spec2);
}

void read_opcode(GGZDataIO * dio, int *opcode)
{
	char op;

	ggz_dio_get_char(dio, &op);
	*opcode = op;
}

void write_opcode(GGZDataIO * dio, int opcode)
{
	char op = opcode;

	assert(opcode >= 0 && opcode < 128);
	ggz_dio_put_char(dio, op);
}

void read_seat(GGZDataIO * dio, int *seat)
{
	char s;

	ggz_dio_get_char(dio, &s);
	*seat = s;
}

void write_seat(GGZDataIO * dio, int seat)
{
	char s = seat;

	assert(seat >= 0 && seat < 127);
	ggz_dio_put_char(dio, s);
}
