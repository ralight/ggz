/*
 * File: hook.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 11/01/00
 *
 * This is the code for handling hook functions
 * 
 * Many of the ideas for this interface come from the hook mechanism in GLib
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

#ifndef __HOOK_H__
#define __HOOK_H__

#include <config.h>
#include <ggzcore.h>

/* _GGZHook : data type for representing single hook in a list */
struct _GGZHook {

	/* Callback ID (unique withing this event) */
	unsigned int id;
	
	/* Actual callback function */
	GGZCallback func;
	
	/* Pointer to user data */
	void* user_data;
	
	/* Function to free user data */
	GGZDestroyFunc destroy;

	/* Pointer to next GGZCallback */
	struct _GGZHook* next;
};


/* GGZHookList : list of hook functions */
typedef struct _GGZHookList {

	/* "Event" ID unique to this hooklist: gets passed to callbacks */
	unsigned int id;
	
	/* Sequence number for callbacks */
	int seq_id;

	/* Linked list of hooks */
	struct _GGZHook* hooks;

} GGZHookList;


GGZHookList* _ggzcore_hook_list_init(const unsigned int id);

int _ggzcore_hook_add(GGZHookList* list, const GGZCallback func);
int _ggzcore_hook_add_full(GGZHookList* list, const GGZCallback func,
			   void* data, const GGZDestroyFunc destroy);

int _ggzcore_hook_remove(GGZHookList *list, const GGZCallback func);
int _ggzcore_hook_remove_id(GGZHookList *list, const unsigned int id);
int _ggzcore_hook_remove_all(GGZHookList *list);

void _ggzcore_hook_list_invoke(GGZHookList *list, void* data);
			       
void _ggzcore_hook_list_destroy(GGZHookList *list);

#endif /* __HOOK_H__ */
