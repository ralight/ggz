/*
 * File: options.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/06/2001
 * Desc: Functions and data for game options system
 * $Id: options.h 2190 2001-08-23 08:06:05Z jdorje $
 *
 * GGZCards has a rather nifty option system.  Each option has a name as
 * its "key".  Each option has a certain number of possible values, in
 * the integer range 0..(n-1).  Each value is linked to a text description.
 * All of these are created by the individual games using the interface
 * functions in this file.
 *
 * When the game starts, the server automatically asks the client (host
 * player) what options (s)he wants.  Options can also be passed in
 * through the command line to the server, and are handled at game start
 * time.
 *
 * Copyright (C) 2001 Brent Hendricks.
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-130
 */

#include <string.h>

int options_set();
int rec_options(int, int *);

void add_option(char *key, int num, int dflt, ...);
void set_option(char *key, int value);
void get_options();
void handle_options();

void finalize_options();
