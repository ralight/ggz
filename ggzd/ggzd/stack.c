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


GGZStack* stack_new(void)
{
	GGZStack *stack;

	stack = list_create(NULL, NULL, NULL, 
				     LIST_ALLOW_DUPS);
	
	return stack;
}


void stack_push(GGZStack *stack, void *item)
{
	list_insert(stack, item);
}


void* stack_pop(GGZStack *stack)
{
	list_entry_t *entry;
	void *head;
	
	/* The stacks 'head' is actually the lists tail */
	entry = list_tail(stack);

	if (entry) {
		head = list_get_data(entry);
		list_delete_entry(stack, entry);
	}
	else
		head = NULL;

	return head;
}


void* stack_top(GGZStack *stack)
{
	list_entry_t *entry;

	/* The stacks 'head' is actually the lists tail */
	entry = list_tail(stack);

	if (entry)
		return list_get_data(entry);
	else
		return NULL;
}


void stack_free(GGZStack *stack)
{
	list_destroy(stack);
}

