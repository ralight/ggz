/*
 * File: options.h
 * Author: Jason Short
 * Project: GGZCards GGZ Server
 * Date: 07/06/2001
 * Desc: Functions and data for game options system
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

extern int options_set();
extern int rec_options(int, int*);

extern void add_option(char* key, int num, int dflt, ...);
extern void set_option(char* key, int value);
extern void get_options();
extern void handle_options();

extern void finalize_options();

