/*
 * File: lists.h
 * Author: Rich Gade
 * Project: GGZ Core Client Lib
	    Modified for inclusion in server (rgade - 08/06/01)
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

#ifndef __LISTS_H__
#define __LISTS_H__

/****
 **** Option values for _ggzcore_list_create()
 ****/

#define LIST_REPLACE_DUPS	0x00
#define LIST_ALLOW_DUPS		0x01


/****
 **** Typedef's for user-defined functions
 ****/

typedef int	(*listEntryCompare)	(void *a, void *b);
typedef	void *	(*listEntryCreate)	(void *data);
typedef	void	(*listEntryDestroy)	(void *data);


/****
 **** Structure definitions for list and entries
 ****/

typedef struct list_entry_t {
	void				*data;
	struct list_entry_t		*next, *prev;
} list_entry_t;

typedef struct list_t {
	struct list_entry_t	*head, *tail;
	listEntryCompare	compare_func;
	listEntryCreate		create_func;
	listEntryDestroy	destroy_func;
	int			options;
} list_t;


/****
 **** Function prototypes
 ****/

list_t *
list_create		(listEntryCompare compare_func,
			 listEntryCreate create_func,
			 listEntryDestroy destroy_func,
			 int options);

int
list_compare_str	(void *a, void *b);

void *
list_create_str		(void *data);

void
list_destroy_str	(void *data);

int
list_insert		(list_t *list,
			 void *data);

list_entry_t *
list_head		(list_t *list);

list_entry_t *
list_tail		(list_t *list);

list_entry_t *
list_next		(list_entry_t *entry);

list_entry_t *
list_prev		(list_entry_t *entry);

void *
list_get_data		(list_entry_t *entry);

list_entry_t *
list_search		(list_t *list,
			 void *data);

list_entry_t *
list_search_alt		(list_t *list,
			 void *data,
			 listEntryCompare compare_func);

void
list_delete_entry	(list_t *list,
			 list_entry_t *entry);

void
list_destroy		(list_t *list);

#endif /* __LISTS_H__ */
