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

void _ggzcore_net_connect(const char* server, const unsigned int port);
void _ggzcore_net_disconnect(void);

int _ggzcore_net_ispending(void);
int _ggzcore_net_process(void);

void _ggzcore_net_send_login(GGZLoginType type, const char* login, const char* pass);
void _ggzcore_net_send_logout(void);

void _ggzcore_net_send_motd(void);

void _ggzcore_net_send_list_rooms(const int type, const char verbose);
void _ggzcore_net_send_join_room(const int room);

void _ggzcore_net_send_list_players(void);

void _ggzcore_net_send_chat(const GGZChatOp op, const char* player, 
			    const char* msg); 

#endif /* __NET_H__ */

