/*
 * File: net.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/22/00
 *
 * Code for performing network I/O
 *
 * Copyright (C) 2000 Brent Hendricks.
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


#ifndef __NET_H__
#define __NET_H__

#include <config.h>
#include <ggzcore.h>
#include <protocol.h>

void _ggzcore_net_init(void);
int _ggzcore_net_connect(const char* server, const unsigned int port);
void _ggzcore_net_disconnect(const unsigned int fd);

/* Functions for sending data/requests to server */
int _ggzcore_net_send_login(const unsigned int fd, 
			    GGZLoginType type, 
			    const char* login, 
			    const char* pass);
int _ggzcore_net_send_logout(const unsigned int fd);
int _ggzcore_net_send_motd(const unsigned int fd);
int _ggzcore_net_send_list_types(const unsigned int fd, 
				 const char verbose);
int _ggzcore_net_send_list_rooms(const unsigned int fd, 
				 const int type, 
				 const char verbose);
int _ggzcore_net_send_join_room(const unsigned int fd, 
				const int room);
int _ggzcore_net_send_list_players(const unsigned int fd);
int _ggzcore_net_send_list_tables(const unsigned int fd, 
				  const int type, 
				  const char global);
int _ggzcore_net_send_chat(const unsigned int fd, 
			   const GGZChatOp op,
			   const char* player, 
			   const char* msg); 

/* Functions for reasding data from server */
int _ggzcore_net_read_opcode(const unsigned int fd, GGZServerOp *op);
int _ggzcore_net_read_server_id(const unsigned int fd, int *protocol);
int _ggzcore_net_read_login_anon(const unsigned int fd, char *status);
int _ggzcore_net_read_motd(const unsigned int fd, int *lines, char ***buffer);
int _ggzcore_net_read_logout(const unsigned int fd, char *status);
int _ggzcore_net_read_num_rooms(const unsigned int fd, int *num);
int _ggzcore_net_read_room(const unsigned int fd, 
			   const char verbose,
			   int *id,
			   char **name,
			   int *game,
			   char **desc);

int _ggzcore_net_read_num_types(const unsigned int fd, int *num);
int _ggzcore_net_read_type(const unsigned int fd, struct _GGZGameType *type);

int _ggzcore_net_read_room_join(const unsigned int fd, char *status);
int _ggzcore_net_read_num_players(const unsigned int fd, int *num);
int _ggzcore_net_read_player(const unsigned int fd, char **name, int *table);

int _ggzcore_net_read_num_tables(const unsigned int fd, int *num);
int _ggzcore_net_read_table(const unsigned int fd, struct _GGZTable *table);

int _ggzcore_net_read_chat(const unsigned int fd, 
			   GGZChatOp *op, 
			   char **name, 
			   char **msg);
int _ggzcore_net_read_update_players(const unsigned int fd, 
				     GGZUpdateOp *op,
				     char **name);
int _ggzcore_net_read_update_tables(const unsigned int fd);
int _ggzcore_net_read_rsp_chat(const unsigned int fd, char *status);


#endif /* __NET_H__ */

