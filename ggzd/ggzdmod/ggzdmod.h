/* 
 * File: ggzdmod.h
 * Author: GGZ Dev Team
 * Project: ggzdmod
 * Date: 10/14/01
 * Desc: GGZ game module functions
 * $Id: ggzdmod.h 5030 2002-10-25 17:33:21Z jdorje $
 *
 * This file contains the main interface for the ggzdmod library.  This
 * library facilitates the communication between the GGZ server (ggzd)
 * and game servers.  This file provides a unified interface that can be
 * used at both ends.
 *
 * Copyright (C) 2001-2002 GGZ Development Team.
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

#include <ggz.h> /* libggz */
#include <ggz_common.h>

/** @file ggzdmod.h
 *  @brief Common functions for interfacing a game server and GGZ.
 *
 * This file contains all libggzdmod functions used by game servers to
 * interface with GGZ (and vice versa).  Just include ggzdmod.h and make sure
 * your program is linked with libggzdmod.  Then use the functions below as
 * appropriate.
 *
 * GGZdmod currently provides an event-driven interface.  Data from
 * communication sockets is read in by the library, and a handler function
 * (registered as a callback) is invoked to handle any events.  The calling
 * program should not read/write data from/to the GGZ socket unless it really
 * knows what it is doing.
 *
 * That this does not apply to the client sockets: ggzdmod provides
 * one file desriptor for communicating (TCP) to each client.  If data
 * is ready to be read by one of these file descriptors ggzdmod may
 * invoke the appropriate handler (see below), but will never actually
 * read any data.
 *
 * Here is a fairly complete example.  In this game we register a handler
 * for each of the possible callbacks.  This particular game is played only
 * when all seats are full; when any seats are empty it must wait (much like
 * a card or board game).
 *
 * @code
 *     // Game-defined handler functions for GGZ events; see below.
 *     void handle_state_change(GGZdMod* ggz, GGZdModEvent event, void* data);
 *     void handle_player_join(GGZdMod* ggz, GGZdModEvent event, void* data);
 *     void handle_player_leave(GGZdMod* ggz, GGZdModEvent event, void* data);
 *     void handle_player_data(GGZdMod* ggz, GGZdModEvent event, void* data);
 *
 *     // Other game-defined functions (not ggz-related).
 *     void game_init(GGZdMod *ggz); // initialize a game
 *     void game_launch(void);           // handle a game "launch"
 *     void game_end(void);              // called before the table shuts down
 *     void resume_playing(void);        // we have enough players to play
 *     void stop_playing(void);          // not enough players to play
 *
 *     int main()
 *     {
 *         GGZdMod *ggz = ggzdmod_new(GGZ_GAME);
 *         // First we register functions to handle some events.
 *         ggzdmod_set_handler(ggz, GGZDMOD_EVENT_STATE,
 *                             &handle_state_change);
 *         ggzdmod_set_handler(ggz, GGZDMOD_EVENT_JOIN,
 *                             &handle_player_join);
 *         ggzdmod_set_handler(ggz, GGZDMOD_EVENT_LEAVE,
 *                             &handle_player_leave);
 *         ggzdmod_set_handler(ggz, GGZDMOD_EVENT_PLAYER_DATA,
 *                             &handle_player_data);
 *
 *         // Do any other game initializations.  You'll probably want to
 *         // track "ggz" globally.
 *         game_init(mod);
 *
 *         // Then we must connect to GGZ
 *         if (ggzdmod_connect(ggz) < 0)
 *             exit(-1);
 *         (void) ggzdmod_log(ggz, "Starting game.");
 *
 *         // ggzdmod_loop does most of the work, dispatching handlers
 *         // above as necessary.
 *         (void) ggzdmod_loop(ggz);
 *
 *         // At the end, we disconnect and destroy the ggzdmod object.
 *         (void) ggzdmod_log(ggz, "Ending game.");
 *         (void) ggzdmod_disconnect(ggz);
 *         ggzdmod_free(ggz);
 *     }
 *
 *     void handle_state_change(GGZdMod* ggz, GGZdModEvent event, void* data)
 *     {
 *         GGZdModState old_state = *(GGZdModState*)data;
 *         GGZdModState new_state = ggzdmod_get_state(ggz);
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
 *     void handle_player_join(GGZdMod* ggz, GGZdModEvent event, void* data)
 *     {
 *         GGZSeat *old_seat = (GGZSeat*)data;
 *         GGZSeat new_seat = ggzdmod_get_seat(ggz, old_seat->num);
 *
 *         // ... do other player initializations ...
 *
 *         if (ggzdmod_count_seats(ggz, GGZ_SEAT_OPEN) == 0)
 *             // this particular game will only play when all seats are full.
 *             // calling this function triggers the STATE event, so we'll end
 *             // up executing resume_playing() above.
 *             ggzdmod_set_state(ggz, GGZDMOD_STATE_PLAYING);
 *     }
 *
 *     void handle_player_leave(GGZdMod* ggz, GGZdModEvent event, void* data)
 *     {
 *         GGZSeat *old_seat = (GGZSeat*)data;
 *         GGZSeat new_seat = ggzdmod_get_seat(ggz, old_seat->num);
 *
 *         // ... do other player un-initializations ...
 *
 *         if (ggzdmod_count_seats(ggz, GGZ_SEAT_PLAYER) == 0)
 *             // the game will exit when all human players are gone
 *             ggzdmod_set_state(ggz, GGZDMOD_STATE_DONE);
 *         else
 *             // this particular game will only play when all seats are full.
 *             // calling this function triggers the STATE event, so we'll end
 *             // up executing stop_playing() above.
 *             ggzdmod_set_state(ggz, GGZDMOD_STATE_WAITING);
 *     }
 *
 *     void handle_player_data(GGZdMod* ggz, GGZdModEvent event, void* data)
 *     {
 *         int player = *(int*)data;
 *         int socket_fd = ggzdmod_get_seat(ggz, player).fd;
 *
 *         // ... read a packet from the socket ...
 *     }
 * @endcode
 *
 * For more information, see the documentation at http://ggz.sf.net/.
 */


#ifndef __GGZDMOD_H__
#define __GGZDMOD_H__

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Table states
 *
 *  Each table has a current "state" that is tracked by ggzdmod.  First
 *  the table is executed and begins running.  Then it receives a launch
 *  event from GGZD and begins waiting for players.  At some point a game
 *  will be started and played at the table, after which it may return
 *  to waiting.  Eventually the table will probably halt and then the
 *  program will exit.
 *
 *  More specifically, the game is in the CREATED state when it is first
 *  executed.  It moves to the WAITING state after GGZ first communicates
 *  with it.  After this, the game server may use ggzdmod_set_state to
 *  change between WAITING, PLAYING, and DONE states.  A WAITING game is
 *  considered waiting for players (or whatever), while a PLAYING game is
 *  actively being played (this information may be, but currently is not,
 *  propogated back to GGZ for display purposes).  Once the state is changed
 *  to DONE, the table is considered dead and will exit shortly
 *  thereafter (ggzdmod_loop will stop looping, etc.) (see the kill_on_exit
 *  game option).
 *
 *  Each time the game state changes, a GGZDMOD_EVENT_STATE event will be
 *  propogated to the game server.
 */
typedef enum {
	GGZDMOD_STATE_CREATED,	/**< Pre-launch; waiting for ggzdmod */
	GGZDMOD_STATE_WAITING,	/**< Ready and waiting to play. */
	GGZDMOD_STATE_PLAYING,	/**< Currently playing a game. */
	GGZDMOD_STATE_DONE	/**< Table halted, prepping to exit. */
} GGZdModState;

/** @brief Callback events.
 *
 *  Each of these is a possible GGZdmod event.  For each event, the
 *  table may register a handler with GGZdmod to handle that
 *  event.
 *  @see GGZdModHandler
 *  @see ggzdmod_set_handler
 */
typedef enum {
	/** @brief Module status changed
	 *  This event occurs when the game's status changes.  The old
	 *  state (a GGZdModState*) is passed as the event's data.
	 *  @see GGZdModState */
	GGZDMOD_EVENT_STATE,

	/** @brief Player joined
	 *  This event occurs when a player joins the table.  The
	 *  old seat (a GGZSeat*) is passed as the event's data.
	 *  The seat information will be updated before the event
	 *  is invoked.
	 *  @note This may be dropped in favor of the SEAT event. */
	GGZDMOD_EVENT_JOIN,

	/** @brief Player left
	 *  This event occurs when a player leaves the table.  The
	 *  old seat (a GGZSeat*) is passed as the event's data.
	 *  The seat information will be updated before the event
	 *  is invoked.
	 *  @note This may be dropped in favor of the SEAT event */
	GGZDMOD_EVENT_LEAVE,

	/** @brief General seat change
	 *  This event occurs when a seat change other than a player
	 *  leave/join happens (which is currently impossible).  The
	 *  old seat (a GGZSeat*) is passed as the event's data.  The
	 *  seat information will be updated before the event is invoked.
	 *  @note This is currently unused, but may eventually replace JOIN and LEAVE.
	 */
	GGZDMOD_EVENT_SEAT,

	/** @brief A spectator joins the game.
	 *  The data of the old spectator (GGZSpectator*) is passed as the
	 *  data for the event.  It can be assumed that the spectator seat
	 *  was previously empty, so the name and socket given will be
	 *  invalid (NULL/-1). */
	GGZDMOD_EVENT_SPECTATOR_JOIN,

	/** @brief A spectator left the game
	 *  The old spectator data can be obtained via the (GGZSpectator*)
	 *  which is passed as the event data. */
	GGZDMOD_EVENT_SPECTATOR_LEAVE,

	/** @brief A spectator seat changed.
	 *  The old spectator data can be obtained via the (GGZSpectator*)
	 *  which is passed as the event data.  This may someday replace
	 *  both SPECTATOR_JOIN and SPECTATOR_LEAVE. */
	GGZDMOD_EVENT_SPECTATOR_SEAT,

	/** @brief Module log request
	 *  This event occurs when a log request happens.  This will
	 *  only be used by the GGZ server; the game server should
	 *  use ggzdmod_log to generate the log. */
	GGZDMOD_EVENT_LOG,

	/** @brief Data available from player
	 *  This event occurs when there is data ready to be read from
	 *  one of the player sockets.  The player number (an int*) is
	 *  passed as the event's data. */
	GGZDMOD_EVENT_PLAYER_DATA,

	/** @brief Data available from spectator
	 *  For games which support spectators, this indicates that one of them
	 *  sent some data to the game server. */
	GGZDMOD_EVENT_SPECTATOR_DATA,

	/** @brief An error has occurred
	 *  This event occurs when a GGZdMod error has occurred.  An
	 *  error message (a char*) will be passed as the event's data.
	 *  GGZdMod may attempt to recover from the error, but it is
	 *  not guaranteed that the GGZ connection will continue to
	 *  work after an error has happened. */
	GGZDMOD_EVENT_ERROR
} GGZdModEvent;

/** @brief The "type" of ggzdmod.
 *
 * The "flavor" of GGZdmod object this is.  Affects what operations are
 * allowed.
 */
typedef enum {
	GGZDMOD_GGZ,	/**< Used by the ggz server ("ggzd"). */
	GGZDMOD_GAME	/**< Used by the game server ("table"). */
} GGZdModType;

/** @brief A GGZdmod object, used for tracking a ggzd<->table connection.
 *
 * A game server should track a pointer to a GGZdMod object; it contains
 * all the state information for communicating with GGZ.  The GGZ server
 * will track one such object for every game table that is running.
 */
typedef struct GGZdMod GGZdMod;

/** @brief Event handler prototype
 *
 *  A function of this type will be called to handle a ggzdmod event.
 *  @param mod The ggzdmod state object.
 *  @param e The event that has occured.
 *  @param data Pointer to additional data for the event.
 *  The additional data will be of the following form:
 *    - GGZDMOD_EVENT_STATE: The old state (GGZdModState*)
 *    - GGZDMOD_EVENT_JOIN: The old seat (GGZSeat*)
 *    - GGZDMOD_EVENT_LEAVE: The old seat (GGZSeat*)
 *    - GGZDMOD_EVENT_SEAT: The old seat (GGZSeat*)
 *    - GGZDMOD_EVENT_SPECTATOR_JOIN: The old spectator's data (GGZSpectator*)
 *    - GGZDMOD_EVENT_SPECTATOR_LEAVE: The old spectator's data (GGZSpectator*)
 *    - GGZDMOD_EVENT_LOG: The message string (char*)
 *    - GGZDMOD_EVENT_PLAYER_DATA: The player number (int*)
 *    - GGZDMOD_EVENT_SPECTATOR_DATA: The spectator number (int*)
 *    - GGZDMOD_EVENT_ERROR: An error string (char*)
 */
typedef void (*GGZdModHandler) (GGZdMod * mod, GGZdModEvent e, void *data);

/** @brief A seat at a GGZ game table.
 *
 *  Each seat at the table is tracked like this.
 */
typedef struct {
	unsigned int num;	/**< Seat index; 0..(num_seats-1). */
	GGZSeatType type;	/**< Type of seat. */
	char *name;		/**< Name of player occupying seat. */
	int fd;			/**< fd to communicate with seat occupant. */
} GGZSeat;

/** @brief A game spectator entry
 *
 *  Spectators are handles differently from other player types.
 */
typedef struct {
	unsigned int num;	/**< Spectator seat index */
	char *name;		/**< The spectator's name */
	int fd;			/**< File descriptor for communication */
} GGZSpectator;

/* 
 * Creation functions
 */

/** @brief Create a new ggzdmod object.
 *
 *  Before connecting through ggzdmod, a new ggzdmod object is needed.
 *  @param type The type of ggzdmod.  Should be GGZDMOD_GAME for game servers.
 *  @see GGZdModType
 */
GGZdMod *ggzdmod_new(GGZdModType type);

/** @brief Destroy a finished ggzdmod object.
 *
 *  After the connection is through, the object may be freed.
 *  @param ggzdmod The GGZdMod object.
 */
void ggzdmod_free(GGZdMod * ggzdmod);

/* 
 * Accessor functions
 */

/** @brief Get the file descriptor for the GGZdMod socket.
 *
 *  @param ggzdmod The GGZdMod object.
 *  @return GGZdMod's main ggzd <-> table socket FD.
 *  @note Don't use this; use ggzdmod_loop and friends instead.
 */
int ggzdmod_get_fd(GGZdMod * ggzdmod);

/** @brief Get the type of the ggzdmod object.
 *  @param ggzdmod The GGZdMod object.
 *  @return The type of the GGZdMod object (GGZ or GAME).
 */
GGZdModType ggzdmod_get_type(GGZdMod * ggzdmod);

/** @brief Get the current state of the table.
 *  @param ggzdmod The GGZdMod object.
 *  @return The state of the table.
 */
GGZdModState ggzdmod_get_state(GGZdMod * ggzdmod);

/** @brief Get the total number of seats at the table.
 *  @return The number of seats, or -1 on error.
 *  @note If no connection is present, -1 will be returned.
 *  @note While in GGZDMOD_STATE_CREATED, we don't know the number of seats.
 */
int ggzdmod_get_num_seats(GGZdMod * ggzdmod);

/** @brief Get all data for the specified seat.
 *  @param ggzdmod The GGZdMod object.
 *  @param seat The seat number (0..(number of seats - 1)).
 *  @return A valid GGZSeat structure, if seat is a valid seat.
 */
GGZSeat ggzdmod_get_seat(GGZdMod * ggzdmod, int seat);

/** @brief Return gamedata pointer
 *
 *  Each GGZdMod object can be given a "gamedata" pointer that is returned
 *  by this function.  This is useful for when a single process serves
 *  multiple GGZdmod's.
 *  @param ggzdmod The GGZdMod object.
 *  @return A pointer to the gamedata block (or NULL if none).
 *  @see ggzdmod_set_gamedata */
void * ggzdmod_get_gamedata(GGZdMod * ggzdmod);

/** @brief Set the number of seats for the table.
 *  @param ggzdmod The GGZdMod object.
 *  @param num_seats The number of seats to set.
 *  @return 0 on success, -1 on failure.
 *  @note This will only work for ggzd.
 *  @todo Allow the table to change the number of seats. */
int ggzdmod_set_num_seats(GGZdMod * ggzdmod, int num_seats);

/** @brief Set gamedata pointer
 *  @param ggzdmod The GGZdMod object.
 *  @param data The gamedata block (or NULL for none).
 *  @see ggzdmod_get_gamedata */
void ggzdmod_set_gamedata(GGZdMod * ggzdmod, void * data);

/** @brief Get the maximum number of spectators.
 *  This function returns the maximum number of spectator seats available.  A
 *  game can use this to iterate over the spectator seats to look for
 *  spectators occupying them.  Since spectators may come and go at any point
 *  and there is no theoretical limit on the number of spectators, you should
 *  consider this value to be dynamic and call this function again each time
 *  you're looking for spectators.
 *  @return The number of available spectator seats, or -1 on error.
 *  @note If no connection is present, -1 will be returned.
 */
int ggzdmod_get_max_num_spectators(GGZdMod *ggzdmod);

/** @brief Get a spectator's data.
 *  @param ggzdmod The GGZdMod object.
 *  @param spectator The number, between 0 and (number of spectators - 1).
 *  @return A valid GGZSpectator structure, if arguments are valid.
 */
GGZSpectator ggzdmod_get_spectator(GGZdMod * ggzdmod, int spectator);

/** @brief Set a handler for the given event.
 *
 *  As described above, GGZdmod uses an event-driven structure.  Each
 *  time an event is called, the event handler (there can be only one)
 *  for that event will be called.  This function registers such an
 *  event handler.
 *  @param mod The GGZdmod object.
 *  @param e The GGZdmod event.
 *  @param func The handler function being registered.
 *  @return 0 on success, negative on failure (bad parameters)
 *  @see ggzdmod_get_gamedata
 */
int ggzdmod_set_handler(GGZdMod * ggzdmod, GGZdModEvent e,
			GGZdModHandler func);

/** @brief Set the module executable, pwd, and arguments
 *
 *  GGZdmod must execute and launch the game to start a table; this
 *  function allows ggzd to specify how this should be done.
 *  @note This should not be called by the table, only ggzd.
 *  @param ggzdmod The GGZdmod object.
 *  @param pwd The working directory for the game, or NULL.
 *  @param args The arguments for the program, as needed by exec.
 *  @note The pwd directory must already exist.
 *  @note The executable must be an absolute path (or relative to pwd).
 */
void ggzdmod_set_module(GGZdMod * ggzdmod,
                        const char *pwd, char **args);

/** @brief Set seat data.
 *
 *  A game server or the ggz server can use this function to set
 *  data about a seat.  The game server may only change the following
 *  things about a seat:
 *    - The name (only if the seat is a bot).
 *    - The socket FD (only if the FD is -1).
 *  @param seat The new seat structure (which includes seat number).
 *  @return 0 on success, negative on failure.
 *  @todo The game should be able to toggle between BOT and OPEN seats.
 *  @todo The game should be able to kick a player out of the table.
 */
int ggzdmod_set_seat(GGZdMod * ggzdmod, GGZSeat * seat);

/** @brief Set spectator data.
 *
 *  A game server or the ggz server can use this function to set
 *  data about a spectator.  The game server may only change the following
 *  things about a spectator:
 *    - The socket FD (only if the FD is -1).
 *  @param spectator The new spectator structure (which includes spectator number).
 *  @return 0 on success, negative on failure.
 */
int ggzdmod_set_spectator(GGZdMod * ggzdmod, GGZSpectator * spectator);


/** @brief Change a player's seat.
 *
 *  Move a player from a seat to another seat, from a spectator seat to
 *  a seat, or from a seat to a spectator seat.
 *  @param old_seat The (spectator) seat number of the player.
 *  @param was_spectator Whether the seat is a normal or spectator seat.
 *  @param new_seat The (spectator) seat the player moves to.
 *  @param is_spectator Whether the new seat is a normal/spectator seat.
 *  @note This should only be called by GGZ.
 *  @return 0 on success, negative on failure.
 */
int ggzdmod_reseat(GGZdMod * ggzdmod,
		   int old_seat, int was_spectator,
		   int new_seat, int is_spectator);

/** @brief Count seats of the given type.
 *
 *  This is a convenience function that counts how many seats
 *  there are that have the given type.  For instance, giving
 *  seat_type==GGZ_SEAT_OPEN will count the number of open
 *  seats.
 *  @param ggzdmod The ggzdmod object.
 *  @param seat_type The type of seat to be counted.
 *  @return The number of seats that match seat_type.
 *  @note This could go into a wrapper library instead.
 */
int ggzdmod_count_seats(GGZdMod * ggzdmod, GGZSeatType seat_type);

/** @brief Count current number of spectators.
 *
 *  This function returns the number of spectators watching the game.  Note
 *  that the spectator numbers may not match up: if there are two spectators
 *  they could be numbered 0 and 4.  If you're trying to iterate through the
 *  existing spectators, you probably want ggzdmod_get_max_num_spectators()
 *  instead.
 *  @param ggzdmod The ggzdmod object
 *  @return The number of spectators watching the game (0 on error)
 */
int ggzdmod_count_spectators(GGZdMod * ggzdmod);

/* 
 * Event/Data handling
 */

/** @brief Check for and handle input.
 *
 *  This function handles input from the communications sockets:
 *    - It will check for input, but will not block.
 *    - It will monitor input from the GGZdmod socket.
 *    - It will monitor input from player sockets only if a handler is
 *      registered for the PLAYER_DATA event.
 *    - It will call an event handler as necessary.
 *  @param ggzdmod The ggzdmod object.
 *  @return -1 on error, the number of events handled (0-1) on success.
 */
int ggzdmod_dispatch(GGZdMod * ggzdmod);

/** @brief Loop while handling input.
 *
 *  This function repeatedly handles input from all sockets.  It will
 *  only stop once the game state has been changed to DONE (or if there
 *  has been an error).
 *  @param ggzdmod The ggzdmod object.
 *  @return 0 on success, -1 on error.
 *  @see ggzdmod_dispatch
 *  @see ggzdmod_set_state
 */
int ggzdmod_loop(GGZdMod * ggzdmod);

/* 
 * Control functions
 */

/** @brief Change the table's state.
 *
 *  This function should be called to change the state of a table.
 *  A game can use this function to change state between WAITING
 *  and PLAYING, or to set it to DONE.
 *  @param ggzdmod The ggzdmod object.
 *  @param state The new state.
 *  @return 0 on success, -1 on failure/error.
 */
int ggzdmod_set_state(GGZdMod * ggzdmod, GGZdModState state);

/** @brief Connect to ggz.
 *
 *  Call this function to make an initial GGZ <-> game connection.
 *  - When called by the game server, this function makes the
 *    physical connection to ggz.
 *  - When called by ggzd, it will launch a table and connect
 *    to it.  Note - if the game fails to exec, this function may not catch it.
 *  @param ggzdmod The ggzdmod object.
 *  @return 0 on success, -1 on failure.
 */
int ggzdmod_connect(GGZdMod * ggzdmod);

/** @brief Disconnect from ggz.
 *
 *  - When called by the game server, this function stops
 *    the connection to GGZ.  It should only be called
 *    when the table is ready to exit.
 *  - When called by the GGZ server, this function will
 *    kill and clean up after the table.
 *  @param ggzdmod The ggzdmod object.
 *  @return 0 on success, -1 on failure.
 */
int ggzdmod_disconnect(GGZdMod * ggzdmod);



/** @brief Log data
 *
 *  This function sends the specified string (printf-style) to
 *  the GGZ server to be logged.
 *  @param ggzdmod The GGZdmod object.
 *  @param fmt A printf-style format string.
 *  @return 0 on success, -1 on failure.
 */
int ggzdmod_log(GGZdMod * ggzdmod, char *fmt, ...)
		ggz__attribute((format(printf, 2, 3)));
		
		
/** @brief Log all information about the ggzdmod.
 *
 *  This is a debugging function that will log all available
 *  information about the GGZdMod object.  It uses ggzdmod_log
 *  for logging.
 *
 *  @param ggzdmod The GGZdMod object.
 *  @return void; errors in ggzdmod_log are ignored.
 */
void ggzdmod_check(GGZdMod *ggzdmod);

#ifdef __cplusplus
}
#endif

#endif /* __GGZDMOD_H__ */
