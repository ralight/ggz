/*
 * File: xmlelement.c
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

#include <config.h>

#include <stdlib.h>
#include <string.h>

#include "ggzcore.h"
#include "xmlelement.h"
#include "msg.h"
#include "memory.h"

static void _ggzcore_xmlelement_do_free(GGZXMLElement *element);


GGZXMLElement* _ggzcore_xmlelement_new(char *tag, char **attrs, void (*process)(), void (*free)())
{
	GGZXMLElement *element;

	element = ggzcore_malloc(sizeof(GGZXMLElement));
	
	_ggzcore_xmlelement_init(element, tag, attrs, process, free);

	return element;
}


void _ggzcore_xmlelement_init(GGZXMLElement *element, char *tag, char **attrs, void (*process)(), void (*free)())
{
	int i;

	if (element) {
		element->tag = ggzcore_strdup(tag);
		element->attributes = _ggzcore_list_create(NULL, 
							   _ggzcore_list_create_str,
							   _ggzcore_list_destroy_str,
							   _GGZCORE_LIST_ALLOW_DUPS);
		element->text = NULL;
		element->process = process;
		
		
		for (i = 0; attrs[i]; i++)
			_ggzcore_list_insert(element->attributes, attrs[i]);

		
		if (free)
			element->free = free;
		else
			element->free = _ggzcore_xmlelement_do_free;
	}
}


void _ggzcore_xmlelement_set_data(GGZXMLElement *element, void *data)
{
	if (element)
		element->data = data;
}


char* _ggzcore_xmlelement_get_tag(GGZXMLElement *element)
{
	return (element ? element->tag : NULL);
}


char* _ggzcore_xmlelement_get_attr(GGZXMLElement *element, char *attr)
{
	_ggzcore_list_entry *item;
	char *data;
	char *value = NULL;

	item = _ggzcore_list_head(element->attributes);
	while (item) {
		data = _ggzcore_list_get_data(item);
		if (strcmp(data, attr) == 0) {
			value = _ggzcore_list_get_data(_ggzcore_list_next(item));
			break;
		}
			
		/* Every other item is a value */
		item = _ggzcore_list_next(_ggzcore_list_next(item));
	}

	return value;
}


void* _ggzcore_xmlelement_get_data(GGZXMLElement *element)
{
	return (element ? element->data : NULL);
}


char* _ggzcore_xmlelement_get_text(GGZXMLElement *element)
{
	return (element ? element->text : NULL);
}


void _ggzcore_xmlelement_add_text(GGZXMLElement *element, const char *text, int len)
{
	int old_len = 0;
	int new_len = 0;

	if (element) {

		/* Allocate space for text if we haven't already */
		if (!element->text) {
			new_len = len + 1;
			element->text = ggzcore_malloc(new_len * sizeof(char));
			element->text[0] = '\0';
		}
		else {
			old_len = strlen(element->text);
			new_len = old_len + len + 1;
			element->text = ggzcore_realloc(element->text, new_len);
		}
		
		strncat(element->text, text, len);
		element->text[new_len - 1] = '\0';
	}
}


void _ggzcore_xmlelement_free(GGZXMLElement *element)
{
	if (element) {
		if (element->tag)
			ggzcore_free(element->tag);
		if (element->text)
			ggzcore_free(element->text);
		if (element->attributes)
			_ggzcore_list_destroy(element->attributes);
		if (element->free)
			element->free(element);
	}
}


static void _ggzcore_xmlelement_do_free(GGZXMLElement *element)
{
	if (element) 
		ggzcore_free(element);
}
