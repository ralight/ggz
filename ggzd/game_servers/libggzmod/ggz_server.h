/*
 * File: ggz.h
 * Author: Brent Hendricks
 * Project: GGZ 
 * Date: 3/35/00
 * Desc: GGZ game module functions
 * $Id: ggz_server.h 2274 2001-08-27 07:33:22Z jdorje $
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
 * Note that this does not apply to the client sockets: ggzdmod provides
 * one file desriptor for communicating (TCP) to each client.  If data
 * is ready to be read by one of these file descriptors ggzdmod may
 * invoke the appropriate handler (see below), but will never actually
 * read any data.
 *
 * Here's the simplest possible example:
 * @code
 *     int ggz_update(ggzd_assign_t event, void* data); // See GGZDHandler
 *
 *     int main() {
 *         // First we register functions to handle some events.
 *         ggzd_set_handler(GGZ_EVENT_LAUNCH, &ggz_update);
 *         ggzd_set_handler(GGZ_EVENT_JOIN, &ggz_update);
 *         ggzd_set_handler(GGZ_EVENT_LEAVE, &ggz_update);
 *         ggzd_set_handler(GGZ_EVENT_PLAYER, &ggz_update);
 *
 *         // Then ggzd_main does all the rest.
 *         ggzd_main();
 *     }
 * @endcode
 *
 * For more information, see the documentation at http://ggz.sf.net/.
 */

/** @brief Seat assignment (status) values.
 *
 *  Each seat at a game has a status taken from one of these.
 *  @see ggzd_get_seat_status
 */
typedef enum {
	GGZ_SEAT_OPEN	= -1, /**< The seat is open (unoccupied). */
	GGZ_SEAT_BOT	= -2, /**< The seat has a bot (AI) in it. */
	GGZ_SEAT_RESV	= -3, /**< The seat is reserved for a player. */
	GGZ_SEAT_NONE	= -4, /**< This seat does not exist. */
	GGZ_SEAT_PLAYER	= -5  /**< The seat has a regular player in it. */
} ggzd_assign_t;

/** @brief Find the seat assignment status of a given seat.
 *  @param seat The GGZ seat number of the queried seat
 *  @return The assignment status. */
ggzd_assign_t ggzd_get_seat_status(int seat);

/** @brief Find the name of a given player.
 *  @param seat The GGZ seat number of the queried player
 *  @return A pointer to the string.  Do not modify.
 *  @todo Is it correct to return a const char*? */
const char* ggzd_get_player_name(int seat);

/** @brief Find the socket file descriptor for a given seat.
 *  @param seat The GGZ seat number of the queried player
 *  @return The file descriptor for the TCP communications socket
 *  @todo Another function will be needed for a UDP socket */
int ggzd_get_player_socket(int seat);

/** @brief Sends a debugging message to ggzd to be logged.
 *  @param fmt a printf-style format string
 *  @param ... a printf-stype list of arguments
 *  @return 0 on success, -1 on failure */
int ggzd_debug(const char *fmt, ...);

/** @brief Count the total number of seats.
 *  @return The total number of seats at the table. */
int ggzd_seats_num(void);

/** @brief Count the number of open seats.
 *  @return The number of open (unoccupied) seats at the table.
 *  @note This is the number of seats whose status is GGZ_SEAT_OPEN. */
int ggzd_seats_open(void);

/** @brief Count the number of bot/AI seats.
 *  @return The number of bot-occupied seats at the table.
 *  @note This is the number of seats whose status is GGZ_SEAT_BOT.
 *  @todo Is this function necessary? */
int ggzd_seats_bot(void);

/** @brief Count the number of reserved seats.
 *  @return The number of reserved seats at the table.
 *  @note This is the number of seats whose status is GGZ_SEAT_RESV
 *  @todo Is this function necessary? */
int ggzd_seats_reserved(void);

/** @brief Count the number of human-occupied seats.
 *  @return The number of occupied player seats at the table.
 *  @note This is the number of seats whose status is GGZ_SEAT_PLAYER
 *  @todo Is this function necessary? */
int ggzd_seats_human(void);

/** @brief Get ggzdmod's highest FD.
 *  @return The highest file descriptor used by ggzd.
 *  @todo Is this function necessary? */
int ggzd_fd_max(void);

/** @brief Set ggzdmod to block (or not) waiting for data.
 *
 *  Sets blocking status of ggzdmod.  If blocking is set, ggzd_main
 *  and ggzd_read_data will block waiting for data (the default).
 *  @param block 1 for blocking, 0 for not
 *  @todo Brent doesn't want to allow non-blocking or have this function.
 *  @todo This function isn't implemented; there's just this prototype.
 */
void ggzd_set_blocking(int block);


/** @brief A ggzdmod event.
 *  @see GGZDHandler */
typedef enum {
	GGZ_EVENT_LAUNCH	= 0,  /**< a game launch event from ggzd */
	GGZ_EVENT_JOIN		= 1,  /**< a player join event from ggzd */
	GGZ_EVENT_LEAVE		= 2,  /**< a player leave event from ggzd */
	GGZ_EVENT_QUIT		= 3,  /**< a game over event from ggzd */
	GGZ_EVENT_PLAYER	= 4,  /**< a message from a client/player */
	GGZ_EVENT_TICK		= 5   /**< a passed-time event */
} ggzd_event_t;

/** @brief Prototype for an event handler function.
 *
 *  @param event_id The type of event that has just happened.
 *  @param handler_data Specific data; varies by event.
 *  @return A negative return value indicates unrecoverable failure.
 *
 *  Here is an example function showing the most generic use:
 *  @code
 *      int ggz_update(ggzd_event_t event, void *data) {
 *          int player, socket_fd;
 *          switch (event) {
 *            case GGZ_EVENT_LAUNCH:
 *              // do something to handle a game launch (startup)
 *              break;
 *            case GGZ_EVENT_JOIN:
 *              player = *(int*)data; // data for this event
 *              // do something to handle a player joining
 *              break;
 *            case GGZ_EVENT_LEAVE:
 *              player = *(int*)data; // data for this event
 *              // do something to handle a player leaving
 *              break;
 *            case GGZ_EVENT_QUIT:
 *              // do something to quit the game
 *              break;
 *            case GGZ_EVENT_PLAYER:
 *              player = *(int*)data;
 *              socket_fd = ggzd_get_player_socket(player);
 *              // read and handle data from the player
 *              break;
 *            case GGZ_EVENT_TICK:
 *              // Real-time games may use the tick event.
 *              break;
 *          }
 *          return 0;
 *      }
 *  @endcode
 */
typedef int (*GGZDHandler)(ggzd_event_t event_id, void *handler_data);

/** @brief Sets a handler for the specified event.
 *
 *  This function registers a handler for a GGZ event.  Each time this
 *  event occurs, that function will be invoked as a callback.
 *  @param event_id The event that is being associated
 *  @param handler The function that will be used as the handler
 */
void ggzd_set_handler(ggzd_event_t event_id, const GGZDHandler handler);

/** @brief Connects to GGZD.
 *  @return The GGZ file descriptor on success, -1 on failure
 *  @todo Is another function needed to recover the FD?
 */
int ggzd_connect(void);

/** @brief Disconnects from GGZD.
 *  @return 0 on success, -1 on failure. */
int ggzd_disconnect(void);

/** @brief Handle data from GGZD.
 *
 *  This function may be called when there's GGZ data ready
 *  to be read from GGZD.  It does internal handling and calls the
 *  appropriate event handler.
 *  @return 0 normally, 1 on gameover, -1 on failure
 *  @note This function only covers the GGZ socket, not player sockets.
 *  @see ggzd_set_handler */
int ggzd_dispatch(void);

/** @brief Read data from all sockets.
 *
 *  This function may be called to read data from GGZD.  It will
 *  block until GGZ or player data is available, then dispatch the
 *  appropriate handler.
 *  @return 0 normally, 1 on gameover, -1 on failure
 *  @see ggzd_set_handler
 *  @note This function supercedes ggzd_dispatch.
 *  @note This function will check for data on both GGZ and player sockets.
 *  @todo This function may not be a part of the final API */
int ggzd_read_data(void);

/** @brief Connect to GGZ and run the game.
 *
 *  This should do all of the GGZ work necessary for most games.
 *  It repeatedly takes data from GGZD and calls the appropriate
 *  event handler.  It also connects to GGZD.
 *  @return 0 on success, -1 on failure
 *  @note This function will check for data on both GGZ and player sockets.
 *  @note This function supercedes ggzd_read_data, ggzd_dispatch, ggzd_connect, and ggzd_disconnect.
 */
int ggzd_main(void);


#endif /* __GGZ_SERVER_GGZ_H */
