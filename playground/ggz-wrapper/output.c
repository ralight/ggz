/*
 * File: output.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/26/00
 * $Id: output.c 6299 2004-11-07 16:28:09Z phh $
 *
 * Functions for display text/messages
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

#include <config.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ggzcore.h>

#include "output.h"
#include "server.h"
void output_text(char* fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	if(!fmt)
		return;
	vprintf(strcat(strdup(fmt),strdup("\n")), list);
	va_end(list);

}

void output_rooms(void)
{
	int i, num;
	GGZRoom *room;
	GGZGameType *type;

	num = ggzcore_server_get_num_rooms(server);

	for (i = 0; i < num; i++) {
		room = ggzcore_server_get_nth_room(server, i);
		type = ggzcore_room_get_gametype(room);
		if (type && strcmp(ggzcore_gametype_get_name(type), "TicTacToe")==0) {
			output_text("-- Room %d:%s(%s)", i,
				    ggzcore_room_get_name(room),
				    ggzcore_gametype_get_name(type));
		}
	}
}

void output_types(void)
{
	int i, num;
	GGZGameType *type;
	num = ggzcore_server_get_num_gametypes(server);
	for (i = 0; i < num; i++) {
		type = ggzcore_server_get_nth_gametype(server, i);
		output_text("-- Gametype %d:%s", i,
		ggzcore_gametype_get_name(type));
	}
}

