/*
 * File: ggz_common.h
 * Author: GGZ Dev Team
 * Project: GGZ Common Library
 * Date: 01/13/2002
 * $Id: ggz_common.h 4507 2002-09-11 03:37:21Z jdorje $
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


/** @brief Table state values.
 *
 * GGZ tables take one of the following states. They are used by the
 * GGZ client and GGZ server.  This is *not* necessarily the same as
 * the game state kept by libggzdmod
 */
typedef enum {
	GGZ_TABLE_ERROR   = -1,  /**< There is some error with the table */
	GGZ_TABLE_CREATED = 0,   /**< Initial created state for the table. */
	GGZ_TABLE_WAITING = 1,   /**< Waiting for enough players to join before playing */
	GGZ_TABLE_PLAYING = 2,   /**< Playing a game */
	GGZ_TABLE_DONE    = 3    /**< The game session is finished and the table will soon exit */
} GGZTableState;
	

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


/** @brief Convert a string to a boolean.
 * 
 *  The string should contain "true" or "false".
 *  @param str The string in question.
 *  @param dflt The default, if the string is unreadible or NULL.
 *  @return The boolean value.
 */
int str_to_bool(const char *str, int dflt);

/** @brief Convert a boolean value to a string.
 *
 *  @param bool_value A boolean value.
 *  @return "true" or "false", as appropriate.
 */
char *bool_to_str(int bool_val);

/**
 * @defgroup numberlist Number lists
 *
 * These functions provide a method for storing and retrieving a simple list
 * of positive integers.  The list must follow a very restrictive form:
 * each value within [1..32] may be included explicitly in the list.  Higher
 * values may only be included as the part of a single given range [x..y].
 */

/** @brief The number list type. */
typedef struct {
	int values;
	int min, max;
} GGZNumberList;

/** @brief Return an empty number list. */
GGZNumberList ggz_numberlist_new(void);

/** @brief Read a number list from a text string.
 *
 *  The string is comprised of a list of values (in the range 1..32)
 *  separated by spaces, followed by an optional range (separated by "..").
 *  Examples: "2 3 4", "2..4", "1..1000", "2, 3, 10 15-50"
 */
GGZNumberList ggz_numberlist_read(char* text);

/** @brief Write a number list to a ggz-malloc'd text string. */
char *ggz_numberlist_write(GGZNumberList *list);

/** @brief Check to see if the given value is set in the number list. */
int ggz_numberlist_isset(GGZNumberList *list, int value);

/** @brief Return the largest value in the set. */
int ggz_numberlist_get_max(GGZNumberList *list);

/** }@ */


/*
 * GGZ Protocols - used by the GGZ server and client.
 * These should perhaps go in ggz_protocols.h
 */

/* Changing anything below may require bumping up the protocol version.
 * Experiment at your own risk. */

/* Chat subop bitmasks */
#define GGZ_CHAT_M_MESSAGE	(unsigned char) 0x40	/* X1XXXXXX */
#define GGZ_CHAT_M_PLAYER	(unsigned char) 0x80	/* 1XXXXXXX */

/* Error opcodes. */
typedef enum {
	E_USR_LOOKUP	    = -1,
	E_BAD_OPTIONS	    = -2,
	E_ROOM_FULL	    = -3,
	E_TABLE_FULL	    = -4,
	E_TABLE_EMPTY	    = -5,
	E_LAUNCH_FAIL	    = -6,
	E_JOIN_FAIL	    = -7,
	E_NO_TABLE	    = -8,
	E_LEAVE_FAIL	    = -9,
	E_LEAVE_FORBIDDEN   = -10,
	E_ALREADY_LOGGED_IN = -11,
	E_NOT_LOGGED_IN	    = -12,
	E_NOT_IN_ROOM	    = -13,
	E_AT_TABLE	    = -14,
	E_IN_TRANSIT	    = -15,
	E_NO_PERMISSION	    = -16,
	E_BAD_XML	    = -17,
	E_SEAT_ASSIGN_FAIL  = -18
} GGZClientReqError;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GGZ_COMMON_H__ */
