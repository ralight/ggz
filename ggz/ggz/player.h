/*
 * File: player.h
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 6/5/00
 *
 * This fils contains functions for handling players
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#include <glib.h>

/* 
 * The Player structure is meant to be a node in a linked list of
 * the players in the current room .
 */
typedef struct Player {
	
	/* Name of player */
	gchar* name;	

	/* Index of table at which this player is "sitting" */
	gint table;

	/* Color to use when display chat messages from this player */
	gint chat_color;
} Player;


void player_list_clear(void);

void player_list_add(gchar* name, gint table, gint color);

void player_list_remove(gchar* name);

void player_list_update(gchar* name, gint table, gint color);

void player_list_iterate(GFunc func);















