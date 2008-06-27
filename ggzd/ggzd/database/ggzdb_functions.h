/*
 * Plugin calls to back-end functions for handling database manipulation
 * (ggzdb <-> ggzdbplugins)
 * Copyright (C) 2007, 2008 GGZ Development Team.
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

/* For documentation, see the corresponding functions in ggzdb_proto.h */

GGZReturn (*_ggzdb_init)(ggzdbConnection connection, int standalone);
void (*_ggzdb_close)(void);
void (*_ggzdb_enter)(void);
void (*_ggzdb_exit)(void);

GGZDBResult (*_ggzdb_init_player)(void);
GGZDBResult (*_ggzdb_player_add)(ggzdbPlayerEntry * player);
GGZDBResult (*_ggzdb_player_get)(ggzdbPlayerEntry * player);
GGZDBResult (*_ggzdb_player_update)(ggzdbPlayerEntry * player);
GGZDBResult (*_ggzdb_player_get_first)(ggzdbPlayerEntry * player);
GGZDBResult (*_ggzdb_player_get_next)(ggzdbPlayerEntry * player);
GGZDBResult (*_ggzdb_player_get_extended)(ggzdbPlayerExtendedEntry * player);

unsigned int (*_ggzdb_player_next_uid)(void);
void (*_ggzdb_player_drop_cursor)(void);

GGZDBResult (*_ggzdb_init_stats)(void);
GGZDBResult (*_ggzdb_stats_lookup)(ggzdbPlayerGameStats *stats);
GGZDBResult (*_ggzdb_stats_update)(ggzdbPlayerGameStats *stats);
GGZDBResult (*_ggzdb_stats_newmatch)(const char *game, const char *winner, const char *savegame);
GGZDBResult (*_ggzdb_stats_savegame)(const char *game, const char *owner, const char *savegame, ggzdbStamp tableid);
GGZDBResult (*_ggzdb_stats_match)(ggzdbPlayerGameStats *stats);
GGZDBResult (*_ggzdb_stats_toprankings)(const char *game, int number, ggzdbPlayerGameStats **rankings);
GGZDBResult (*_ggzdb_stats_calcrankings)(const char *game);

GGZList* (*_ggzdb_savegames)(const char *game, const char *owner);
GGZList* (*_ggzdb_savegame_owners)(const char *game);

GGZDBResult (*_ggzdb_savegame_player)(ggzdbStamp tableid, int seat, const char *name, int type);

GGZDBResult (*_ggzdb_rooms)(RoomStruct *rooms, int num);

int (*_ggzdb_reconfiguration_fd)(void);
RoomStruct* (*_ggzdb_reconfiguration_room)(void);
void (*_ggzdb_reconfiguration_load)(void);

