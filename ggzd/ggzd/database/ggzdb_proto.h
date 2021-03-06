/*
 * Function prototypes for plugins for database manipulation (ggzdbplugin)
 * Copyright (C) 2002 - 2008 GGZ Development Team.
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

#include "ggzdb.h"

/** @brief Open the main database connection or file
 *
 *  @param connection Connection information from ggzd
 *  @param standalone If set, assume a single thread, do not require locking
 *
 *  @return GGZ_OK or GGZ_ERROR
 */
GGZReturn _ggzdb_init(ggzdbConnection connection, int standalone);

/** @brief Close the database */
void _ggzdb_close(void);

/** @brief Lock the database for exclusive access from one thread */
void _ggzdb_enter(void);

/** @brief Unlock the database access again */
void _ggzdb_exit(void);

/** @brief Try to add a player.
 *
 *  Attempt to add a new player to the database.  If the player already
 *  exists, it should return DUPKEY.
 *  @return GGZDB_NO_ERROR, GGZDB_ERR_DUPKEY, or GGZDB_ERR_DB
 */
GGZDBResult _ggzdb_player_add(ggzdbPlayerEntry * player);

/** @brief Try to retrieve a player by name.
 *
 *  This function is given a player name (in the player entry), and should
 *  retrieve a full entry for the player.  If the player does not exist it
 *  should return NOTFOUND.
 *  @return GGZDB_NO_ERROR, GGZDB_ERR_NOTFOUND, or GGZDB_ERR_DB
 */
GGZDBResult _ggzdb_player_get(ggzdbPlayerEntry * player);

/** @brief Update an existing player entry.
 *
 *  Update a player's information given the entry structure.  The player
 *  should already exist in the database, so the function should not fail.
 *  @return GGZDB_NO_ERROR or GGZDB_ERR_DB
 */
GGZDBResult _ggzdb_player_update(ggzdbPlayerEntry * player);

/** @brief Retrieve the *first* player entry in the database.
 *
 *  Given an empty player structure, this function should fill it in with
 *  data for the first player in the DB.  It should create a cursor of some
 *  sort so that subsequent calls to _ggzdb_player_get_next will "just work"
 *  and return the rest of the players.
 *  @return GGZDB_NO_ERROR or GGZDB_ERR_DB
 *  @todo What if there are no entries?
 *  @note The function does not have to be threadsafe.
 */
GGZDBResult _ggzdb_player_get_first(ggzdbPlayerEntry * player);

/** @brief Retrieve the *next* player entry in the database.
 *
 *  Given an empty player structure, this function should find and fill in the
 *  next player in the database.  The order is unimportant, but it is
 *  important that sequentially calling this function (after getting the
 *  first player with _ggzdb_player_get_first) finds all players.  If there
 *  are no more players, NOTFOUND should be returned.
 *  @return GGZDB_NO_ERROR, GGZDB_ERR_NOTFOUND, or GGZDB_ERR_DB
 *  @note The function does not have to be threadsafe or reentrant.
 */
GGZDBResult _ggzdb_player_get_next(ggzdbPlayerEntry * player);

/** @brief Try to retrieve more info about a player by name.
 *
 *  This function is given a player name (in the extended player entry),
 *  and returns additional information which might be available in the
 *  database.
 *  If the player does not exist it should return NOTFOUND.
 *  @return GGZDB_NO_ERROR, GGZDB_ERR_NOTFOUND, or GGZDB_ERR_DB
 */
GGZDBResult _ggzdb_player_get_extended(ggzdbPlayerExtendedEntry * player);

/** @brief Drop the current cursor used in retrieving all players.
 *
 *  This function can drop the cursor used by get_first and get_next.
 */
void _ggzdb_player_drop_cursor(void);


/** Look up game stats.  The "player" and "game" field should be already
 *  filled in; everything else is filled in by the function.  If the entry
 *  is not found, return GGZDB_ERR_NOTFOUND. 
 */
GGZDBResult _ggzdb_stats_lookup(ggzdbPlayerGameStats *stats);

/** Update game stats.  If the entry does not already exist, create it. */
GGZDBResult _ggzdb_stats_update(ggzdbPlayerGameStats *stats);

/** Create a new match statistics entry, possibly with a winner,
 *  and optionally with a saved game.
 */
GGZDBResult _ggzdb_stats_newmatch(const char *game, const char *winner, const char *savegame);

/** Register a savegame entry temporarily */
GGZDBResult _ggzdb_stats_savegame(const char *game, const char *owner, const char *savegame, ggzdbStamp tableid);

/** Add player entry to a match statistics entry */
GGZDBResult _ggzdb_stats_match(ggzdbPlayerGameStats *stats);

/** Report the top-N players for a certain game type */
GGZDBResult _ggzdb_stats_toprankings(const char *game, int number, ggzdbPlayerGameStats **rankings);

/** Recalculate statistics for a certain game type */
GGZDBResult _ggzdb_stats_calcrankings(const char *game);

/** Find out the next id for a player in non-SQL plugins; ignore for SQL */
unsigned int _ggzdb_player_next_uid(void);

/** Return a list of strings representing unfinished savegame filenames for
 *  a given game-type and table owner. Usually the list will only contain
 *  one entry, but not always. */
GGZList *_ggzdb_savegames(const char *game, const char *owner);

/** Return a list of ggzdbSavegamePlayers structures representing the owners
 *  of a savegame, which reference all unfinished savegames of a player. */
GGZList *_ggzdb_savegame_owners(const char *game);

/** Registers player information for a savegame so the table seats can be
 *  reserved for them. */
GGZDBResult _ggzdb_savegame_player(ggzdbStamp tableid, int seat, const char *name, int type);

/** Stores the ggzd room list into the database and flags them as file-based,
 *  as opposed to rooms which can be added and removed through GGZ Community. */
GGZDBResult _ggzdb_rooms(RoomStruct *rooms, int num);

/** Returns the file descriptor to listen on for changes in the SQL rooms
 *  database. Returning -1 means this is not supported. */
int _ggzdb_reconfiguration_fd(void);

/** Returns the list of rooms which were added or removed through GGZ Community. */
RoomStruct* _ggzdb_reconfiguration_room(void);

/** Triggers the reconfiguration fd on startup of ggzd to load up all rooms
 *  in the database before accepting connections. */
void _ggzdb_reconfiguration_load(void);

/** Carries out escaping on the provided string that is specific to the
 *  database module.  This is an optional function and if it is not provided
 *  then no escaping will be carried out.
 *
 *  @return The escaped string or NULL on error. The escaped string must be
 *  freed using free(), *not* using ggz_free().
 */
char *_ggzdb_escape(const char *str);

/** Removes the escaping added in _ggzdb_escape().  This is an optional
 *  function and if it is not provided then no unescaping will be carried out.
 *
 *  @return The unescaped string or NULL on error. The unescaped string must be
 *  freed using free(), *not* using ggz_free().
 */
char *_ggzdb_unescape(const char *str);
