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

#include "err_func.h"
#include "xmlelement.h"


static void xmlelement_do_free(GGZXMLElement *element);


GGZXMLElement* xmlelement_new(char *tag, char **attrs, void (*process)(), void (*free)())
{
	GGZXMLElement *element;

	if ( (element = malloc(sizeof(GGZXMLElement))) == NULL)
		err_sys_exit("malloc error in xmlelement_new()");
	
	xmlelement_init(element, tag, attrs, process, free);

	return element;
}


void xmlelement_init(GGZXMLElement *element, char *tag, char **attrs, void (*process)(), void (*free)())
{
	int i;

	if (element) {
		element->tag = strdup(tag);
		element->attributes = list_create(NULL, 
							   list_create_str,
							   list_destroy_str,
							   LIST_ALLOW_DUPS);
		element->text = NULL;
		element->data = NULL;
		element->process = process;
		
		
		for (i = 0; attrs[i]; i++)
			list_insert(element->attributes, attrs[i]);

		
		if (free)
			element->free = free;
		else
			element->free = xmlelement_do_free;
	}
}


void xmlelement_set_data(GGZXMLElement *element, void *data)
{
	if (element)
		element->data = data;
}


char* xmlelement_get_tag(GGZXMLElement *element)
{
	return (element ? element->tag : NULL);
}


char* xmlelement_get_attr(GGZXMLElement *element, char *attr)
{
	list_entry_t *item;
	char *data;
	char *value = NULL;

	item = list_head(element->attributes);
	while (item) {
		data = list_get_data(item);
		if (strcmp(data, attr) == 0) {
			value = list_get_data(list_next(item));
			break;
		}
			
		/* Every other item is a value */
		item = list_next(list_next(item));
	}

	return value;
}


void* xmlelement_get_data(GGZXMLElement *element)
{
	return (element ? element->data : NULL);
}


char* xmlelement_get_text(GGZXMLElement *element)
{
	return (element ? element->text : NULL);
}


void xmlelement_add_text(GGZXMLElement *element, const char *text, int len)
{
	int old_len = 0;
	int new_len = 0;

	if (element) {

		/* Allocate space for text if we haven't already */
		if (!element->text) {
			new_len = len + 1;
			if ( (element->text = malloc(new_len * sizeof(char))) == NULL)
				err_sys_exit("malloc error in xmlelement_add_text()");
			element->text[0] = '\0';
		}
		else {
			old_len = strlen(element->text);
			new_len = old_len + len + 1;
			if ( (element->text = realloc(element->text, new_len)) == NULL)
				err_sys_exit("realloc error in xmlelement_add_text()");
		}
		
		strncat(element->text, text, len);
		element->text[new_len - 1] = '\0';
	}
}


void xmlelement_free(GGZXMLElement *element)
{
	if (element) {
		if (element->tag)
			free(element->tag);
		if (element->text)
			free(element->text);
		if (element->attributes)
			list_destroy(element->attributes);
		if (element->free)
			element->free(element);
	}
}


static void xmlelement_do_free(GGZXMLElement *element)
{
	if (element) 
		free(element);
}
