/*
 * File: ggz_client.h
 * Author: GGZ Dev Team
 * Project: GGZ Common Client Lib
 * Date: 01/13/2002
 * $Id: ggz_client.h 3107 2002-01-13 23:41:19Z jdorje $
 *
 * Interface file to be included client-side by both ggzcore (ggz client
 * library) and ggzmod (ggz game client library).
 *
 * Copyright (C) 2002 Brent Hendricks.
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

#ifndef __GGZ_CLIENT_H__
#define __GGZ_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @brief Seat status values.
 *
 * Each "seat" at a table of a GGZ game can have one of these values.
 * They are used by ggzdmod and the game server; their use in game
 * clients is completely optional.
 */
typedef enum {
	GGZ_SEAT_OPEN = -1,	   /**< The seat is open (unoccupied). */
	GGZ_SEAT_BOT = -2,	   /**< The seat has a bot (AI) in it. */
	GGZ_SEAT_RESERVED = -3,	   /**< The seat is reserved for a player. */
	GGZ_SEAT_NONE = -4,	   /**< This seat does not exist. */
	GGZ_SEAT_PLAYER = -5	   /**< The seat has a regular player in it. */
} GGZSeatType;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GGZ_CLIENT_H__ */
