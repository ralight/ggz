/* 
 * File: io.h
 * Author: GGZ Dev Team
 * Project: ggzmod
 * Date: 11/18/01
 * Desc: Functions for reading/writing messages from/to game modules
 * $Id: io.h 7046 2005-03-26 09:51:15Z josef $
 *
 * This file contains the backend for the ggzmod library.  This
 * library facilitates the communication between the GGZ core client (ggz)
 * and game clients.  This file provides backend code that can be
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
int _io_send_launch(int fd);
int _io_send_state(int fd, GGZModState state);
int _io_send_server(int fd, const char *host, unsigned int port,
		    const char *handle);
int _io_send_player(int fd, const char *name, int is_spectator, int seat_num);
int _io_send_seat(int fd, GGZSeat *seat);
int _io_send_spectator_seat(int fd, GGZSpectatorSeat *seat);
int _io_send_msg_chat(int fd, const char *player, const char *chat_msg);
int _io_send_stats(int fd, int num_players, GGZStat *player_stats,
		   int num_spectators, GGZStat *spectator_stats);

int _io_send_req_stand(int fd);
int _io_send_req_sit(int fd, int seat_num);
int _io_send_req_boot(int fd, const char *name);
int _io_send_request_bot(int fd, int seat_num);
int _io_send_request_open(int fd, int seat_num);
int _io_send_request_chat(int fd, const char *chat_msg);

/* Read and dispatch message */
int _io_read_data(GGZMod * ggzmod);

/* Functions for sending repsonses */
int _io_respond_launch(int fd, char status);
