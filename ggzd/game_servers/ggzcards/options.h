/* 
 * File: options.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/06/2001
 * Desc: Functions and data for game options system
 * $Id: options.h 2418 2001-09-09 03:42:21Z jdorje $
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

#include <config.h>		/* Site-specific config */

#include <string.h>

/** @return TRUE iff all options have been set.
 *  Options are set after get_options is called and then (if
 *  necessary) handle_options handle's the client's response.
 *  @note Only needed by common code. */
int options_set();

/** Reads a list of options from the client as sent by the
 *  option protocol.
 *  @param num_options the number of options in the list
 *  @param options array options are placed in
 *  @return 0 on success, -1 on failure
 *  @note Only called by common code.
 *  @see handle_options */
int rec_options(int num_options, int *options);

/** adds an option to the list of pending options
 *  @param key the key string for the option
 *  @param num the number of values [0..num-1] the option can have
 *  @param dflt the default value of the option
 *  @param ... the list of option strings, one for each value
 *  @see game_get_options */
void add_option(char *key, int num, int dflt, ...);

/** sets an option
 *  @param key the key string of the option
 *  @value the value to which the option shall be set
 *  @note Only called by common code. */
void set_option(char *key, int value);

/** Request the current list of options from the client.
 *  Uses the specificed option protocol for asking the host
 *  what options to use.  Options that have already been set
 *  are not asked about.
 *  @return Returns (void) immediately, upon success or failure.
 *  @note If there are no pending options, no request is sent.
 *  @note Only called by common code. */
void get_options();

/** Receives options from the host client and puts them in the option list.
 *  Should be called when an option message is received in response
 *  to a get_options() option request.
 *  @note Only called by common code. */
void handle_options();

/** Calls the game's option handler for each option.  Must be
 *  called manually at the start of a game.
 *  @note Only called by common code. */
void finalize_options();
