/*
 * File: seats.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/3/00
 * Desc: Support functions for table seats
 * $Id: seats.c 3185 2002-01-24 10:59:56Z jdorje $
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

#include <config.h>		/* Site specific config */

#include <string.h>

#include <ggzd.h>
#include <ggzdmod.h>
#include <table.h>
#include <seats.h>

/* FIXME: most of this file could simply be removed.  --JDS */

static int count_seats(GGZTable* table, GGZSeatType type)
{
	int i, count = 0;
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		if (table->seat_types[i] == type)
			count++;
	return count;
}


int seats_open(GGZTable* table)
{
	return count_seats(table, GGZ_SEAT_OPEN);
}


int seats_num(GGZTable* table)
{
	int i;
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		if (table->seat_types[i] == GGZ_SEAT_NONE)
			break;
	return i;
}


int seats_bot(GGZTable* table)
{
	return count_seats(table, GGZ_SEAT_BOT);
}


int seats_reserved(GGZTable* table)
{
	return count_seats(table, GGZ_SEAT_RESERVED);
}


int seats_human(GGZTable* table)
{
	return count_seats(table, GGZ_SEAT_PLAYER);
}


GGZSeatType seats_type(GGZTable* table, int seat)
{
	return table->seat_types[seat];
}
