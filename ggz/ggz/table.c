/*
 * File: table.c
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 6/12/00
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


#include <glib.h>

#include <err_func.h>
#include <seats.h>
#include <table.h>


/* List of tables in current room */
static GList* tables;

/* Local functions for manipulating table list */
static void table_free_names(gpointer table, gpointer data);
static void table_list_print(void);
static void table_print(gpointer table, gpointer data);
static gint table_match_id(gconstpointer table, gconstpointer id);


void table_list_clear(void)
{
	g_list_foreach(tables, table_free_names, NULL); 
	g_list_free(tables);
	tables = NULL;
}


void table_list_add(Table* table)
{
	dbg_msg("Adding table %d to table list", table->id);
	
	tables = g_list_append(tables, (gpointer)table);
	table_list_print();
}


void table_list_remove(guint id)
{
	GList* node;

	dbg_msg("Removing table %d from table list", id);
	
	node = g_list_find_custom(tables, GINT_TO_POINTER(id), table_match_id);
	/* name not found */
	if (!node)
		return;

	tables = g_list_remove_link(tables, node);
	g_list_foreach(node, table_free_names, NULL); 
	g_list_free(node);
	table_list_print();
}


void table_list_player_join(guint id, guint seat, gchar* name)
{
	GList* node;
	Table* table;

	node = g_list_find_custom(tables, GINT_TO_POINTER(id), table_match_id);
	/* name not found */
	if (!node)
		return;

	table = (Table*)(node->data);
	table->seats[seat] = GGZ_SEAT_PLAYER; 
	table->names[seat] = g_strdup(name);
}


void table_list_player_leave(guint id, guint seat)
{
	GList* node;
	Table* table;

	node = g_list_find_custom(tables, GINT_TO_POINTER(id), table_match_id);
	/* name not found */
	if (!node)
		return;

	table = (Table*)(node->data);
	table->seats[seat] = GGZ_SEAT_OPEN; 
	if (table->names[seat]) {
		g_free(table->names[seat]);
		table->names[seat] = NULL;
	}
}


void table_list_iterate(GFunc func)
{
	g_list_foreach(tables, func, NULL);
}


static void table_free_names(gpointer table, gpointer data)
{
	guint i;

	for (i = 0; i < seats_num(*(Table*)table); i++) {
		if (((Table*)table)->names[i]) {
			g_free(((Table*)table)->names[i]);
			((Table*)table)->names[i] = NULL;
		}
	}
}


static void table_list_print(void)
{
	g_list_foreach(tables, table_print, NULL); 
}


static void table_print(gpointer table, gpointer data)
{
	dbg_msg("Table id: %d", ((Table*)table)->id);
	dbg_msg("Room num: %d", ((Table*)table)->room);
	dbg_msg("Table dsc: %s", ((Table*)table)->desc);
}


static gint table_match_id(gconstpointer table, gconstpointer id)
{
	if (((Table*)table)->id == (GPOINTER_TO_INT(id)))
		return 0;
	
	return 1;
}
