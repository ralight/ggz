/* 
 * File: ggzdmod.h
 * Author: GGZ Dev Team
 * Project: ggzdmod
 * Date: 10/14/01
 * Desc: GGZ game module functions
 * $Id: ggzdmod.h 2767 2001-12-01 06:44:49Z bmh $
 *
 * This file contains the main interface for the ggzdmod library.  This
 * library facilitates the communication between the GGZ server (ggzd)
 * and game servers.  This file provides a unified interface that can be
 * used at both ends.
 *
 * Copyright (C) 2001 GGZ Dev Team.
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

/** @file ggzdmod/ggzdmod.h
 *  @brief Common functions for ggzdmod ggz<->table connection.
 *
 * This file contains all libggzdmod functions used by game servers to
 * interface with GGZD (and vice versa).  Just include ggzdmod.h and make sure
 * your program is linked with libggzdmod.  Then use the functions below as
 * appropriate.
 *
 * GGZDmod currently provides an event-driven interface.  Data from communication
 * sockets is read in by the library, and a handler function (registered as a
 * callback) is invoked to handle the event.  The calling program should not
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
 *     void ggz_update(GGZdModEvent event, GGZdMod* mod, void* data); // See GGZDHandler
 *
 *     int main() {
 *         GGZdMod *mod = ggzdmod_new(GGZ_GAME);
 *         // First we register functions to handle some events.
 *         ggzdmod_set_handler(GGZ_EVENT_STATE, &ggz_update);
 *         ggzdmod_set_handler(GGZ_EVENT_JOIN, &ggz_update);
 *         ggzdmod_set_handler(GGZ_EVENT_LEAVE, &ggz_update);
 *         ggzdmod_set_handler(GGZ_EVENT_PLAYER_DATA, &ggz_update);
 *
 *         // Then ggzd_main_loop does all the rest.
 *         ggzdmod_loop();
 *     }
 * @endcode
 *
 * For more information, see the documentation at http://ggz.sf.net/.
 */


#ifndef __GGZDMOD_H__
#define __GGZDMOD_H__


/** @brief Seat status values.
 *
 * Each "seat" at a table of a GGZ game can have one of these values.
 * They are tracked by ggzdmod. */
typedef enum {
	GGZ_SEAT_OPEN = -1,	   /**< The seat is open (unoccupied). */
	GGZ_SEAT_BOT = -2,	   /**< The seat has a bot (AI) in it. */
	GGZ_SEAT_RESV = -3,	   /**< The seat is reserved for a player. */
	GGZ_SEAT_NONE = -4,	   /**< This seat does not exist. */
	GGZ_SEAT_PLAYER = -5	   /**< The seat has a regular player in it. */
} GGZdModSeat;

/** @brief Table states
 *
 *  Each table has a current "state" that is tracked by ggzdmod.  First
 *  the table is exececuted and begins running.  Then it receives a launch
 *  event from GGZD and begins waiting for players.  At some point a game
 *  will be started and played at the table, after which it may return
 *  to waiting.  Eventually the table will probably halt and then the
 *  program will exit. */
typedef enum {
	GGZDMOD_STATE_CREATED,	/**< Pre-launch; waiting for ggzdmod */
	GGZDMOD_STATE_WAITING,	/**< Ready and waiting to play. */
	GGZDMOD_STATE_PLAYING,	/**< Currently playing a game. */
	GGZDMOD_STATE_DONE		/**< Table halted, prepping to exit. */
} GGZdModState;

/* @brief Callback events. Each of these is a possible ggzdmod event.  For
   each event, the table may register a handler with ggzdmod to handle that
   event. @see GGZdModHandler @see ggzdmod_set_handler */
typedef enum {
	GGZDMOD_EVENT_STATE,	/**< Module status changed */
	GGZDMOD_EVENT_JOIN,		/**< Player joined */
	GGZDMOD_EVENT_LEAVE,	/**< Player left */
	GGZDMOD_EVENT_LOG,		/**< Module log request */
	GGZDMOD_EVENT_PLAYER_DATA,	/**< Data avilable from player */
	GGZDMOD_EVENT_ERROR		/**< Error (not used yet) */
} GGZdModEvent;

/** @brief The "type" of ggzdmod.
 *
 * The "flavor" of GGZdmod object this is.  Affects what operations are
 * allowed. */
typedef enum {
	GGZDMOD_GGZ, /**< Used by ggzd, the ggz end. */
	GGZDMOD_GAME /**< Used by the table/game end. */
} GGZdModType;

/**< @brief A ggzdmod object, used for tracking a ggz<->table connection.
 *
 * A game should track a pointer to a GGZdMod object; it contains all the
 * state information for a ggz<->table connection. */
typedef void GGZdMod;

/** @brief Event handler prototype
 *
 *  A function of this type will be called to handle a ggzdmod event.
 *  @param mod The ggzdmod state object.
 *  @param e The event that has occured.
 *  @param data Pointer to additional data for the event.
 *  The additional data will be of the following form:
 *    - GGZDMOD_EVENT_STATE: NULL
 *    - GGZDMOD_EVENT_JOIN: The player number (int*)
 *    - GGZDMOD_EVENT_LEAVE: The player number (int*)
 *    - GGZDMOD_EVENT_LOG: The message string (char*)
 *    - GGZDMOD_EVENT_PLAYER_DATA: The player number (int*)
 *    - GGZDMOD_EVENT_ERROR: NULL (for now)
 *
 *  Here is an example function showing the most generic use for a table:
 *  @code
 *      void ggz_update(ggzd_event_t event, void *data) {
 *          int player, socket_fd;
 *          switch (event) {
 *            case GGZDMOD_EVENT_STATE:
 *              old_state = *(GGZdModState*)data; // data for this event
 *              // do something to handle a game launch (startup or quit)
 *              break;
 *            case GGZDMOD_EVENT_JOIN:
 *              player = *(int*)data; // data for this event
 *              // do something to handle a player joining
 *              break;
 *            case GGZDMOD_EVENT_LEAVE:
 *              player = *(int*)data; // data for this event
 *              // do something to handle a player leaving
 *              break;
 *            case GGZDMOD_EVENT_PLAYER_DATA:
 *              player = *(int*)data;
 *              socket_fd = ggzd_get_player_socket(player);
 *              // read and handle data from the player
 *              break;
 *          }
 *      }
 *  @endcode
 */
typedef void (*GGZdModHandler) (GGZdMod * mod, GGZdModEvent e, void *data);

/** @brief A seat at a GGZ table.
 *
 *  Each seat at the table is tracked like this. */
typedef struct _GGZSeat {
	int num;		/**< Seat index; 0..(num_seats-1). */
	GGZdModSeat type;	/**< Type of seat. */
	char *name;		/**< Name of player occupying seat. */
	int fd;			/**< fd to communicate with seat occupant. */
} GGZSeat;

/* 
 * Creation functions
 */

/** @brief Create a new ggzdmod object.
 *
 *  Before connecting through ggzdmod, a new ggzdmod object is needed. */
GGZdMod *ggzdmod_new(GGZdModType type);

/** @brief Destroy a finished ggzdmod object.
 *
 *  After the connection is through, the object may be freed. */
void ggzdmod_free(GGZdMod * mod);

/* 
 * Accessor functions
 */

/** @brief Return the file descriptor for the ggzdmod socket. */
int ggzdmod_get_fd(GGZdMod * mod);

/** @brief Return the type of the ggzdmod object. */
GGZdModType ggzdmod_get_type(GGZdMod * mod);

/** @brief Return the current state of the table. */
GGZdModState ggzdmod_get_state(GGZdMod * mod);

/** @brief Return the total number of seats at the table. */
int ggzdmod_get_num_seats(GGZdMod * mod);

/** @brief Returns data for the specified seat.
 *  @note This has not been finalized. */
GGZSeat ggzdmod_get_seat(GGZdMod * mod, int seat);

/** @brief Return gamedata pointer*/
void * ggzdmod_get_gamedata(GGZdMod * mod);

/** @brief Set the number of seats for the table.
 *  @note This will only work for ggzd.
 *  @todo How does this work for the table? */
void ggzdmod_set_num_seats(GGZdMod * mod, int num_seats);

/** @brief Set gamedata pointer*/
void ggzdmod_set_gamedata(GGZdMod * mod, void * data);

/** @brief Set a handler for the given event. */
void ggzdmod_set_handler(GGZdMod * mod, GGZdModEvent e, GGZdModHandler func);

/** @brief Set the module executable and it's arguments 
 *  @note It should not be called by the table, only ggzd. */
void ggzdmod_set_module(GGZdMod * mod, char **args);

/** @brief Set seat data.
 *  @note The seat parameter contains the seat number.
 *  @todo This interface is unwieldy.
 *  @return 0 on success, negative on failure.
 */
int ggzdmod_set_seat(GGZdMod * mod, GGZSeat * seat);

/* 
 * Event/Data handling
 */

/** @brief Check for and handle input.
 *
 *  This function handles input from the communications sockets:
 *    - It will check for input, but will not block.
 *    - It will monitor input from all sockets, ggzdmod and player FD's.
 *    - It will call an event handler as necessary. */
int ggzdmod_dispatch(GGZdMod * mod);

/** @brief Loop while handling input.
 *
 *  This function repeatedly handles input from all sockets.
 *  @see ggzdmod_dispatch
 *  @see ggzdmod_halt_table */
int ggzdmod_loop(GGZdMod * mod);

/* 
 * Control functions
 */

/** @brief Halt the table.
 *
 *  This function should be called when the table is finished.  When
 *  called by the game, it will stop looping and change the game state.
 *  @note It should inform ggzd of this change.
 *  @note ggzd should be able to request this as well. */
int ggzdmod_halt_table(GGZdMod * mod);

/** @brief Connect to ggz
 *
 *  This function makes the physical connection to ggz.
 */
int ggzdmod_connect(GGZdMod * mod);

/** @brief Disconnect from ggz
 *
 *  This function stops the connection to ggz.
 *  @note It should not be called by ggzd, only the table. */
int ggzdmod_disconnect(GGZdMod * mod);



/** @brief Log data
 *
 *  This function sends the specified string (printf-style) to
 *  ggzd to be logged. */
int ggzdmod_log(GGZdMod * mod, char *fmt, ...);


#endif /* __GGZDMOD_H__ */
