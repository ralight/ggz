/* 
 * File: ggzmod.h
 * Author: GGZ Dev Team
 * Project: ggzmod
 * Date: 10/14/01
 * Desc: GGZ game module functions
 * $Id: ggzmod.h 6105 2004-07-15 16:43:00Z jdorje $
 *
 * This file contains the main interface for the ggzmod library.  This
 * library facilitates the communication between the GGZ server (ggz)
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

/** @file ggzmod.h
 *  @brief Common functions for interfacing a game server and GGZ.
 *
 * This file contains all libggzmod functions used by game servers to
 * interface with GGZ (and vice versa).  Just include ggzmod.h and make sure
 * your program is linked with libggzmod.  Then use the functions below as
 * appropriate.
 *
 * GGZmod currently provides an event-driven interface.  Data from
 * communication sockets is read in by the library, and a handler function
 * (registered as a callback) is invoked to handle any events.  The calling
 * program should not read/write data from/to the GGZ socket unless it really
 * knows what it is doing.
 *
 * That this does not apply to the client sockets: ggzmod provides
 * one file desriptor for communicating (TCP) to each client.  If data
 * is ready to be read by one of these file descriptors ggzmod may
 * invoke the appropriate handler (see below), but will never actually
 * read any data.
 *
 * For more information, see the documentation at http://ggz.sf.net/.
 */


#ifndef __GGZMOD_H__
#define __GGZMOD_H__

#define GGZMOD_VERSION_MAJOR 0
#define GGZMOD_VERSION_MINOR 0
#define GGZMOD_VERSION_MICRO 8
#define GGZMOD_VERSION_IFACE "1:1:0"

#include <ggz_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Table states
 *
 *  Each table has a current "state" that is tracked by ggzmod.  First
 *  the table is executed and begins running.  Then it receives a launch
 *  event from GGZ and begins waiting for players.  At some point a game
 *  will be started and played at the table, after which it may return
 *  to waiting.  Eventually the table will probably halt and then the
 *  program will exit.
 *
 *  More specifically, the game is in the CREATED state when it is first
 *  executed.  It moves to the WAITING state after GGZ first communicates
 *  with it.  After this, the game server may use ggzmod_set_state to
 *  change between WAITING, PLAYING, and DONE states.  A WAITING game is
 *  considered waiting for players (or whatever), while a PLAYING game is
 *  actively being played (this information may be, but currently is not,
 *  propogated back to GGZ for display purposes).  Once the state is changed
 *  to DONE, the table is considered dead and will exit shortly
 *  thereafter (ggzmod_loop will stop looping, etc.) (see the kill_on_exit
 *  game option).
 *
 *  Each time the game state changes, a GGZMOD_EVENT_STATE event will be
 *  propogated to the game server.
 */
typedef enum {
	GGZMOD_STATE_CREATED,	/**< Pre-launch; waiting for ggzmod */
	GGZMOD_STATE_WAITING,	/**< Ready and waiting to play. */
	GGZMOD_STATE_PLAYING,	/**< Currently playing a game. */
	GGZMOD_STATE_DONE	/**< Table halted, prepping to exit. */
} GGZModState;

/** @brief Callback events.
 *
 *  Each of these is a possible GGZmod event.  For each event, the
 *  table may register a handler with GGZmod to handle that
 *  event.
 *  @see GGZModHandler
 *  @see ggzmod_set_handler
 */
typedef enum {
	/** @brief Module status changed
	 *  This event occurs when the game's status changes.  The old
	 *  state (a GGZModState*) is passed as the event's data.
	 *  @see GGZModState */
	GGZMOD_EVENT_STATE,

	/** @brief A new server connection has been made 
	 * This event occurs when a new connection to the game server
	 * has been made.  The fd is passed as the event's data. */
	GGZMOD_EVENT_SERVER,

	/** @brief The player's seat status has changed.
	 *
	 *  This event occurs when the player's seat status changes;
	 *  i.e. he changes seats or starts/stops spectating.  The
	 *  event data is a int[2] pair consisting of the old
	 *  {is_spectator, seat_num}. */
	GGZMOD_EVENT_PLAYER,

	/** @brief A seat change.
	 *
	 *  This event occurs when a seat change occurs.  The old seat
	 *  (a GGZSeat*) is passed as the event's data.  The seat
	 *  information will be updated before the event is invoked. */
	GGZMOD_EVENT_SEAT,

	/** @brief A spectator seat change.
	 *
	 *  This event occurs when a spectator seat change occurs.  The
	 *  old spectator (a GGZSpectator*) is passed as the event's data.
	 *  The spectator information will be updated before the event is
	 *  invoked. */
	GGZMOD_EVENT_SPECTATOR_SEAT,

	/** @brief A chat message event.
	 *
	 *  This event occurs when we receive a chat.  The chat may have
	 *  originated in another game client or from the GGZ client; in
	 *  either case it will be routed to us.  The chat information
	 *  (a GGZChat*) is passed as the event's data.  Note that the
	 *  chat may originate with a player or a spectator, and they may
	 *  have changed seats or left the table by the time it gets to
	 *  us. */
	GGZMOD_EVENT_CHAT,

	/** A player's stats have been updated.
	 *  @see ggzmod_player_get_record
	 *  @see ggzmod_player_get_rating
	 *  @see ggzmod_player_get_ranking
	 *  @see ggzmod_player_get_highscore
	 *  @param data The name of the player whose stats have changed. */
	GGZMOD_EVENT_STATS,
	
	/** @brief An error has occurred
	 *  This event occurs when a GGZMod error has occurred.  An
	 *  error message (a char*) will be passed as the event's data.
	 *  GGZMod may attempt to recover from the error, but it is
	 *  not guaranteed that the GGZ connection will continue to
	 *  work after an error has happened. */
	GGZMOD_EVENT_ERROR		
} GGZModEvent;

/** @brief The "type" of ggzmod.
 *
 * The "flavor" of GGZmod object this is.  Affects what operations are
 * allowed.
 */
typedef enum {
	GGZMOD_GGZ,	/**< Used by the ggz client ("ggz"). */
	GGZMOD_GAME	/**< Used by the game client ("table"). */
} GGZModType;

/** @brief A seat at a GGZ game table.
 *
 *  Each seat at the table is tracked like this.
 */
typedef struct {
	unsigned int num;	/**< Seat index; 0..(num_seats-1). */
	GGZSeatType type;	/**< Type of seat. */
	const char * name;	/**< Name of player occupying seat. */
} GGZSeat;

/** @brief A game spectator entry.
 *
 * Spectators are kept in their own table.  A spectator seat is occupied
 * if it has a name, empty if the name is NULL.
 */
typedef struct {
	unsigned int num;	/**< Spectator seat index */
	const char * name;	/**< The spectator's name (NULL => empty) */
} GGZSpectatorSeat;

typedef struct {
	const char *player;
	const char *message;
} GGZChat;

/** @brief A GGZmod object, used for tracking a ggz<->table connection.
 *
 * A game client should track a pointer to a GGZMod object; it
 * contains all the state information for communicating with GGZ.  The
 * GGZ client will track one such object for every game table that is
 * running.  */
typedef struct GGZMod GGZMod;

/** @brief Event handler prototype
 *
 *  A function of this type will be called to handle a ggzmod event.
 *  @param mod The ggzmod state object.
 *  @param e The event that has occured.
 *  @param data Pointer to additional data for the event.
 *  The additional data will be of the following form:
 *    - GGZMOD_EVENT_STATE: The old state (GGZModState*)
 *    - GGZMOD_EVENT_SERVER: The fd of the server connection (int*)
 *    - GGZMOD_EVENT_PLAYER: The old player data (int[2])
 *    - GGZMOD_EVENT_SEAT: The old seat (GGZSeat*)
 *    - GGZMOD_EVENT_SPECTATOR_SEAT: The old seat (GGZSpectatorSeat*)
 *    - GGZMOD_EVENT_ERROR: An error string (char*)
 */
typedef void (*GGZModHandler) (GGZMod * mod, GGZModEvent e, void *data);

/* 
 * Creation functions
 */

/** @brief Create a new ggzmod object.
 *
 *  Before connecting through ggzmod, a new ggzmod object is needed.
 *  @param type The type of ggzmod.  Should be GGZMOD_GAME for game servers.
 *  @see GGZModType
 */
GGZMod *ggzmod_new(GGZModType type);

/** @brief Destroy a finished ggzmod object.
 *
 *  After the connection is through, the object may be freed.
 *  @param ggzmod The GGZMod object.
 */
void ggzmod_free(GGZMod * ggzmod);

/* 
 * Accessor functions
 */

/** @brief Get the file descriptor for the GGZMod socket.
 *
 *  @param ggzmod The GGZMod object.
 *  @return GGZMod's main ggz <-> table socket FD.
 */
int ggzmod_get_fd(GGZMod * ggzmod);

/** @brief Get the type of the ggzmod object.
 *  @param ggzmod The GGZMod object.
 *  @return The type of the GGZMod object (GGZ or GAME).
 */
GGZModType ggzmod_get_type(GGZMod * ggzmod);

/** @brief Get the current state of the table.
 *  @param ggzmod The GGZMod object.
 *  @return The state of the table.
 */
GGZModState ggzmod_get_state(GGZMod * ggzmod);

/** @brief Get the fd of the game server connection
 *  @param ggzmod The GGZMod object.
 *  @return The server connection fd
 */
int ggzmod_get_server_fd(GGZMod * ggzmod);

/** @brief Get the total number of seats at the table.
 *  @return The number of seats, or -1 on error.
 *  @note If no connection is present, -1 will be returned.
 *  @note While in GGZMOD_STATE_CREATED, we don't know the number of seats.
 */
int ggzmod_get_num_seats(GGZMod *ggzmod);

/** @brief Get all data for the specified seat.
 *  @param ggzmod The GGZMod object.
 *  @param seat The seat number (0..(number of seats - 1)).
 *  @return A valid GGZSeat structure, if seat is a valid seat.
 */
GGZSeat ggzmod_get_seat(GGZMod *ggzmod, int seat);

/** @brief Get the maximum number of spectators.
 *  This function returns the maximum number of spectator seats available.  A
 *  game can use this to iterate over the spectator seats to look for
 *  spectators occupying them.  Since spectators may come and go at any point
 *  and there is no limit on the number of spectators, you should consider
 *  this value to be dynamic and call this function again each time you're
 *  looking for spectators.
 *  @return The number of available spectator seats, or -1 on error.
 */
int ggzmod_get_num_spectator_seats(GGZMod *ggzmod);

/** @brief Get a spectator's data.
 *  @param ggzmod The GGZdMod object.
 *  @param seat The number, between 0 and (number of spectators - 1).
 *  @return A valid GGZSpectator structure, if given a valid seat.
 */
GGZSpectatorSeat ggzmod_get_spectator_seat(GGZMod * ggzmod, int seat);

/** @brief Get data about this player.
 *
 *  Call this function to find out where at the table this player is
 *  sitting.
 *  @param ggzmod The GGZMod object.
 *  @param is_spectator Will be set to TRUE iff player is spectating.
 *  @param seat_num Will be set to the number of our (spectator) seat.
 *  @return The name of the player (or NULL on error).
 */
const char * ggzmod_get_player(GGZMod *ggzmod,
			       int *is_spectator, int *seat_num);

/** @brief Return gamedata pointer
 *
 *  Each GGZMod object can be given a "gamedata" pointer that is returned
 *  by this function.  This is useful for when a single process serves
 *  multiple GGZmod's.
 *  @param ggzmod The GGZMod object.
 *  @return A pointer to the gamedata block (or NULL if none).
 *  @see ggzmod_set_gamedata */
void * ggzmod_get_gamedata(GGZMod * ggzmod);

/** @brief Set gamedata pointer
 *  @param ggzmod The GGZMod object.
 *  @param data The gamedata block (or NULL for none).
 *  @see ggzmod_get_gamedata */
void ggzmod_set_gamedata(GGZMod * ggzmod, void * data);

/** @brief Set a handler for the given event.
 *
 *  As described above, GGZmod uses an event-driven structure.  Each
 *  time an event is called, the event handler (there can be only one)
 *  for that event will be called.  This function registers such an
 *  event handler.
 *  @param ggzmod The GGZmod object.
 *  @param e The GGZmod event.
 *  @param func The handler function being registered.
 *  @see ggzmod_get_gamedata
 */
void ggzmod_set_handler(GGZMod * ggzmod, GGZModEvent e, GGZModHandler func);


/* 
 * Event/Data handling
 */

/** @brief Check for and handle input.
 *
 *  This function handles input from the communications sockets:
 *    - It will check for input, but will not block.
 *    - It will monitor input from the GGZmod socket.
 *    - It will monitor input from player sockets only if a handler is
 *      registered for the PLAYER_DATA event.
 *    - It will call an event handler as necessary.
 *  @param ggzmod The ggzmod object.
 *  @return -1 on error, the number of events handled (0-1) on success.
 */
int ggzmod_dispatch(GGZMod * ggzmod);

/* 
 * Control functions
 */

/** @brief Change the table's state.
 *
 *  This function should be called to change the state of a table.
 *  A game can use this function to change state between WAITING
 *  and PLAYING, or to set it to DONE.
 *  @param ggzmod The ggzmod object.
 *  @param state The new state.
 *  @return 0 on success, -1 on failure/error.
 */
int ggzmod_set_state(GGZMod * ggzmod, GGZModState state);

/** @brief Connect to ggz.
 *
 *  Call this function to make an initial GGZ <-> game connection.
 *  - When called by the game server, this function makes the
 *    physical connection to ggz.
 *  - When called by ggz, it will launch a table and connect
 *    to it.
 *  @param ggzmod The ggzmod object.
 *  @return 0 on success, -1 on failure.
 */
int ggzmod_connect(GGZMod * ggzmod);

/** @brief Disconnect from ggz.
 *
 *  - When called by the game server, this function stops
 *    the connection to GGZ.  It should only be called
 *    when the table is ready to exit.
 *  - When called by the GGZ server, this function will
 *    kill and clean up after the table.
 *  @param ggzmod The ggzmod object.
 *  @return 0 on success, -1 on failure.
 */
int ggzmod_disconnect(GGZMod * ggzmod);


void ggzmod_request_stand(GGZMod * ggzmod);
void ggzmod_request_sit(GGZMod * ggzmod, int seat_num);
void ggzmod_request_boot(GGZMod * ggzmod, const char *name);
void ggzmod_request_bot(GGZMod * ggzmod, int seat_num);
void ggzmod_request_open(GGZMod * ggzmod, int seat_num);
void ggzmod_request_chat(GGZMod *ggzmod, const char *chat_msg);

/** @brief Get the player's win-loss record.
 *  @return TRUE if there is a record; FALSE if not or on error.
 */
int ggzmod_player_get_record(GGZSeat *player,
			     int *wins, int *losses,
			     int *ties, int *forfeits);

/** @brief Get the player's rating.
 *  @return TRUE if there is a rating; FALSE if not or on error.
 */
int ggzmod_player_get_rating(GGZSeat *player, int *rating);

/** @brief Get the player's ranking.
 *  @return TRUE if there is a ranking; FALSE if not or on error.
 */
int ggzmod_player_get_ranking(GGZSeat *player, int *ranking);

/** @brief Get the player's highscore.
 *  @return TRUE if there is a highscore; FALSE if not or on error.
 */
int ggzmod_player_get_highscore(GGZSeat *player, long *highscore);

#ifdef __cplusplus
}
#endif

#endif /* __GGZMOD_H__ */
