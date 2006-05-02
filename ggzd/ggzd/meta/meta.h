/*
 * TelGGZ - The GGZ Gaming Zone Telnet Wrapper
 * Copyright (C) 2001 - 2003 Josef Spillner, dr_maux@users.sourceforge.net

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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef TELGGZ_META_H
#define TELGGZ_META_H

#include "uri.h"

struct serverattribute_t
{
	char *key;
	char *value;
};
typedef struct serverattribute_t ServerAttribute;

/* GGZ server information structure */
struct serverentry_t
{
	/*int id;*/
	URI uri;
	ServerAttribute *attributes;
	/*int preference;
	char *version;
	char *location;
	int speed;*/
};
typedef struct serverentry_t ServerEntry;

ServerEntry *meta_server_new(URI uri);
void meta_server_attribute(ServerEntry *server,
	const char *key, const char *value);
void meta_server_free(ServerEntry *server);

/* Initialize the meta server configuration */
//void meta_init(void);

/* Get a list of GGZ servers */
//ServerGGZ **meta_query(const char *version);

/* Update internal list of meta servers */
//void meta_sync(void);

/* Free a ServerGGZ list */
//void meta_free(ServerGGZ **server);

/* Add information about a server */
int meta_add(ServerEntry *server, const char *classname, const char *category,
	const char *metaserveruri, const char *username, const char *password);

#endif

