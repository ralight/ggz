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


GGZXMLElement* _ggzcore_xmlelement_new(char *tag, void (*process)(), void (*free)())
{
	GGZXMLElement *element;

	element = ggzcore_malloc(sizeof(GGZXMLElement));
	
	_ggzcore_xmlelement_init(element, tag, process, free);

	return element;
}


void _ggzcore_xmlelement_init(GGZXMLElement *element, char *tag, void (*process)(), void (*free)())
{
	if (element) {
		element->tag = ggzcore_strdup(tag);
		element->text = NULL;
		element->process = process;
		
		if (free)
			element->free = free;
		else
			element->free = _ggzcore_xmlelement_do_free;
	}
}


char* _ggzcore_xmlelement_get_tag(GGZXMLElement *element)
{
	return (element ? element->tag : NULL);
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
		if (element->free)
			element->free(element);
	}
}


static void _ggzcore_xmlelement_do_free(GGZXMLElement *element)
{
	if (element) 
		ggzcore_free(element);
}
