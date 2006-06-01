/*
 * GGZ Metaserver access library (libggzmeta)
 * Leightweight XML parser functions
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

#ifndef GGZ_MINIDOM_H
#define GGZ_MINIDOM_H

#ifdef __cplusplus
extern "C" {
#endif

struct att_t
{
	char *name;				/* name of the attribute */
	char *value;			/* value of the attribute */
};

struct ele_t
{
	struct ele_t *parent;	/* pointer to the parent */
	struct ele_t **el;		/* list of child elements */
	struct att_t **at;		/* list of attributes */
	char *name;				/* tag identifier */
	char *value;			/* value inside the tag*/
	int elnum;				/* number of child elements [redundant] */
	int atnum;				/* number of attribute pairs [redundant] */
};

struct dom_t
{
	struct ele_t *el;		/* the root node (may be NULL) */
	int valid;				/* validity of the DOM */
	int processed;			/* indicates whether you can work with it */
};

typedef struct dom_t DOM;	/* Domain Object Model */
typedef struct ele_t ELE;	/* Elements */
typedef struct att_t ATT;	/* Attributes */

DOM *minidom_load(const char *file);
DOM *minidom_parse(const char *stream);
void minidom_dump(DOM *dom);
void minidom_dumpfile(DOM *dom, const char *file);
void minidom_free(DOM *dom);

ELE *MD_query(ELE *parent, const char *name);
ELE **MD_querylist(ELE *parent, const char *name);
void MD_freelist(ELE **elelist);

char *MD_att(ELE *element, const char *name);

#ifdef __cplusplus
}
#endif

#endif

