/*
 * File: stack.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 8/31/01
 *
 * This is the code for handling stacks
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

#include "lists.h"
#include "stack.h"


GGZStack* _ggzcore_stack_new(void)
{
	GGZStack *stack;

	stack = _ggzcore_list_create(NULL, NULL, NULL, 
				     _GGZCORE_LIST_ALLOW_DUPS);
	
	return stack;
}


void _ggzcore_stack_push(GGZStack *stack, void *item)
{
	_ggzcore_list_insert(stack, item);
}


void* _ggzcore_stack_pop(GGZStack *stack)
{
	_ggzcore_list_entry *entry;
	void *head;
	
	/* The stacks 'head' is actually the lists tail */
	entry = _ggzcore_list_tail(stack);

	if (entry) {
		head = _ggzcore_list_get_data(entry);
		_ggzcore_list_delete_entry(stack, entry);
	}
	else
		head = NULL;

	return head;
}


void* _ggzcore_stack_top(GGZStack *stack)
{
	_ggzcore_list_entry *entry;

	/* The stacks 'head' is actually the lists tail */
	entry = _ggzcore_list_tail(stack);

	if (entry)
		return _ggzcore_list_get_data(entry);
	else
		return NULL;
}


void _ggzcore_stack_free(GGZStack *stack)
{
	_ggzcore_list_destroy(stack);
}

