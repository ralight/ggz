/*
 * File: parse_opt.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/29/99
 * Desc: Parse command-line arguments and conf file
 *
 * Copyright (C) 1999 Brent Hendricks.
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

#include "room.h"

/* Parse command-line options */
void parse_args(int argc, char *argv[]);

/* Parse options from conf file, but don't overwrite existing options*/
void parse_conf_file(void);

/* Main entry point for parsing the game files */
void parse_game_files(void);

/* Main entry point for parsing the room files */
void parse_room_files(void);

/* Dynamic room reconfiguration */
void parse_room_change(const char *room);
void parse_room_change_db(RoomStruct *rooms);

