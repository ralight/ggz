/*
 * File: seats.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/3/00
 * Desc: Support functions for table seats
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


#ifndef __GGZ_TABLE_H_
#define __GGZ_TABLE_H_

#include "table.h"
#include <ggz_common.h>


/* Seat structure */
struct GGZTableSeat {

	/* Seat index */
	int index;

	/* Type of player in seat */
	GGZSeatType type;

	/* Player's name */
	char name[MAX_USER_NAME_LEN + 1];

	/* File descriptor */
	int fd;
};

#define GGZ_SEATNUM_ANY -1


/** @brief Count the number of seats of the given type at the table.
 *
 *  @param table The table to count seats at.
 *  @param type The type of seat to count.
 *  @return The number of seats that match the type.
 */
int seats_count(GGZTable* table, GGZSeatType type);

/** @brief Give the total number of seats at the table.
 *
 *  @param table The table we are querying.
 *  @return The number of seats at the table.
 */
int seats_num(GGZTable* table);

/** @brief Find the type of the given seat at the table.
 *
 *  @param table The table we are querying.
 *  @param seat The # of the seat we are querying.
s *  @return The type of the given seat.
 */
GGZSeatType seats_type(GGZTable* table, int seat);


#endif /* __GGZ_TABLE_H_ */
