/*
 * File: ggz_common.h
 * Author: GGZ Dev Team
 * Project: GGZ Common Library
 * Date: 01/13/2002
 * $Id: ggz_common.h 3227 2002-02-03 19:19:15Z jdorje $
 *
 * This provides GGZ-specific functionality that is common to
 * some or all of the ggz-server, game-server, ggz-client, and
 * game-client.
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

#ifndef __GGZ_COMMON_H__
#define __GGZ_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @brief Seat status values.
 *
 * Each "seat" at a table of a GGZ game can have one of these values.
 * They are used by the GGZ client, GGZ server, and game servers; their
 * use in game clients is completely optional.
 */
typedef enum {
	GGZ_SEAT_NONE,		/**< This seat does not exist. */
	GGZ_SEAT_OPEN,		/**< The seat is open (unoccupied). */
	GGZ_SEAT_BOT,		/**< The seat has a bot (AI) in it. */
	GGZ_SEAT_PLAYER,	/**< The seat has a regular player in it. */
	GGZ_SEAT_RESERVED,	/**< The seat is reserved for a player. */
} GGZSeatType;

/** @brief Get a string identifier for the GGZSeatType.
 *
 *  This returns a pointer to a static string describing the given seat type.
 *  It is useful for text-based communications protocols and debugging
 *  output.
 *  @param type The GGZSeatType, which determines the string returned.
 *  @note This is the inverse of ggz_string_to_seattype.
 */
char *ggz_seattype_to_string(GGZSeatType type);

/** @brief Get a GGZSeatType for the given string identifier.
 *
 *  This returns a GGZSeatType that is associated with the given string
 *  description.
 *  @param type_str A string describing a GGZSeatType.
 *  @note If the type_str cannot be parsed GGZ_SEAT_NONE may be returned.
 *  @note This is the inverse of ggz_seattype_to_string.
 */
GGZSeatType ggz_string_to_seattype(const char *type_str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GGZ_COMMON_H__ */
