/*
 * File: player.c
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

#include <err_func.h>
#include <player.h>
#include <chat.h>


/* List  of players in current room */
static GList* players;

/* Local functions for manipulating player list */
static void player_free_name(gpointer, gpointer);
static void player_list_print(void);
static void player_print(gpointer, gpointer);
static gint player_match_name(gconstpointer, gconstpointer);


void player_list_clear(void)
{
	g_list_foreach(players, player_free_name, NULL); 
	g_list_free(players);
	players = NULL;
}


void player_list_add(gchar* name, gint table, gint color)
{
	Player* player;

	chat_print(CHAT_COLOR_SERVER, "-->", name);

	dbg_msg("Adding %s to player list", name);
	
	player = g_malloc(sizeof(Player));
	player->name = g_strdup(name);
	player->table = table;
	player->chat_color = color;

	players = g_list_append(players, (gpointer)player);
	player_list_print();
}


void player_list_remove(gchar* name)
{
	GList* node;

	chat_print(CHAT_COLOR_SERVER, "<--", name);

	dbg_msg("Removing %s from player list", name);
	
	node = g_list_find_custom(players, (gpointer)name, player_match_name);
	/* name not found */
	if (!node)
		return;

	players = g_list_remove_link(players, node);
	g_list_foreach(node, player_free_name, NULL); 
	g_list_free(node);
	player_list_print();
}


void player_list_update(gchar* name, gint table, gint color)
{
	GList* node;
	Player* player;

	dbg_msg("Updating player list info for %s", name);

	node = g_list_find_custom(players, (gpointer)name, player_match_name);
	/* name not found */
	if (!node)
		return;

	/* Update information */
	player = (Player*)node->data;
	player->table = table;
	player->chat_color = color;

	player_list_print();
}


void player_list_iterate(GFunc func)
{
	g_list_foreach(players, func, NULL);
}


static void player_free_name(gpointer player, gpointer data)
{
	if (((Player*)player)->name)
		g_free(((Player*)player)->name);
}


static void player_list_print(void)
{
	g_list_foreach(players, player_print, NULL); 
}


static void player_print(gpointer player, gpointer data)
{
	dbg_msg("Player name: %s", ((Player*)player)->name);
	dbg_msg("Player table: %d", ((Player*)player)->table);
	dbg_msg("Player color: %d", ((Player*)player)->chat_color);
}


static gint player_match_name(gconstpointer player, gconstpointer name)
{
	return strcmp(((Player*)player)->name, name);
}

