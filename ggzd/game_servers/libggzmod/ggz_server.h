/*
 * File: ggz.h
 * Author: Brent Hendricks
 * Project: GGZ 
 * Date: 3/35/00
 * Desc: GGZ game module functions
 * $Id: ggz_server.h 2273 2001-08-27 06:48:01Z jdorje $
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


#ifndef __GGZ_SERVER_GGZ_H
#define __GGZ_SERVER_GGZ_H

/**
 * @file ggz_server.h
 * @brief The interface for the ggzdmod library used by game servers.
 *
 * This file contains all libggzdmod functions used by game servers to
 * interface with GGZD.  Just include ggz_server.h and make sure your program
 * is linked with libggzmod.  Then use the functions below as appropriate.
 *
 * GGZDmod currently provides an event-driven interface.  Data from GGZD
 * is read in by the library, and a handler function (registered as a
 * callback) is invoked to handle the event.  The game server should not
 * read data from the GGZD socket unless it really knows what it is doing.
 *
 * That this does not apply to the client sockets: ggzdmod provides
 * one file desriptor for communicating (TCP) to each client.  If data
 * is ready to be read by one of these file descriptors ggzdmod may
 * invoke the appropriate handler (see below), but will never actually
 * read any data.
 *
 * @todo Example code.
 *
 * For more information, see the documentation at http://ggz.sf.net/.
 */

/** @brief Seat assignment values.
 *
 *  Each seat at a game has a status taken from one of these.
 */
typedef enum {
	GGZ_SEAT_OPEN	= -1, /**< The seat is open (unoccupied). */
	GGZ_SEAT_BOT	= -2, /**< The seat has a bot (AI) in it. */
	GGZ_SEAT_RESV	= -3, /**< The seat is reserved for a player. */
	GGZ_SEAT_NONE	= -4, /**< This seat does not exist. */
	GGZ_SEAT_PLAYER	= -5  /**< The seat has a regular player in it. */
} ggzd_assign_t;

/* accessor functions for seat data */
/* FIXME: should the return values be const? */
ggzd_assign_t ggzd_get_seat_status(int seat);
char* ggzd_get_player_name(int seat);
int ggzd_get_player_socket(int seat);

/** @brief Sends a debugging message to ggzd to be logged.
 *  @param fmt a printf-style format string
 *  @param ... a printf-stype list of arguments
 *  @return 0 on success, -1 on failure */
int ggzd_debug(const char *fmt, ...);

/** @return The total number of seats at the table. */
int ggzd_seats_num(void);

/** @return The number of open (unoccupied) seats at the table. */
int ggzd_seats_open(void);

/** @return The number of bot-occupied seats at the table. */
int ggzd_seats_bot(void);

/** @return The number of reserved seats at the table. */
int ggzd_seats_reserved(void);

/** @return The number of occupied player seats at the table. */
int ggzd_seats_human(void);

/** @return The highest file descriptor used by ggzd.
 *  @todo Is this even necessary??? */
int ggzd_fd_max(void);

/** Sets blocking status of ggzdmod.  If blocking is set, ggzd_main
 *  and ggzd_read_data will block waiting for data (the default).
 *  @param block 1 for blocking, 0 for not
 *  @todo Brent doesn't want to have this function.
 *  @todo This function isn't implemented; there's just this prototype.
 */
void ggzd_set_blocking(int block);


typedef enum {
	GGZ_EVENT_LAUNCH	= 0,  /**< a game launch event from ggzd */
	GGZ_EVENT_JOIN		= 1,  /**< a player join event from ggzd */
	GGZ_EVENT_LEAVE		= 2,  /**< a player leave event from ggzd */
	GGZ_EVENT_QUIT		= 3,  /**< a game over event from ggzd */
	GGZ_EVENT_PLAYER	= 4,  /**< a message from a client/player */
	GGZ_EVENT_TICK		= 5   /**< a passed-time event */
} ggzd_event_t;

/** A handler function should correspond to this prototype. */
typedef int (*GGZDHandler)(ggzd_event_t event_id, void *handler_data);

/** Sets a handler for the specified event. */
void ggzd_set_handler(ggzd_event_t event_id, const GGZDHandler handler);

/** Connects to GGZD.
 *  @return The GGZ file descriptor on success, -1 on failure
 */
int ggzd_connect(void);

/** Disconnects from GGZD.
 *  @return 0 on success, -1 on failure. */
int ggzd_disconnect(void);

/** This function may be called when there's GGZ data ready
 *  to be read from GGZD.  It calls the appropriate event handler.
 *  @return 0 normally, 1 on gameover, -1 on failure
 *  @note This function only covers the GGZ socket, not player sockets.
 *  @see ggzd_set_handler */
int ggzd_dispatch(void);

/** This function may be called to read data from GGZD.  It will
 *  block until GGZ or player data is available, then dispatch the
 *  appropriate handler.
 *  @return 0 normally, 1 on gameover, -1 on failure
 *  @see ggzd_set_handler
 *  @note This function supercedes ggzd_dispatch.
 *  @note This function will check for data on both GGZ and player sockets.
 *  @note This function may not be a part of the final API */
int ggzd_read_data(void);

/** This should do all of the GGZ work necessary for most games.
 *  It repeatedly takes data from GGZD and calls the appropriate
 *  event handler.  It also connects to GGZD.
 *  @return 0 on success, -1 on failure
 *  @note This function will check for data on both GGZ and player sockets.
 *  @note This function supercedes ggzd_read_data, ggzd_dispatch, ggzd_connect, and ggzd_disconnect.
 */
int ggzd_main(void);


#endif /* __GGZ_SERVER_GGZ_H */
