/*
 * File: room.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/20/00
 * Desc: Functions for interfacing with the room and chat facility
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

extern RoomStruct *chat_room;
extern RoomInfo room_info;

extern void room_initialize(void);
extern void room_create_additional(void);
extern int room_join(const int, const int, const int);
extern int room_handle_request(const int, const int, const int);
