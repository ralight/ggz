/*
 * File: seats.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/3/00
 * Desc: Support functions for table seats
 * $Id: seats.c 10067 2008-06-24 22:01:07Z jdorje $
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

#ifdef HAVE_CONFIG_H
# include <config.h>		/* Site specific config */
#endif

#include <string.h>

#include <ggz.h>

#include "datatypes.h"
#include "err_func.h"
#include "ggzd.h"
#include "ggzdmod.h"
#include "seats.h"
#include "table.h"

int seats_count(GGZTable* table, GGZSeatType type)
{
	int i, count = 0;
	int max = seats_num(table);

	for (i = 0; i < max; i++)
		if (table->seat_types[i] == type)
			count++;
	return count;
}


int seats_num(GGZTable* table)
{
	int i;

	i = table->num_seats;

#  ifdef DEBUG
	for (i = 0; i < table->num_seats; i++)
		if (table->seat_types[i] == GGZ_SEAT_NONE) {
			ggz_error_msg("Table has seat %d with type NONE.", i);
			break;
		}
#  endif

	return i;
}


int spectators_count(GGZTable* table)
{
	int i, count = 0;
	int max = spectator_seats_num(table);

	for (i = 0; i < max; i++)
		if (table->spectators[i][0] != '\0')
			count++;
	return count;
}


int spectator_seats_num(GGZTable *table)
{
	return table->max_num_spectators;
}


GGZSeatType seats_type(GGZTable* table, int seat)
{
	if (seat < 0 || seat >= seats_num(table))
		return GGZ_SEAT_NONE;
	return table->seat_types[seat];
}
