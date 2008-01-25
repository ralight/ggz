/*
 * GGZ Metaserver access library (libggzmeta)
 * Client-side access to the metaserver
 * Copyright (C) 2001 - 2006 Josef Spillner <josef@ggzgamingzone.org>

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef GGZ_META_H
#define GGZ_META_H

/* Header files - URI is now in libggz */
#include <ggz.h>

/* Data type describing a server property */
/* In queries, those are used as restrictions for refined search */
struct serverattribute_t
{
	char *key;
	char *value;
};
typedef struct serverattribute_t ServerAttribute;

/* Data type describing a server */
/* This can be a GGZ metaserver, a GGZ server or any other server */
/* The set of attributes depends on the server type */
struct serverentry_t
{
	ggz_uri_t uri;
	ServerAttribute *attributes;
};
typedef struct serverentry_t ServerEntry;

/* === Basic metaserver objects and lists thereof === */
/* ================================================== */

/* Create a new server object */
/* Ownership of URI remains with the caller */
ServerEntry *meta_server_new(ggz_uri_t uri);

/* Addition of attribute to a server object */
void meta_server_attribute(ServerEntry *server,
	const char *key, const char *value);

/* Return a specific attribute */
const char *meta_server_findattribute(ServerEntry *server, const char *key);

/* Free'ing all memory associated with a server, including attributes */
void meta_server_free(ServerEntry *server);

/* Construct a server list */
/* To begin, list must be NULL */
/* After each call the enlarged list is returned */
/* The server should then not be freed, it becomes part of the list */
/* List however is growing and must be free'd with meta_list_free() */
ServerEntry **meta_list_server(ServerEntry **list, ServerEntry *server);

/* Free a server list */
void meta_list_free(ServerEntry **server);

/* === Publication functions === */
/* ============================= */

/* Add information about a server to one metaserver, given by URL */
int meta_add(ServerEntry *server, const char *classname, const char *category,
	const char *metaserveruri, const char *username, const char *password);

/* === Query functions, both generic and GGZ-specific === */
/* ====================================================== */

/* Get a list of servers from a single metaserver */
/* Must be free'd with meta_list_free() after use */
ServerEntry **meta_query(const char *metaserveruri, const char *classname,
	const char *category, ServerEntry *restriction);

/* Get a list of servers from a set of metaservers */
/* Must be free'd with meta_list_free() after use */
ServerEntry **meta_queryall(ServerEntry **servers, const char *classname,
	const char *category, ServerEntry *restriction);

/* Get a list of all GGZ servers from a set of metaservers */
/* Must be free'd with meta_list_free() after use */
/* This is really just a shortcut function for meta_queryall() */
ServerEntry **meta_queryallggz(ServerEntry **servers, const char *protocol);

/* === GGZ metaserver network synchronisation === */
/* ============================================== */

/* Initialize the meta server configuration */
/* Must be free'd with meta_list_free() after use */
ServerEntry **meta_network_load(void);

/* Update internal list of meta servers */
/* Note that the metaserver list will be free'd */
/* A replacement list is returned instead, containing more entries */
ServerEntry **meta_network_sync(ServerEntry **servers);

/* Update internal list of meta servers */
void meta_network_store(ServerEntry **servers);

#endif

