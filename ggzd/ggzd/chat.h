/*
 * File: chat.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 5/10/00
 * Desc: Functions for handling/manipulating GGZ chat/messaging
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

#include <ggz_common.h>

#include "ggzd.h"
#include "players.h"


/* Queue up chat message for room consumption */
GGZReturn chat_room_enqueue(int room, unsigned char opcode, GGZPlayer* sender, 
			    char *msg);

/* Queue up a chat to a specific player */
GGZClientReqError chat_player_enqueue(char* receiver, unsigned char opcode, 
				      GGZPlayer* sender, char *msg);

/* Queue up a server announcement to a specific player's chat window */
GGZReturn chat_server_2_player(char *player_name, char *msg);
