/*
 * File: misc.c
 * Author: Rich Gade
 * Project: GGZ Core Client Lib
 * Date: 11/06/01
 *
 * Miscellaneous convenience functions
 *
 * Copyright (C) 2001 Brent Hendricks.
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
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ggz.h"

#include "misc.h"
#include "support.h"

static char * _fill_buffer(char *low_byte, GGZFile *file);

char * ggz_xml_unescape(const char *str)
{
	char *new, *q;
	const char *p;
	size_t len = 0;

	if(str == NULL)
		return NULL;

	for(p=str; *p != '\0'; p++) {
		if(*p == '&') {
			if(!strncmp(p+1, "apos;", 5))
				p += 5;
			else if(!strncmp(p+1, "quot;", 5))
				p += 5;
			else if(!strncmp(p+1, "amp;", 4))
				p += 4;
			else if(!strncmp(p+1, "lt;", 3))
				p += 3;
			else if(!strncmp(p+1, "gt;", 3))
				p += 3;
		}
		len++;
	}

	if(len == strlen(str))
		return ggz_strdup(str);

	q = new = ggz_malloc(len+1);
	for(p=str; *p != '\0'; p++) {
		if(*p == '&') {
			if(!strncmp(p+1, "apos;", 5)) {
				*q = '\'';
				p += 5;
			} else if(!strncmp(p+1, "quot;", 5)) {
				*q = '"';
				p += 5;
			} else if(!strncmp(p+1, "amp;", 4)) {
				*q = '&';
				p += 4;
			} else if(!strncmp(p+1, "lt;", 3)) {
				*q = '<';
				p += 3;
			} else if(!strncmp(p+1, "gt;", 3)) {
				*q = '>';
				p += 3;
			}
		} else
			*q = *p;
		q++;
	}


	*q = '\0';

	return new;
}


char * ggz_xml_escape(const char *str)
{
	char *new, *q;
	const char *p;
	size_t len = 0;

	if(str == NULL)
		return NULL;

	for(p=str; *p != '\0'; p++)
		switch((int)*p) {
			case '\'':
			case '"':
				len += 6;
				break;
			case '&':
				len += 5;
				break;
			case '>':
			case '<':
				len += 4;
				break;
			default:
				len++;
		}

	if(len == strlen(str))
		return ggz_strdup(str);

	q = new = ggz_malloc(len+1);
	for(p=str; *p != '\0'; p++)
		switch((int)*p) {
			case '\'':
				memcpy(q, "&apos;", 6);
				q += 6;
				break;
			case '"':
				memcpy(q, "&quot;", 6);
				q += 6;
				break;
			case '&':
				memcpy(q, "&amp;", 5);
				q += 5;
				break;
			case '>':
				memcpy(q, "&gt;", 4);
				q += 4;
				break;
			case '<':
				memcpy(q, "&lt;", 4);
				q += 4;
				break;
			default:
				*q = *p;
				q++;
		}
	*q = '\0';

	return new;
}


#define START_BUF_SIZE 1024
#define INC_BUF_SIZE   512

GGZFile * ggz_get_file_struct(int fdes)
{
	GGZFile *new;
	int bytes_read;

	/* Setup a new GGZFile struct */
	new = ggz_malloc(sizeof(GGZFile));
	new->fdes = fdes;

	/* Allocate and prefill the data buffer */
	new->bufsize = START_BUF_SIZE;
	new->p = new->buf = ggz_malloc(new->bufsize);
	bytes_read = read(fdes, new->p, new->bufsize);
	new->e = new->p + bytes_read;

	return new;
}


char * ggz_read_line(GGZFile *file)
{
	char *data_line;
	/* int d_off; */

	data_line = file->p;
	while(1) {
		if(file->p == file->e) {
			data_line = _fill_buffer(data_line, file);
			/* EOF Check */
			if(file->p == file->e) {
				/* Must guarantee NULL term */
				/* We KNOW that p is a valid mem location */
				/* because we just tried to expand the buf */
				*file->p = '\0';
				break;
			}
		}
		if(*file->p == '\n') {
			*file->p = '\0';
			file->p++;
			break;
		}
		file->p++;
	}

	if(data_line == file->p)
		return NULL;
	else
		return ggz_strdup(data_line);
}


static char * _fill_buffer(char *low_byte, GGZFile *file)
{
	int off, len;

	if(low_byte == file->buf) {
		/* Buffer expansion required to fit data line */
		off = file->p - file->buf;	/* Save p/e offsets */

		file->bufsize += INC_BUF_SIZE;	/* Make a bigger buffer */
		file->buf = ggz_realloc(file->buf, file->bufsize);

		file->p = file->e = file->buf + off; /* Setup new p/e offsets */
	} else {
		/* Relocate bytes to low end of buffer to make space */
		if((len = file->e - low_byte) > 0)
			memmove(file->buf, low_byte, len);
		file->p = file->buf + len;
		file->e = file->p;
	}
	low_byte = file->buf;		/* low is now at start */

	/* Now that we've made space, fill the buffer */
	len = file->bufsize - (file->e - low_byte);
	if((len = read(file->fdes, file->e, len)) >= 0)
		file->e += len;

	return low_byte;
}


void ggz_free_file_struct(GGZFile *file)
{
	ggz_free(file->buf);
	ggz_free(file);
}


static int safe_string_compare(const char *s1, const char *s2,
			       int (*cmp_func)(const char *a, const char *b))
{
	
	if (s1 == NULL) {
		if (s2 == NULL) /* If they're both NULL, consider them equal */
			return 0;
		else
			return -1;
	}

	if (s2 == NULL)
		return 1;
	
	/* If both strings are non-NULL, do normal string compare */
	return cmp_func(s1, s2);
}

int ggz_strcmp(const char *a, const char *b)
{
	return safe_string_compare(a, b, strcmp);
}

int ggz_strcasecmp(const char *a, const char *b)
{
	return safe_string_compare(a, b, strcasecmp);
}
