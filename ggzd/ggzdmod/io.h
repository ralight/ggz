/* 
 * File: io.h
 * Author: GGZ Dev Team
 * Project: ggzdmod
 * Date: 11/18/01
 * Desc: Functions for reading/writing messages from/to game modules
 * $Id: io.h 6113 2004-07-16 17:44:10Z jdorje $
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


#include "ggzdmod.h"
#include "mod.h"

/* Functions for sending IO messages */
int _io_send_launch(int fd, int seats, int spectators);
int _io_send_seat_change(int fd, GGZSeat *seat);
int _io_send_spectator_change(int fd, GGZSpectator *spectator);
int _io_send_state(int fd, GGZdModState state);
int _io_send_seat(int fd, GGZSeat *seat);
int _io_send_reseat(int fd,
		    int old_seat, int was_spectator,
		    int new_seat, int is_spectator);

int _io_send_log(int fd, char *msg);
int _io_send_game_report(int fd, int num_players,
			 char **names, GGZSeatType *types,
			 int *teams, GGZGameResult *results,
			 int *scores);
int _io_send_req_num_seats(int fd, int num_seats);
int _io_send_req_boot(int fd, const char *name);
int _io_send_req_bot(int fd, int seat_num);
int _io_send_req_open(int fd, int seat_num);

/* Read and dispatch message */
int _io_read_data(GGZdMod * ggzdmod);

/* Functions for sending repsonses */
int _io_respond_launch(int fd, char status);
int _io_respond_state(int fd);
