/*
 * File: seats.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/3/00
 * Desc: Support functions for table seats
 * $Id: seats.c 5901 2004-02-11 03:19:44Z jdorje $
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

#ifdef UNLIMITED_SEATS
	i = table->num_seats;

#  ifdef DEBUG
	for (i = 0; i < table->num_seats; i++)
		if (table->seat_types[i] == GGZ_SEAT_NONE) {
			err_msg("Table has seat %d with type NONE.", i);
			break;
		}
#  endif
#else
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		if (table->seat_types[i] == GGZ_SEAT_NONE)
			break;
#endif

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
#ifdef UNLIMITED_SPECTATORS
	return table->max_num_spectators;
#else
	int allow_spectators;

	pthread_rwlock_rdlock(&game_types[table->type].lock);
	allow_spectators = game_types[table->type].allow_spectators;
	pthread_rwlock_unlock(&game_types[table->type].lock);

	return allow_spectators ? MAX_TABLE_SPECTATORS : 0;
#endif
}


GGZSeatType seats_type(GGZTable* table, int seat)
{
	if (seat < 0 || seat >= seats_num(table))
		return GGZ_SEAT_NONE;
	return table->seat_types[seat];
}
