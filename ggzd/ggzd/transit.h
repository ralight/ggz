/*
 * File: transit.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/26/00
 * Desc: Functions for handling table transits
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


#include "seats.h"

/* Table transit flag values */
#define GGZ_TRANSIT_JOIN   0x01  /* %0000 0001 */
#define GGZ_TRANSIT_LEAVE  0x02  /* %0000 0010 */
#define GGZ_TRANSIT_SEAT   0x04

int transit_table_event(int room, int index, char opcode, char* name);

int transit_seat_event(int room, int index, struct GGZTableSeat seat, char *caller);

int transit_player_event(char* name, char opcode, int status, int index);
