/* 
 * File: ggzdmod.h
 * Author: GGZ Dev Team
 * Project: ggzdmod
 * Date: 10/14/01
 * Desc: GGZ game module functions
 * $Id: ggzdmod.h 2801 2001-12-07 21:04:03Z jdorje $
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
 * Here is a fairly complete example.  In this game we register a handler
 * for each of the possible callbacks.  This particular game is played only
 * when all seats are full; when any seats are empty it must wait (much like
 * a card or board game).
 * @code
 *     // Game-defined handler functions; see below.
 *     void handle_state_change(GGZdModEvent event, GGZdMod* mod, void* data);
 *     void handle_player_join(GGZdModEvent event, GGZdMod* mod, void* data);
 *     void handle_player_leave(GGZdModEvent event, GGZdMod* mod, void* data);
 *     void handle_player_data(GGZdModEvent event, GGZdMod* mod, void* data);
 *
 *     // Other game-defined functions (not ggz-related).
 *     void game_init(GGZdMod *ggzdmod); // initialize a game
 *     void game_launch(void);           // handle a game "launch"
 *     void game_end(void);              // called before the table shuts down
 *     void resume_playing(void);        // we have enough players to play
 *     void stop_playing(void);          // we don't have enough players to play
 *
 *     int main()
 *     {
 *         GGZdMod *mod = ggzdmod_new(GGZ_GAME);
 *         // First we register functions to handle some events.
 *         ggzdmod_set_handler(mod, GGZDMOD_EVENT_STATE,       &handle_state_change);
 *         ggzdmod_set_handler(mod, GGZDMOD_EVENT_JOIN,        &handle_player_join);
 *         ggzdmod_set_handler(mod, GGZDMOD_EVENT_LEAVE,       &handle_player_leave);
 *         ggzdmod_set_handler(mod, GGZDMOD_EVENT_PLAYER_DATA, &handle_player_data);
 *
 *         // Do any other game initializations.
 *         game_init(mod);
 *
 *         // Then we must connect to ggzd
 *         if (ggzdmod_connect(mod) < 0)
 *             exit(-1);
 *         (void)ggzdmod_log(mod, "Starting game.");
 *
 *         // ggzdmod_loop does most of the work, dispatching handlers
 *         // above as necessary.
 *         (void)ggzdmod_loop(mod);
 *
 *         // At the end, we disconnect and destroy the ggzdmod object.
 *         (void)ggzdmod_log(mod, "Ending game.");
 *         (void)ggzdmod_disconnect(mod);
 *         ggzdmod_free(mod);
 *     }
 *
 *     void handle_state_change(GGZdModEvent event, GGZdMod* mod, void* data)
 *     {
 *         GGZdModState old_state = *(GGZdModState*)data;
 *         GGZdModState new_state = ggzdmod_get_state(mod);
 *         if (old_state == GGZDMOD_STATE_CREATED)
 *             // ggzdmod data isn't initialized until it connects with GGZ
 *             // during the game launch, so some initializations should wait
 *             // until here.
 *             game_launch();
 *         switch (new_state) {
 *           case GGZDMOD_STATE_WAITING:
 *             // At this point we've entered the "waiting" state where we
 *             // aren't actually playing.  This is generally triggered by
 *             // the game calling ggzdmod_set_state, which happens when
 *             // a player leaves (down below).  It may also be triggered
 *             // by GGZ automatically.
 *             stop_playing();
 *             break;
 *           case GGZDMOD_STATE_PLAYING:
 *             // At this point we've entered the "playing" state, so we
 *             // should resume play.  This is generally triggered by
 *             // the game calling ggzdmod_set_state, which happens when
 *             // all seats are full (down below).  It may also be
 *             // triggered by GGZ automatically.
 *             resume_playing();
 *             break;
 *           case GGZDMOD_STATE_DONE:
 *             // at this point ggzdmod_loop will stop looping, so we'd
 *             // better close up shop fast.  This will only happen
 *             // automatically if all players leave, but we can force it
 *             // using ggzdmod_set_state.
 *             game_end();
 *             break;
 *         }
 *     }
 *
 *     void handle_player_join(GGZdModEvent event, GGZdMod* mod, void* data)
 *     {
 *         int player = *(int*)data;
 *         GGZSeat seat = ggzdmod_get_seat(mod, player);
 *
 *         // ... do other player initializations ...
 *
 *         if (ggzdmod_count_seats(mod, GGZ_SEAT_OPEN) == 0)
 *             // this particular game will only play when all seats are full.
 *             ggzdmod_set_state(mod, GGZDMOD_STATE_PLAYING);
 *     }
 *
 *     void handle_player_leave(GGZdModEvent event, GGZdMod* mod, void* data)
 *     {
 *         int player = *(int*)data;
 *
 *         // ... do other player un-initializations ...
 *
 *         if (ggzdmod_count_seats(mod, GGZ_SEAT_PLAYER) == 0)
 *             // the game will exit when all human players are gone
 *             ggzdmod_set_state(mod, GGZDMOD_STATE_DONE);
 *         else
 *             // this particular game will only play when all seats are full.
 *             ggzdmod_set_state(mod, GGZDMOD_STATE_WAITING);
 *     }
 *
 *     void handle_player_data(GGZdModEvent event, GGZdMod* mod, void* data)
 *     {
 *         int player = *(int*)data;
 *         int socket_fd = ggzdmod_get_seat(mod, player).fd;
 *
 *         // ... read a packet from the socket ...
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

/** @brief Callback events.
 *
 *  Each of these is a possible ggzdmod event.  For each event, the
 *  table may register a handler with ggzdmod to handle that
 *  event.
 *  @see GGZdModHandler
 *  @see ggzdmod_set_handler */
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

/** @brief A ggzdmod object, used for tracking a ggz<->table connection.
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
 *    - GGZDMOD_EVENT_STATE: The old state (GGZdModState*)
 *    - GGZDMOD_EVENT_JOIN: The player number (int*)
 *    - GGZDMOD_EVENT_LEAVE: The player number (int*)
 *    - GGZDMOD_EVENT_LOG: The message string (char*)
 *    - GGZDMOD_EVENT_PLAYER_DATA: The player number (int*)
 *    - GGZDMOD_EVENT_ERROR: NULL (for now)
 */
typedef void (*GGZdModHandler) (GGZdMod * mod, GGZdModEvent e, void *data);

/** @brief A seat at a GGZ table.
 *
 *  Each seat at the table is tracked like this. */
typedef struct {
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

/** @brief Return gamedata pointer
 *
 *  Each ggzdmod object can be given a "gamedata" pointer,
 *  that is returned by this function.  This is useful for
 *  when a single process serves multiple ggzdmod's.
 *  @see ggzdmod_set_gamedata */
void * ggzdmod_get_gamedata(GGZdMod * mod);

/** @brief Set the number of seats for the table.
 *  @note This will only work for ggzd.
 *  @todo How does this work for the table? */
void ggzdmod_set_num_seats(GGZdMod * mod, int num_seats);

/** @brief Set gamedata pointer */
void ggzdmod_set_gamedata(GGZdMod * mod, void * data);

/** @brief Set a handler for the given event.
 *  @see ggzdmod_get_gamedata */
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

/** @brief Count seats of the given type.
 *
 *  This is a convenience function that counts how many seats
 *  there are that have the given type.  For instance, giving
 *  seat_type==GGZ_SEAT_OPEN will count the number of open
 *  seats.
 *  @param mod The ggzdmod object.
 *  @param seat_type The type of seat to be counted.
 *  @return The number of seats that match seat_type.
 *  @note This could go into a wrapper library instead. */
int ggzdmod_count_seats(GGZdMod * mod, GGZdModSeat seat_type);

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
 *  This function repeatedly handles input from all sockets.  It will
 *  only stop once the game state has been changed to DONE.
 *  @param mod The ggzdmod object.
 *  @return 0 on success, -1 on error.
 *  @see ggzdmod_dispatch
 *  @see ggzdmod_set_state */
int ggzdmod_loop(GGZdMod * mod);

/* 
 * Control functions
 */

/** @brief Change the table's state.
 *
 *  This function should be called to change the state of a table.
 *  A game can use this function to change state between WAITING
 *  and PLAYING, or to set it to DONE.
 *  @param mod The ggzdmod object.
 *  @param state The new state.
 *  @return 0 on success, -1 on failure/error. */
int ggzdmod_set_state(GGZdMod * mod, GGZdModState state);

/** @brief Connect to ggz
 *
 *  This function makes the physical connection to ggz.
 *  @param mod The ggzdmod object.
 *  @return The ggzdmod socket FD on success, -1 on failure.
 */
int ggzdmod_connect(GGZdMod * mod);

/** @brief Disconnect from ggz
 *
 *  This function stops the connection to ggz.
 *  @param mod The ggzdmod object.
 *  @return 0 on success, -1 on failure. */
int ggzdmod_disconnect(GGZdMod * mod);



/** @brief Log data
 *
 *  This function sends the specified string (printf-style) to
 *  ggzd to be logged.
 *  @param mod The ggzdmod object.
 *  @param fmt A printf-style format string.
 *  @return 0 on success, -1 on failure.
 *  @todo We might want to specify the log level (LOG, DEBUG, ...)
 */
int ggzdmod_log(GGZdMod * mod, char *fmt, ...);


#endif /* __GGZDMOD_H__ */
