/* 
 * File: io.h
 * Author: GGZ Dev Team
 * Project: ggzdmod
 * Date: 11/18/01
 * Desc: Functions for reading/writing messages from/to game modules
 * $Id: io.h 4912 2002-10-14 20:22:18Z jdorje $
 *
 * This file contains the backend for the ggzdmod library.  This
 * library facilitates the communication between the GGZ server (ggzd)
 * and game servers.  This file provides backend code that can be
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


#include "ggzmod.h"
#include "mod.h"

/* Functions for sending IO messages */
int _io_send_launch_begin(int fd, int num_seats, int num_spectator_seats,
			  int is_spectator, int seat_num);
int _io_send_state(int fd, GGZModState state);
int _io_send_server(int fd, int server_fd);
int _io_send_player(int fd, int is_spectator, int seat_num);
int _io_send_seat(int fd, GGZSeat *seat);
int _io_send_spectator_seat(int fd, GGZSpectatorSeat *seat);

/* Read and dispatch message */
int _io_read_data(GGZMod * ggzmod);

/* Functions for sending repsonses */
int _io_respond_launch(int fd, char status);
