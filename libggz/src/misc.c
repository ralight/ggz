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


#include <config.h>

#include <stdio.h>
#include <string.h>

#include "ggz.h"


char * ggz_xml_unescape(char *str)
{
	char *new;
	char *p, *q;
	int len = 0;

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


char * ggz_xml_escape(char *str)
{
	char *new;
	char *p, *q;
	int len = 0;

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
