/*
 * File: lists.c
 * Author: Rich Gade
 * Project: GGZ Core Client Lib
 *          Modified for inclusion in server (rgade - 08/06/01)
 * Date: 9/19/00
 *
 * This is the code for handling list functions for ggzcore
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

#include "lists.h"


/* list_create()
 *	Creates a new list structure, and associates a comparison function
 *	(for sort/searches), create function (for user data structures),
 *	and destroy function with it.  Options can be found in lists.h
 *
 *	Returns a pointer to the list data structure
 */
list_t * list_create(listEntryCompare compare_func,
		     listEntryCreate create_func,
		     listEntryDestroy destroy_func, int options)
{
	list_t *new;

	if(compare_func == NULL && !(options & LIST_ALLOW_DUPS)) {
		/* debug warn - "Cannot replace dups w/o a compare func" */
	}

	new = malloc(sizeof(list_t));
	if(!new) {
		/* debug FATAL - "malloc() failure in _ggzcore_list_create" */
		return NULL;
	}
	new->head = NULL;
	new->tail = NULL;
	new->compare_func = compare_func;
	new->create_func = create_func;
	new->destroy_func = destroy_func;
	new->options = options;

	return new;
}


/* list_compare_str()
 *	A sample comparison function, this compares simple strings
 *
 *	Returns <0 if a<b,
 *		=0 if a=b,
 *		>0 if a>b
 */
int list_compare_str(void *a, void *b)
{
	if(!a || !b) {
		/* debug FATAL - "NULL passed to _ggzcore_list_compare_str" */
		return 1;
	}
	return strcmp(a, b);
}


/* list_create_str()
 *	A sample data create function, this makes a copy of a string
 *
 *	Returns a pointer to the allocated data entry
 */
void * list_create_str(void *data)
{
	void *new;

	if(!data) {
		/* debug FATAL - "NULL passed to _ggzcore_list_create_str" */
		return NULL;
	}

	new = malloc(strlen(data)+1);
	if(!new) {
		/* debug FATAL - "malloc() failure in _ggzcore_list_create_str" */
		return NULL;
	}
	strcpy(new, data);

	return new;
}


/* list_destroy_str()
 *	A sample data destroy function, this destroys strings created by
 *	_ggzcore_list_create_str()
 */
void list_destroy_str(void *data)
{
	if(!data) {
		/* debug FATAL - "NULL passed to _ggzcore_list_destroy_str" */
		return;
	}

	free(data);
}


/* list_insert()
 *	Inserts an entry into a list, copying the data entry if an entry
 *	creation function is known and placing the data in-order if a
 *	comparison function is registered.
 *
 *	Returns 0 normally
 *		or 1 if an entry was replaced
 *		or -1 on error
 */
int list_insert(list_t *list, void *data)
{
	list_entry_t *new, *p, *q;
	int compare=-1;
	int rc=0;

	if(!list) {
		/* debug warn - "NULL list passed to _ggzcore_list_insert" */
		return -1;
	}
	if(!data) {
		/* debug warn - "NULL data passed to _ggzcore_list_insert" */
		return -1;
	}

	/* Setup the new entry */
	new = malloc(sizeof(list_entry_t));
	if(!new) {
		/* debug FATAL - "malloc() failure in _ggzcore_list_insert" */
		return -1;
	}
	new->prev = NULL;
	new->next = NULL;
	if(list->create_func)
		new->data = (*list->create_func) (data);
	else
		new->data = data;

	/* Find where to stuff it */
	if(list->compare_func)
	{
		/* Search for the first slot that has higher-order data */
		p = list->head;
		q = NULL;
		while(p) {
			compare = (*list->compare_func) (p->data, data);
			if(compare >= 0)
				break;
			q = p;
			p = p->next;
		}
		/* p should point AFTER the insertion point, q before */
		if(compare == 0
		   && !(list->options & LIST_ALLOW_DUPS)) {
			/* The matching case, replace existing entry in place */
			if(q)
				q->next = new;
			else
				list->head = new;
			new->next = p->next;
			new->prev = q;
			if(p->next)
				p->next->prev = new;
			else
				list->tail = new;
			if(list->destroy_func)
				(*list->destroy_func) (p->data);
			free(p);
			rc = 1;
		} else {
			/* Slightly easier, just insert a new value */
			if(q)
				q->next = new;
			else
				list->head = new;
			new->next = p;
			new->prev = q;
			if(p)
				p->prev = new;
			else
				list->tail = new;
		}
	} else {
		/* Pretty easy if not doing in-order list */
		p = list->tail;
		if(p)
			p->next = new;
		new->next = NULL;
		new->prev = p;
		list->tail = new;
		if(!list->head)
			list->head = new;
	}

	return rc;
}


/* list_head/tail/next/prev()
 * list_get_data()
 *	These functions simply return a pointer from a list or entry
 *	They aren't strictly necessary, but should make the user code
 *	a bit easier to read and maintain.
 */
list_entry_t *list_head(list_t *list)
{
	if(!list) {
		/* debug warn - "NULL list passed to _ggzcore_list_head" */
		return NULL;
	}
	return list->head;
}

list_entry_t *list_tail(list_t *list)
{
	if(!list) {
		/* debug warn - "NULL list passed to _ggzcore_list_tail" */
		return NULL;
	}
	return list->tail;
}

list_entry_t *list_next(list_entry_t *entry)
{
	if(!entry) {
		/* debug warn - "NULL entry passed to _ggzcore_list_next" */
		return NULL;
	}
	return entry->next;
}

list_entry_t *list_prev(list_entry_t *entry)
{
	if(!entry) {
		/* debug warn - "NULL entry passed to _ggzcore_list_prev" */
		return NULL;
	}
	return entry->prev;
}

void * list_get_data(list_entry_t *entry)
{
	if(!entry) {
		/* debug warn - "NULL entry passed to _ggzcore_list_get_data" */
		return NULL;
	}
	return entry->data;
}


/* list_search()
 *	This function searches the list for a value using the
 *	user-supplied comparison function.  It will terminate early
 *	if it passes where the value should be in the list.
 *
 *	Returns pointer to list entry which contains value
 *		or NULL on failure
 */
list_entry_t * list_search(list_t *list, void *data)
{
	list_entry_t *p;
	int result;

	if(!list) {
		/* debug warn - "NULL list passed to _ggzcore_list_search" */
		return NULL;
	}
	if(!data) {
		/* debug warn - "NULL data passed to _ggzcore_list_search" */
		return NULL;
	}

	if(list->compare_func == NULL) {
		/* debug FATAL - "Cannot search list w/o a compare func" */
		return NULL;
	}

	p = list->head;
	while(p) {
		result = (*list->compare_func) (p->data, data);
		if(result == 0)
			return p;
		if(result > 0)
			return NULL;
		p = p->next;
	}

	return NULL;
}


/* list_search_alt()
 *	This function searches the list for a value using the search
 *	function specified by compare_func.  Note that the search cannot
 *	terminate early unless it finds the search target as we cannot
 *	assume that the list is in order for this comparison function.
 *
 *	Returns pointer to list entry which contains value
 *		or NULL on failure
 */
list_entry_t * list_search_alt(list_t *list, void *data,
			       listEntryCompare compare_func)
{
	list_entry_t *p;

	if(!list) {
		/* debug warn - "NULL list passed to _ggzcore_list_search_alt"*/
		return NULL;
	}
	if(!data) {
		/* debug warn - "NULL data passed to _ggzcore_list_search_alt"*/
		return NULL;
	}

	if(compare_func == NULL) {
		/* debug FATAL - "Cannot search list w/o a compare func" */
		return NULL;
	}

	p = list->head;
	while(p) {
		if((compare_func) (p->data, data) == 0)
			return p;
		p = p->next;
	}

	return NULL;
}



/* list_delete_entry()
 *	Removes an entry from a list, calling a destructor if registered
 */
void list_delete_entry(list_t *list, list_entry_t *entry)
{
	if(!list) {
		/* debug warn - "NULL list passed to _ggzcore_list_delete_entry" */
		return;
	}
	if(!entry) {
		/* debug warn - "NULL entry passed to _ggzcore_list_delete_entry" */
		return;
	}

	if(entry->prev) {
		entry->prev->next = entry->next;
		if(entry->next)
			entry->next->prev = entry->prev;
		else
			list->tail = entry->prev;
	} else {
		list->head = entry->next;
		if(entry->next)
			entry->next->prev = NULL;
		else
			list->tail = NULL;
	}

	if(list->destroy_func)
		(*list->destroy_func) (entry->data);

	free(entry);
}


/* list_destroy()
 *	Eliminates all list entries and deallocates the list structure
 */
void list_destroy(list_t *list)
{
	list_entry_t *p, *q;

	if(!list) {
		/* debug warn - "NULL list passed to _ggzcore_list_destroy" */
		return;
	}

	p = list->head;
	while(p) {
		q = p->next;
		if(list->destroy_func)
			(*list->destroy_func) (p->data);
		free(p);
		p = q;
	}

	free(list);
}
