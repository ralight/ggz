/* 
 * File: protocol.c
 * Author: Jason Short
 * Project: GGZCards Server/Client
 * Date: 06/26/2001
 * Desc: Enumerations for the ggzcards client-server protocol
 * $Id: protocol.c 2418 2001-09-09 03:42:21Z jdorje $
 *
 * This just contains the communications protocol information.
 *
 * It should be identical between the server and all clients, and
 * the server contains the master copy.
 *
 * Copyright (C) 1999 Brent Hendricks.
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

/* NOTE: This file should only be changed in the server. */

#ifdef HAVE_CONFIG
#  include <config.h>		/* Site-specific config */
#endif

#include <assert.h>

#include <easysock.h>

#include "protocol.h"

int read_card(int fd, card_t * card)
{
	if (es_read_char(fd, &card->face) < 0 ||
	    es_read_char(fd, &card->suit) < 0 ||
	    es_read_char(fd, &card->deck) < 0)
		return -1;
	return 0;
}

int write_card(int fd, card_t card)
{
	if (es_write_char(fd, card.face) < 0 ||
	    es_write_char(fd, card.suit) < 0 ||
	    es_write_char(fd, card.deck) < 0)
		return -1;
	return 0;
}

int read_opcode(int fd, int *opcode)
{
	char op;
	if (es_read_char(fd, &op) < 0)
		return -1;
	*opcode = op;
	return 0;
}

int write_opcode(int fd, int opcode)
{
	char op = opcode;
	assert(opcode >= 0 && opcode < 128);
	return es_write_char(fd, op);
}

int read_seat(int fd, int *seat)
{
	char s;
	if (es_read_char(fd, &s) < 0)
		return -1;
	*seat = s;
	return 0;
}

int write_seat(int fd, int seat)
{
	char s = seat;
	assert(seat >= 0 && seat < 127);
	return es_write_char(fd, s);
}
