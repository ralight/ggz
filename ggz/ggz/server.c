/*
 * File: server.c
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 6/19/00
 *
 * This file contains functions for handling server client profiles
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
#include <stdio.h>

#include <datatypes.h>
#include <err_func.h>
#include <ggzrc.h>
#include <server.h>

/* List of server profiles */
static GList* servers;

/* Local functions for manipulating server profile list */
static void server_list_print(void);
static void server_print(gpointer server, gpointer data);
static gint server_match_name(gconstpointer, gconstpointer);

void server_profiles_load(void)
{
	guint i, count; 
	char** profiles;
	Server* server;

	ggzrc_read_list("Servers", "ProfileList", &count, &profiles);

	dbg_msg("%d server profiles found:", count);
	for (i = 0; i < count; i++) {
		server = g_malloc0(sizeof(Server));
		server->name = profiles[i];
		server->host = ggzrc_read_string(server->name, "Host", NULL);
		server->port = ggzrc_read_int(server->name, "Port", 5688);
		server->type = ggzrc_read_int(server->name, "Type", 
					      GGZ_LOGIN_GUEST);
		server->login = ggzrc_read_string(server->name, "Login", NULL);
		if (server->type == GGZ_LOGIN)
			server->password = ggzrc_read_string(server->name, 
							     "Password", NULL);
		server_list_add(server);
	}
	server_list_print();
}


void server_list_add(Server* server)
{
	dbg_msg("Adding profile %s to server list", server->name);
	servers = g_list_append(servers, (gpointer)server);
}


GList* server_get_names(void)
{
	GList* list = NULL;
	GList* current;
	
	/* Iterate through list, grabbing names */
	for (current = servers; current != NULL; current = current->next)
		list = g_list_append(list, ((Server*)(current->data))->name);
	
	return list;
}


Server* server_get(gchar* name)
{
	GList* node;
	
	node = g_list_find_custom(servers, name, server_match_name);
	
	if (!node)
		return NULL;

	return (Server*)(node->data);
}


static void server_list_print(void)
{
	g_list_foreach(servers, server_print, NULL); 
}


static void server_print(gpointer server, gpointer data)
{
	dbg_msg("Profile name: %s", ((Server*)server)->name);
	dbg_msg("  Host %s:%d", ((Server*)server)->host, 
		((Server*)server)->port);
	dbg_msg("  Login type: %d", ((Server*)server)->type);
	dbg_msg("  Login: %s", ((Server*)server)->login);
	if (((Server*)server)->type == GGZ_LOGIN)
		dbg_msg("  Password: %s", ((Server*)server)->password);
}


static gint server_match_name(gconstpointer server, gconstpointer name)
{
	return strcmp(((Server*)server)->name, name);
}
