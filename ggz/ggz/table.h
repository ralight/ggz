/*
 * File: table.h
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 6/5/00
 *
 * This fils contains functions for handling tables
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

#ifndef _GGZ_TABLE_H
#define _GGZ_TABLE_H

#include <glib.h>

#include <datatypes.h>

#define MAX_TABLE_SIZE 8
#define MAX_TABLE_DESC_LEN 512

/* 
 * The Table structure is meant to be a node in a linked list of
 * the tables in the current room.
 */
typedef struct Table {

	/* Unique identifier for this table */
	guint id;

	/* ID of room which this table is in */
	guint room;

	/* ID of game type being played on this table */
	guint type;

	/* State of table */
	guchar state;

	/* Seat assignments */
	gint seats[MAX_TABLE_SIZE];
	
	/* Names of players at table */
	gchar* names[MAX_TABLE_SIZE];
	
	/* Table description */
	gchar desc[MAX_TABLE_DESC_LEN];
} Table;


void table_list_clear(void);

void table_list_add(Table* table);

void table_list_remove(guint id);

void table_list_player_join(guint id, guint seat, gchar* name);

void table_list_player_leave(guint id, guint seat);

void table_list_iterate(GFunc func);

#endif
