/*
 * File: lists.h
 * Author: Rich Gade
 * Project: GGZ Core Client Lib
 * Date: 9/19/00
 *
 * This is the code for handling list functions for ggzcore
 *
 * Copyright (C) 2000 Brent Hendricks.
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


/****
 **** Option values for _ggzcore_list_create()
 ****/

#define _GGZCORE_LIST_REPLACE_DUPS	0x00
#define _GGZCORE_LIST_ALLOW_DUPS	0x01


/****
 **** Typedef's for user-defined functions
 ****/

typedef int	(*_ggzcoreEntryCompare)	(void *a, void *b);
typedef	void *	(*_ggzcoreEntryCreate)	(void *data);
typedef	void	(*_ggzcoreEntryDestroy)	(void *data);


/****
 **** Structure definitions for list and entries
 ****/

typedef struct _ggzcore_list_entry {
	void				*data;
	struct _ggzcore_list_entry	*next, *prev;
} _ggzcore_list_entry;

typedef struct _ggzcore_list {
	struct _ggzcore_list_entry	*head, *tail;
	_ggzcoreEntryCompare		compare_func;
	_ggzcoreEntryCreate		create_func;
	_ggzcoreEntryDestroy		destroy_func;
	int				options;
} _ggzcore_list;


/****
 **** Function prototypes
 ****/

_ggzcore_list *
_ggzcore_list_create		(_ggzcoreEntryCompare compare_func,
				 _ggzcoreEntryCreate create_func,
				 _ggzcoreEntryDestroy destroy_func,
				 int options);

int
_ggzcore_list_compare_str	(void *a,
				 void *b);

void *
_ggzcore_list_create_str	(void *data);

void
_ggzcore_list_destroy_str	(void *data);

int
_ggzcore_list_insert		(_ggzcore_list *list,
				 void *data);

_ggzcore_list_entry *
_ggzcore_list_head		(_ggzcore_list *list);

_ggzcore_list_entry *
_ggzcore_list_tail		(_ggzcore_list *list);

_ggzcore_list_entry *
_ggzcore_list_next		(_ggzcore_list_entry *entry);

_ggzcore_list_entry *
_ggzcore_list_prev		(_ggzcore_list_entry *entry);

void *
_ggzcore_list_get_data		(_ggzcore_list_entry *entry);

_ggzcore_list_entry *
_ggzcore_list_search		(_ggzcore_list *list,
				 void *data);

void
_ggzcore_list_delete_entry	(_ggzcore_list *list,
				 _ggzcore_list_entry *entry);

void
_ggzcore_list_destroy		(_ggzcore_list *list);
