/*
 * File: chat.h
 * Author: Justin Zaun
 * Project: GGZ Client
 * Date: 04/26/2000
 *
 * This fils contains function declarations for connecting with the server
 *
 * Copyright (C) 2000 Justin Zaun.
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


#include<gtk/gtk.h>

#define CHAT_COLOR_NONE 0
#define CHAT_COLOR_SOME 1
#define CHAT_COLOR_FULL  2

void display_chat(gchar *name, gchar *msg);
