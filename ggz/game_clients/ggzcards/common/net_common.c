/*
 * File: net_common.c
 * Author: Jason Short
 * Project: GGZCards Server/Client
 * Date: 04/16/2002
 * Desc: GGZCards network common code
 * $Id: net_common.c 6293 2004-11-07 05:51:47Z jdorje $
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

int read_card(int fd, card_t * card)
{
	if (ggz_read_char(fd, &card->face) < 0 ||
	    ggz_read_char(fd, &card->suit) < 0 ||
	    ggz_read_char(fd, &card->deck) < 0)
		return -1;

	/* We go ahead and check the card for validity. */
	if (is_valid_card(*card))
		return 0;

#if 0	/* This could be dangerous - anyone could crash us! */
	assert(FALSE);
#endif

#if 0	/* This probably makes the most sense, but... */
	return -1;
#endif

	*card = UNKNOWN_CARD;
	return 0;
}

int write_card(int fd, card_t card)
{
	/* Check for validity. */
	assert(is_valid_card(card));

	if (ggz_write_char(fd, card.face) < 0 ||
	    ggz_write_char(fd, card.suit) < 0 ||
	    ggz_write_char(fd, card.deck) < 0)
		return -1;
	return 0;
}

int read_bid(int fd, bid_t * bid)
{
	if (ggz_read_char(fd, &bid->sbid.val) < 0 ||
	    ggz_read_char(fd, &bid->sbid.suit) < 0 ||
	    ggz_read_char(fd, &bid->sbid.spec) < 0 ||
	    ggz_read_char(fd, &bid->sbid.spec2) < 0)
		return -1;
	return 0;
}

/** @brief Writes a bid to the socket.
 *  @param fd The file descriptor to which to read.
 *  @param bid A pointer to the bid data.
 *  @return 0 on success, -1 on failure. */
int write_bid(int fd, bid_t bid)
{
	if (ggz_write_char(fd, bid.sbid.val) < 0 ||
	    ggz_write_char(fd, bid.sbid.suit) < 0 ||
	    ggz_write_char(fd, bid.sbid.spec) < 0 ||
	    ggz_write_char(fd, bid.sbid.spec2) < 0)
		return -1;
	return 0;
}

int read_opcode(int fd, int *opcode)
{
	char op;
	if (ggz_read_char(fd, &op) < 0)
		return -1;
	*opcode = op;
	return 0;
}

int write_opcode(int fd, int opcode)
{
	char op = opcode;
	assert(opcode >= 0 && opcode < 128);
	return ggz_write_char(fd, op);
}

int read_seat(int fd, int *seat)
{
	char s;
	if (ggz_read_char(fd, &s) < 0)
		return -1;
	*seat = s;
	return 0;
}

int write_seat(int fd, int seat)
{
	char s = seat;
	assert(seat >= 0 && seat < 127);
	return ggz_write_char(fd, s);
}
