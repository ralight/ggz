/*
 * File: server.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 1/19/01
 *
 * Code for handling server connection state and properties
 *
 * Copyright (C) 2001 Brent Hendricks.
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

#ifndef __SERVER_H__
#define __SERVER_H__

#include "ggzcore.h"
#include "gametype.h"
#include "room.h"


void _ggzcore_server_list_players(struct _GGZServer *server);

void _ggzcore_server_list_tables(struct _GGZServer *server, 
				 const int type, 
				 const char global);

struct _GGZRoom* _ggzcore_server_get_room_by_id(struct _GGZServer *server,
						const unsigned int id);

struct _GGZGameType* _ggzcore_server_get_type_by_id(struct _GGZServer *server,
						    const unsigned int id);

void _ggzcore_server_chat(struct _GGZServer *server, 
			  const GGZChatOp opcode,
			  const char *player,
			  const char *msg);


#endif /* __SERVER_H__ */
