/*
 * File: table.c
 * Author: Justin Zaun
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
 *
 * This fils contains functions for handling tables
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#include "config.h"
#include "table.h"
#include "lists.h"
#include "msg.h"
#include "ggzcore.h"

#include <stdlib.h>
#include <string.h>

/* 
 * The Table structure is meant to be a node in a linked list of
 * the tables in the current room .
 */


/* Publicly exported functions */


int ggzcore_table_get_num(GGZTable *table)
{
	if (!table)
		return -1;

	return _ggzcore_table_get_num(table);
}


GGZGameType* ggzcore_table_get_type(GGZTable *table)
{
	if (!table)
		return NULL;

	return _ggzcore_table_get_type(table);
}


char ggzcore_table_get_state(GGZTable *table)
{
	if (!table)
		return -1;

	return _ggzcore_table_get_state(table);
}


int ggzcore_table_get_seats(GGZTable *table)
{
	if (!table)
		return -1;

	return _ggzcore_table_get_seats(table);
}

     
int ggzcore_table_get_open(GGZTable *table)
{
	if (!table)
		return -1;

	return _ggzcore_table_get_open(table);
}

     
int  ggzcore_table_get_bots(GGZTable *table)
{
	if (!table)
		return -1;

	return _ggzcore_table_get_bots(table);
}

     
char* ggzcore_table_get_desc(GGZTable *table)
{
	if (!table)
		return NULL;

	return _ggzcore_table_get_desc(table);
}


/* 
 * Internal library functions (prototypes in table.h) 
 * NOTE:All of these functions assume valid inputs!
 */

struct _GGZTable* _ggzcore_table_new(void)
{
	struct _GGZTable *table;

	table = ggzcore_malloc(sizeof(struct _GGZTable));

	/* FIXME: anything we should mark invalid? */
	return table;
}


void _ggzcore_table_init(struct _GGZTable *table, 
			 const int id,
			 struct _GGZGameType *gametype,
			 const char state,
			 const int seats,
			 const int open,
			 const int bots,
			 const char *desc)
{
	table->id = id;
	table->gametype = gametype;
	table->state = state;
	table->seats = seats;
	table->open = open;
	table->bots = bots;
	if (desc)
		table->desc = strdup(desc);

}


void _ggzcore_table_free(struct _GGZTable *table)
{
	if (table->desc)
		free(table->desc);
	
	ggzcore_free(table);
}


unsigned int _ggzcore_table_get_num(struct _GGZTable *table)
{
	return table->id;
}


struct _GGZGameType* _ggzcore_table_get_type(struct _GGZTable *table)
{
	return table->gametype;
}


char _ggzcore_table_get_state(struct _GGZTable *table)
{
	return table->state;
}


int _ggzcore_table_get_seats(struct _GGZTable *table)
{
	return table->seats;
}


int _ggzcore_table_get_open(struct _GGZTable *table)
{
	return table->open;
}


int _ggzcore_table_get_bots(struct _GGZTable *table)
{
	return table->bots;
}


char* _ggzcore_table_get_desc(struct _GGZTable *table)
{
	return table->desc;
}


int _ggzcore_table_compare(void* p, void* q)
{
	if(((struct _GGZTable*)p)->id == ((struct _GGZTable*)q)->id)
		return 0;
	if(((struct _GGZTable*)p)->id > ((struct _GGZTable*)q)->id)
		return 1;
	if(((struct _GGZTable*)p)->id < ((struct _GGZTable*)q)->id)
		return -1;

	return 0;
}


void* _ggzcore_table_create(void* p)
{
	struct _GGZTable *new, *src = p;

	new = _ggzcore_table_new();
	_ggzcore_table_init(new, src->id, src->gametype, src->state, 
			    src->seats, src->open, src->bots, src->desc);
	return (void*)new;
}


void  _ggzcore_table_destroy(void* p)
{
	_ggzcore_table_free(p);
}



/* Static functions internal to this file */

