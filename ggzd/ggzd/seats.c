/*
 * File: seats.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/3/00
 * Desc: Support functions for table seats
 * $Id: seats.c 4508 2002-09-11 03:48:41Z jdorje $
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

#include "datatypes.h"
#include "ggzd.h"
#include "ggzdmod.h"
#include "table.h"
#include "seats.h"

extern struct GameInfo game_types[MAX_GAME_TYPES];

int seats_count(GGZTable* table, GGZSeatType type)
{
	int i, count = 0;
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		if (table->seat_types[i] == type)
			count++;
	return count;
}


int seats_num(GGZTable* table)
{
	int i;
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		if (table->seat_types[i] == GGZ_SEAT_NONE)
			break;
	return i;
}


int spectators_count(GGZTable* table)
{
	int i, count = 0;
	for (i = 0; i < table->max_num_spectators; i++)
		if (!table->spectators[i][0])
			count++;
	return count;
}


int spectator_seats_num(GGZTable *table)
{
	return table->max_num_spectators;
}


GGZSeatType seats_type(GGZTable* table, int seat)
{
	return table->seat_types[seat];
}
