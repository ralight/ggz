/*
 * File: chat.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 2/5/00
 * Desc: Functions for interfacing with the chat facility
 *
 * Copyright (C) 1999 Brent Hendricks.
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


void chat_init(void);
int  chat_add(int p_index, char* msg);
char chat_check_unread(int p_index, int c_index);
int  chat_check_num_unread(int p_index);
void chat_mark_read(int p_index, int c_index);
void chat_mark_all_read(int p_index);
void chat_get(int c_index, char* name, char* chat);

