/*
 * File: xmlelement.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 8/31/01
 *
 * This is the code for handling the an XML element
 *
 * Copyright (C) 2000,2001 Brent Hendricks.
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

#ifndef __XMLELEMENT_H__
#define __XMLELEMENT_H__

#include "lists.h"

struct _GGZXMLElement {
	
	/* The name of the tag */
	char *tag;

	/* Text content */
	char *text;

	/* Tag attributes */
	list_t *attributes;
	
	/* Extra data associated with tag (usually gleaned from children) */
	void *data;

	/* Function to free allocated memory */
	void (*free)();

	/* Function to process tag */
	void (*process)();
};


typedef struct _GGZXMLElement GGZXMLElement;

GGZXMLElement* xmlelement_new(char *tag, char **attrs, void (*process)(), void (*free)());
void xmlelement_init(GGZXMLElement *element, char *tag, char **attrs, void (*process)(), void (*free)());

void xmlelement_set_data(GGZXMLElement*, void *data);

char* xmlelement_get_tag(GGZXMLElement*);
char* xmlelement_get_attr(GGZXMLElement *element, char *attr);
void* xmlelement_get_data(GGZXMLElement*);
char* xmlelement_get_text(GGZXMLElement*);
void xmlelement_add_text(GGZXMLElement*, const char *text, int len);

void xmlelement_free(GGZXMLElement*);


#endif /* __XMLSTACK_H__ */
