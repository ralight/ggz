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
static GList* deleted;

/* Local functions for manipulating server profile list */
static void server_list_print(void);
static void server_print(gpointer server, gpointer data);
static void server_free_node(gpointer server, gpointer data);
static gint server_match_name(gconstpointer, gconstpointer);


void server_profiles_load(void)
{
	guint i, count; 
	char** profiles;
	Server* server;
	
	/* Clear any previous list */
	if (servers) {
		g_list_foreach(servers, server_free_node, NULL); 	
		g_list_free(servers);
		servers = NULL;
	}

	/* Clear list of deleted servers */
	if (deleted) {
		g_list_foreach(deleted, server_free_node, NULL); 	
		g_list_free(deleted);
		deleted = NULL;
	}

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


void server_profiles_save(void)
{
	int count;
	const char** profiles;
	Server* server;
	GList* node;
	
	count = g_list_length(servers);
	profiles = server_get_names();
	
	ggzrc_write_list("Servers", "ProfileList", count, profiles);	
	g_free(profiles);

	for (node = servers; node != NULL; node = node->next) {
		server = (Server*)(node->data);
		dbg_msg("Profile %s about to be saved", server->name);
		
		ggzrc_write_string(server->name, "Host", server->host);
		ggzrc_write_int(server->name, "Port", server->port);
		ggzrc_write_int(server->name, "Type", server->type);
		ggzrc_write_string(server->name, "Login", server->login);
		if (server->type == GGZ_LOGIN)
			ggzrc_write_string(server->name, "Password", 
					   server->password);
	}

	for (node = deleted; node != NULL; node = node->next) {
		server = (Server*)(node->data);
		dbg_msg("Profile %s about to be deleted", server->name);
		ggzrc_remove_section(server->name);
	}
	
	/* Clear list of deleted servers */
	if (deleted) {
		g_list_foreach(deleted, server_free_node, NULL); 	
		g_list_free(deleted);
		deleted = NULL;
	}
}


void server_list_add(Server* server)
{
	dbg_msg("Adding profile %s to server list", server->name);
	servers = g_list_append(servers, (gpointer)server);
}


GList* server_get_name_list(void)
{
	GList* list = NULL;
	GList* current;
	
	/* Iterate through list, grabbing names */
	for (current = servers; current != NULL; current = current->next)
		list = g_list_append(list, ((Server*)(current->data))->name);
	
	return list;
}


const char** server_get_names(void)
{
	const char** profiles;
	int i = 0;
	GList* node;

	profiles = g_malloc0(sizeof(char*) * g_list_length(servers));
	node = servers;
	while (node) {
		profiles[i++] = ((Server*)(node->data))->name;
		node = node->next;
	}

	return profiles;
}


Server* server_get(gchar* name)
{
	GList* node;
	
	node = g_list_find_custom(servers, name, server_match_name);
	
	if (!node)
		return NULL;

	return (Server*)(node->data);
}


void server_list_remove(gchar* name)
{
	GList* node;
	Server* server;

	dbg_msg("Removing %s from server list", name);
	
	node = g_list_find_custom(servers, (gpointer)name, server_match_name);
	/* name not found */
	if (!node)
		return;

	server = (Server*)(node->data);
	servers = g_list_remove_link(servers, node);
	g_list_free_1(node);

	dbg_msg("Adding profile %s to deleted list", server->name);
	deleted = g_list_append(deleted, (gpointer)server);
	server_list_print();
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


static void server_free_node(gpointer server, gpointer data)
{
	if (((Server*)server)->name)
		g_free(((Server*)server)->name);

	if (((Server*)server)->host)
		g_free(((Server*)server)->host);

	if (((Server*)server)->login)
		g_free(((Server*)server)->login);

	if (((Server*)server)->password)
		g_free(((Server*)server)->password);

	g_free(server);
}


static gint server_match_name(gconstpointer server, gconstpointer name)
{
	return strcmp(((Server*)server)->name, name);
}
