/*
 * File: nets.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 8/27/01
 * Desc: Functions for handling network IO
 *
 * Copyright (C) 1999-2001 Brent Hendricks.
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


#ifndef _GGZ_NET_H
#define _GGZ_NET_H

#include <config.h>
#include <players.h>
#include <login.h>
#include <room.h>
#include <datatypes.h>

typedef struct _GGZNetIO GGZNetIO;

/* Create a new network IO object */
GGZNetIO* net_new(int fd, GGZPlayer *player);

/* Set dump file for session */
int net_set_dump_file(GGZNetIO *net, const char* filename);

/* Get filedescriptor for communication */
int net_get_fd(GGZNetIO* net);

/* Read data from socket for a particular player */
int net_read_data(GGZNetIO* net);

/* Disconnect from network */
void net_disconnect(GGZNetIO* net);

/* Free up resources used by net object */
void net_free(GGZNetIO* net);

/* Functions to send data to the client */
int net_send_serverid(GGZNetIO *net);
int net_send_server_full(GGZNetIO *net);
int net_send_login(GGZNetIO *net, GGZLoginType type, char status, char *password);
int net_send_motd(GGZNetIO *net);
int net_send_motd_error(GGZNetIO *net, char status);

int net_send_room_list_error(GGZNetIO *net, char status);
int net_send_room_list_count(GGZNetIO *net, int count);
int net_send_room(GGZNetIO *net, int index, RoomStruct *room, char verbose);
int net_send_room_list_end(GGZNetIO *net);

int net_send_type_list_error(GGZNetIO *net, char status);
int net_send_type_list_count(GGZNetIO *net, int count);
int net_send_type(GGZNetIO *net, int index, GameInfo *type, char verbose);
int net_send_type_list_end(GGZNetIO *net);

int net_send_player_list_error(GGZNetIO *net, char status);
int net_send_player_list_count(GGZNetIO *net, int count);
int net_send_player(GGZNetIO *net, GGZPlayer *p2);
int net_send_player_list_end(GGZNetIO *net);

int net_send_table_list_error(GGZNetIO *net, char status);
int net_send_table_list_count(GGZNetIO *net, int count);
int net_send_table(GGZNetIO *net, GGZTable *table);
int net_send_table_list_end(GGZNetIO *net);

int net_send_room_join(GGZNetIO *net, char status);
int net_send_chat(GGZNetIO *net, unsigned char opcode, char *name, char *msg);
int net_send_chat_result(GGZNetIO *net, char status);
int net_send_table_launch(GGZNetIO *net, char status);
int net_send_table_join(GGZNetIO *net, char status);
int net_send_table_leave(GGZNetIO *net, char status);
int net_send_player_update(GGZNetIO *net, unsigned char opcode, char *name);
int net_send_table_update(GGZNetIO *net, unsigned char opcode, GGZTable *table, int seat);
int net_send_logout(GGZNetIO *net, char status);

int net_send_game_data(GGZNetIO *net, int size, char *data);


#endif /* _GGZ_NET_H */
