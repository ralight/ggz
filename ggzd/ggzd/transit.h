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

/* Table transit values */
typedef enum {
	GGZ_TRANSIT_JOIN,
	GGZ_TRANSIT_LEAVE,
	GGZ_TRANSIT_SEAT,
	GGZ_TRANSIT_JOIN_SPECTATOR,
	GGZ_TRANSIT_LEAVE_SPECTATOR,
	GGZ_TRANSIT_SIT,
	GGZ_TRANSIT_STAND,
	GGZ_TRANSIT_MOVE
} GGZTransitType;

#if 0
GGZReturn transit_table_event(int room, int index, char opcode,
			      const char *name);
#endif

GGZReturn transit_seat_event(int room_index, int table_id,
			     GGZTransitType transit,
			     struct GGZTableSeat seat, const char *caller,
			     int reason);

GGZReturn transit_player_event(const char* name, GGZTransitType opcode,
			       GGZClientReqError status,
			       const char *caller, int reason, int index);
