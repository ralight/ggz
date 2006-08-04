/*
 * File: nets.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 8/27/01
 * Desc: Functions for handling network IO
 * $Id: net.h 8474 2006-08-04 14:48:01Z josef $
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

#include "client.h"
#include "datatypes.h"
#include "login.h"
#include "players.h"
#include "protocols.h"
#include "room.h"


/* Create a new network IO object */
GGZNetIO* net_new(int fd, GGZClient *client);

/* Set dump file for session */
GGZReturn net_set_dump_file(GGZNetIO *net, const char* filename);

/* Get filedescriptor for communication */
int net_get_fd(GGZNetIO* net);

/* Read data from socket for a particular player */
GGZPlayerHandlerStatus net_read_data(GGZNetIO* net);

/* Disconnect from network */
void net_disconnect(GGZNetIO* net);

/* Free up resources used by net object */
void net_free(GGZNetIO* net);

/* Functions to send data to the client */
GGZReturn net_send_serverid(GGZNetIO *net, const char *srv_name, bool use_tls);
GGZReturn net_send_server_full(GGZNetIO *net, const char *srv_name);
GGZReturn net_send_login(GGZNetIO *net, GGZLoginType type,
			 GGZClientReqError status, const char *password);
GGZReturn net_send_motd(GGZNetIO *net);
GGZReturn net_send_motd_error(GGZNetIO *net, GGZClientReqError status);

GGZReturn net_send_room_list_error(GGZNetIO *net, GGZClientReqError status);
GGZReturn net_send_room_list_count(GGZNetIO *net, int count);
GGZReturn net_send_room(GGZNetIO *net, int index,
			RoomStruct *room, bool verbose);
GGZReturn net_send_room_list_end(GGZNetIO *net);

GGZReturn net_send_type_list_error(GGZNetIO *net, GGZClientReqError status);
GGZReturn net_send_type_list_count(GGZNetIO *net, int count);
GGZReturn net_send_type(GGZNetIO *net, int index,
			GameInfo *type, bool verbose);
GGZReturn net_send_type_list_end(GGZNetIO *net);

GGZReturn net_send_player_list_error(GGZNetIO *net, GGZClientReqError status);
GGZReturn net_send_player_list_count(GGZNetIO *net, int room_id, int count);
GGZReturn net_send_player(GGZNetIO *net, GGZPlayer *p2);
GGZReturn net_send_player_list_end(GGZNetIO *net);

GGZReturn net_send_table_list_error(GGZNetIO *net, GGZClientReqError status);
GGZReturn net_send_table_list_count(GGZNetIO *net, int room_id, int count);
GGZReturn net_send_table(GGZNetIO *net, GGZTable *table);
GGZReturn net_send_table_list_end(GGZNetIO *net);

GGZReturn net_send_room_join(GGZNetIO *net, GGZClientReqError status);
GGZReturn net_send_chat(GGZNetIO *net, GGZChatType type,
			const char *sender, const char *msg);
GGZReturn net_send_chat_result(GGZNetIO *net, GGZClientReqError status);
GGZReturn net_send_admin_result(GGZNetIO *net, GGZClientReqError status);
GGZReturn net_send_table_launch(GGZNetIO *net, GGZClientReqError status);
GGZReturn net_send_table_join(GGZNetIO *net, bool is_spectator,
			      const unsigned int table_index);
GGZReturn net_send_table_join_result(GGZNetIO *net, GGZClientReqError status);
GGZReturn net_send_table_leave(GGZNetIO *net, GGZLeaveType reason,
			       const char *player);
GGZReturn net_send_table_leave_result(GGZNetIO *net, GGZClientReqError status);
GGZReturn net_send_reseat_result(GGZNetIO *net, GGZClientReqError status);
GGZReturn net_send_player_update(GGZNetIO *net, GGZPlayerUpdateType opcode,
				 const char *name,
				 int room_id, int other_room_id);
GGZReturn net_send_table_update(GGZNetIO *net, GGZTableUpdateType opcode,
				GGZTable *table, void *seat_data,
				int room_id);
GGZReturn net_send_room_update(GGZNetIO *net, GGZRoomUpdateType opcode,
			       int index, int player_count, RoomStruct *room);
GGZReturn net_send_update_result(GGZNetIO *net, GGZClientReqError status);
GGZReturn net_send_logout(GGZNetIO *net, GGZClientReqError status);

GGZReturn net_send_ping(GGZNetIO *net);

GGZReturn net_send_info_list_begin(GGZNetIO *net);
GGZReturn net_send_info(GGZNetIO *net, int num, const char *realname,
			const char *photo, const char *host);
GGZReturn net_send_info_list_end(GGZNetIO *net);


#endif /* _GGZ_NET_H */
