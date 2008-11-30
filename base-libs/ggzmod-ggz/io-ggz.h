/*
 * GGZMOD-GGZ - C implementation of the GGZ client-client protocol (GGZ side).
 * This file is part of the package ggz-base-libs.
 *
 * io-ggz.h: Reading/writing messages from/to game modules, GGZ side.
 *
 * Copyright (C) 2001 - 2008 GGZ Gaming Zone Development Team
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

/*
 * This file contains the backend for the ggzmod library.  This
 * library facilitates the communication between the GGZ core client (ggz)
 * and game clients.  This file provides backend code that can be
 * used at both ends.
 */

#ifndef __GGZ_IO_GGZ_H__
#define __GGZ_IO_GGZ_H__

#include "ggzmod-ggz.h"
#include "mod-ggz.h"

/* Functions for sending IO messages (ggz+game) */
int _io_ggz_send_state(int fd, GGZModState state);

/* Functions for sending IO messages (ggz only) */
int _io_ggz_send_launch(int fd);
int _io_ggz_send_server(int fd, const char *host, unsigned int port,
		    const char *handle);
int _io_ggz_send_server_fd(int fd, int server_fd);
int _io_ggz_send_player(int fd, const char *name, int is_spectator, int seat_num);
int _io_ggz_send_seat(int fd, GGZSeat *seat);
int _io_ggz_send_spectator_seat(int fd, GGZSpectatorSeat *seat);
int _io_ggz_send_msg_chat(int fd, const char *player, const char *chat_msg);
int _io_ggz_send_stats(int fd, int num_players, GGZStat *player_stats,
		   int num_spectators, GGZStat *spectator_stats);
int _io_ggz_send_msg_info(int fd, int num, GGZList *infos);
int _io_ggz_send_msg_rankings(int fd, int num, GGZList *rankings);

/* Read and dispatch message */
int _io_ggz_read_data(GGZMod * ggzmod);

#endif /* __GGZ_IO_GGZ_H__ */
