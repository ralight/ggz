/*
 * GGZ Metaserver access library (libggzmeta)
 * URI handling functions
 * Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>

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

#ifndef GGZ_URI_H
#define GGZ_URI_H

/* Data type describing a generic URI */
struct uri_t
{
	char *protocol;
	char *user;
	char *host;
	int port;
	char *path;
};
typedef struct uri_t URI;

/* Create a new URI object from a string describing an URI */
/* Object must be free'd with uri_free() after use */
URI uri_from_string(const char *uristring);

/* Convert an URI object back into a string */
/* The string must be free'd with free() after use */
char *uri_to_string(URI uri);

/* Free an URI object (or rather, its contents) */
void uri_free(URI uri);

#endif

