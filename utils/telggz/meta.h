/*
 * TelGGZ - The GGZ Gaming Zone Telnet Wrapper
 * Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net

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

#ifndef META_H
#define META_H

/* GGZ server information structure */
struct serverggz_t
{
	int id;
	char *host;
	int port;
	int preference;
	char *version;
	char *location;
	int speed;
};
typedef struct serverggz_t ServerGGZ;

/* Initialize the meta server configuration */
void meta_init();

/* Get a list of GGZ servers */
ServerGGZ **meta_query(const char *version);

/* Update internal list of meta servers */
void meta_sync();

/* Free a ServerGGZ list */
void meta_free(ServerGGZ **server);

#endif

