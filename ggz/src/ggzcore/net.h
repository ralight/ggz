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

struct _GGZNet* _ggzcore_net_new(void);
void _ggzcore_net_init(struct _GGZNet *net, 
		       struct _GGZServer *server, 
		       const char *host,
		       unsigned int port);

void _ggzcore_net_free(struct _GGZNet *net);

char*        _ggzcore_net_get_host(struct _GGZNet *net);
unsigned int _ggzcore_net_get_port(struct _GGZNet *net);
int          _ggzcore_net_get_fd(struct _GGZNet *net);

int _ggzcore_net_connect(struct _GGZNet *net);

void _ggzcore_net_disconnect(const unsigned int fd);

/* Functions for sending data/requests to server */
int _ggzcore_net_send_login(const unsigned int fd, 
			    GGZLoginType type, 
			    const char* login, 
			    const char* pass);
int _ggzcore_net_send_logout(const unsigned int fd);
int _ggzcore_net_send_motd(const unsigned int fd);
int _ggzcore_net_send_list_types(struct _GGZNet *net, const char verbose);
int _ggzcore_net_send_list_rooms(struct _GGZNet *net,
				 const int type,
				 const char verbose);
int _ggzcore_net_send_join_room(struct _GGZNet *net, 
				const unsigned int room_num);

int _ggzcore_net_send_list_players(const unsigned int fd);
int _ggzcore_net_send_list_tables(const unsigned int fd, 
				  const int type, 
				  const char global);
int _ggzcore_net_send_chat(const unsigned int fd, 
			   const GGZChatOp op,
			   const char* player, 
			   const char* msg); 

int _ggzcore_net_send_table_launch(const unsigned int fd,
				   const int type,
				   char *desc,
				   const int num_seats);
int _ggzcore_net_send_table_join(const unsigned int fd, const unsigned int num);
int _ggzcore_net_send_table_leave(const unsigned int fd);

int _ggzcore_net_send_seat(const unsigned int fd, 
			   GGZSeatType seat, 
			   char *name);
int _ggzcore_net_send_game_data(const unsigned int fd, int size, char *buffer);

/* Functions for reasding data from server */
int _ggzcore_net_read_data(struct _GGZNet *net);



#endif /* __NET_H__ */

