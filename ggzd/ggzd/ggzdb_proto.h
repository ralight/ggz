/*
 * File: ggzdb.h
 * Author: GGZ Development Team
 * Project: GGZ Server
 * Date: 09/08/2002
 * Desc: Back-end functions for handling database manipulation
 * $Id: ggzdb_proto.h 4480 2002-09-09 03:24:42Z jdorje $
 *
 * Copyright (C) 2002 GGZ Development Team.
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

int _ggzdb_init(ggzdbConnection connection, int standalone);

void _ggzdb_close(void);

void _ggzdb_enter(void);

void _ggzdb_exit(void);

int _ggzdb_init_player(char *datadir);

int _ggzdb_player_add(ggzdbPlayerEntry *);

int _ggzdb_player_get(ggzdbPlayerEntry *);

int _ggzdb_player_update(ggzdbPlayerEntry *);

int _ggzdb_player_get_first(ggzdbPlayerEntry *);

int _ggzdb_player_get_next(ggzdbPlayerEntry *);

void _ggzdb_player_drop_cursor(void);

unsigned int _ggzdb_player_next_uid(void);
