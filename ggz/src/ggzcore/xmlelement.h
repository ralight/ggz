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

struct _GGZXMLElement {
	
	/* The name of the tag */
	char *tag;

	/* Text content */
	char text[1024];
	
	/* Function to free allocated memory */
	void (*free)();

	/* Function to process tag */
	void (*process)();
};


typedef struct _GGZXMLElement GGZXMLElement;

GGZXMLElement* _ggzcore_xmlelement_new(char *tag, void (*process)(), void (*free)());
void _ggzcore_xmlelement_init(GGZXMLElement *element, char *tag, void (*process)(), void (*free)());

char* _ggzcore_xmlelement_get_tag(GGZXMLElement*);
/*void* _ggzcore_xmlelement_get_data(GGZXMLElement*);*/
char* _ggzcore_xmlelement_get_text(GGZXMLElement*);
void _ggzcore_xmlelement_add_text(GGZXMLElement*, const char *text, int len);

void _ggzcore_xmlelement_free(GGZXMLElement*);


#endif /* __XMLSTACK_H__ */
