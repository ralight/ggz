/* 
 * GGZDMOD - C implementation of the GGZ server-server protocol
 *
 * Copyright (C) 2001 - 2008 GGZ Development Team.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/* This file contains functions for reading/writing messages from/to game
 * servers. It implements the GGZ server-server protocol including all the
 * messages described in protocol.h.
 */

#ifndef GGZDMOD_IO_H
#define GGZDMOD_IO_H

#include "ggzdmod.h"
#include "mod.h"

/* Functions for sending IO messages */
int _io_send_launch(int fd, const char *game, int seats, int spectators);
int _io_send_seat_change(int fd, const GGZSeat *seat);
int _io_send_spectator_change(int fd, const GGZSeat *spectator);
int _io_send_state(int fd, GGZdModState state);
int _io_send_seat(int fd, const GGZSeat *seat);
int _io_send_reseat(int fd,
		    int old_seat, int was_spectator,
		    int new_seat, int is_spectator);
int _io_send_savedgame(int fd, const char *savedgame);

int _io_send_log(int fd, const char *msg);
int _io_send_game_report(int fd, int num_players,
			 const char * const *names, const GGZSeatType *types,
			 const int *teams, const GGZGameResult *results,
			 const int *scores);
int _io_send_savegame_report(int fd, const char *savegame);
int _io_send_req_num_seats(int fd, int num_seats);
int _io_send_req_boot(int fd, const char *name);
int _io_send_req_bot(int fd, int seat_num);
int _io_send_req_open(int fd, int seat_num);

/* Read and dispatch message */
int _io_read_data(GGZdMod * ggzdmod);

/* Functions for sending repsonses */
int _io_respond_launch(int fd, char status);
int _io_respond_state(int fd);

#endif
