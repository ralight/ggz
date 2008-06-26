/*
 * Helper functions for ggzd database modules
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

#ifdef HAVE_CONFIG_H
# include <config.h>		/* Site specific config */
#endif

#include <ggz.h>
#include <string.h>

#include "ggzdb_util.h"

/* Helper function, might go into libggz*/
char *_ggz_sql_escape(const char *str)
{
	char *newstr, *q;
	const char *p;
	size_t len = 0;

	if(str == NULL)
		return NULL;

	len = strlen(str);

	for(p = str; *p != '\0'; p++) {
		if(*p == '\'') {
			len += 1;
		}
	}

	if(len == strlen(str))
		return ggz_strdup(str);

	newstr = ggz_malloc(len + 1);
	q = newstr;

	for(p = str; *p != '\0'; p++) {
		if(*p == '\'') {
			*q++ = '\'';
			*q = *p;
		} else {
			*q = *p;
		}
		q++;
	}
	*q = '\0';

	return newstr;
}

/* Helper function, might go into libggz*/
/*char *_ggz_sql_unescape(const char *str)
{
	char *new, *q;
	const char *p;
	size_t len = 0;

	if(str == NULL)
		return NULL;

	len = strlen(str);

	for(p = str; *p != '\0'; p++) {
		if(!strncmp(p, "\'\'", 2)) {
			p += 1;
		}
		len++;
	}

	if(len == strlen(str))
		return ggz_strdup(str);

	q = new = ggz_malloc(len + 1);
	for(p = str; *p != '\0'; p++) {
		if(!strncmp(p, "\'\'", 2)) {
			*q = '\'';
			q++;
			p += 1;
		} else {
			*q = *p;
			q++;
		}
	}
	*q = '\0';

	return new;
}*/

