/*
 * File: hook.c
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

#include <config.h>
#include <ggzcore.h>
#include <hook.h>
#include <state.h>
#include <net.h>

#include <errno.h>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>


/* _ggzcore_hook_list_init() - Allocate room for and initialize a hook
 *                             list structure 
 *
 * Receives:
 * unsigned int id       : ID code of event for this hook list
 *                       : to be passed to all hook functions
 * Returns:
 * GGZHookList* : pointer to new hooklist structure
 */
GGZHookList* _ggzcore_hook_list_init(const unsigned int id)
{
	GGZHookList *hooks;

	if ( (hooks = calloc(1, sizeof(GGZHookList))) == NULL)
		ggzcore_error_sys_exit("calloc() failed in _ggzcore_hook_list_init");
	
	hooks->id = id;

	return hooks;
}
	

/* _ggzcore_hook_add() - Add hook function to list
 *
 * Receives:
 * GGZHookList* list : Hooklist to which we are adding function
 * GGZCallback func  : Callback function
 *
 * Returns:
 * int : id for this callback 
 */
int _ggzcore_hook_add(GGZHookList* list, const GGZCallback func)
{
	return _ggzcore_hook_add_full(list, func, NULL, NULL);
}


/* _ggzcore_hook_add_full() - Add hook function to list, specifying
 *                            all parameters 
 * 
 * Receives:
 * GGZHookList* list     : Hooklist to which we are adding function
 * GGZCallback func      : Callback function
 * void* user_data       : "User" data to pass to callback
 * GGZDestroyFunc destroy: function to call to free user data
 *
 * Returns:
 * int : id for this callback 
 */
int _ggzcore_hook_add_full(GGZHookList* list, const GGZCallback func,
			   void* user_data, GGZDestroyFunc destroy)
{
	struct _GGZHook *hook, *cur, *next;
	
	if ( (hook = calloc(1, sizeof(struct _GGZHook))) == NULL)
		ggzcore_error_sys_exit("calloc() failed in ggzcore_hook_add_full");
	
	/* Assign unique ID */
	hook->id = list->seq_id++;
	hook->func = func;
	hook->user_data = user_data;
	hook->destroy = destroy;

	/* Append onto list of callbacks */
	if ( (next = list->hooks) == NULL)
		list->hooks = hook;
	else {
		while (next) {
			cur = next;
			next = cur->next;
		}
		cur->next = hook;
	}
	
	return hook->id;
}


/* _ggzcore_hook_remove_all() - Remove all hooks from a list
 *
 * Receives:
 * GGZHookList* list : Hooklist from which to remove hooks
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int _ggzcore_hook_remove_all(GGZHookList *list)
{
	struct _GGZHook *cur, *next;

	next = list->hooks;
	while (next) {
		cur = next;
		next = cur->next;
		/* FIXME: need to worry about freeing user_data */
		free(cur);
	}
	list->hooks = NULL;
	
	return 0;
}


/* _ggzcore_hook_remove_id() - Remove specific hook from a list
 *
 * Receives:
 * GGZHookList* list : Hooklist from which to remove hooks
 * unsigned int id   : ID of hook to remove
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int _ggzcore_hook_remove_id(GGZHookList *list, const unsigned int id)
{
	struct _GGZHook *cur, *prev = NULL;

	cur = list->hooks;
	while (cur && cur->id != id) {
		prev = cur;
		cur = cur->next;
	}
	
	if (cur) {
		/* Special case if it was first in the list */
		if (!prev)
			list->hooks = cur->next;
		else
			prev->next = cur->next;

		/* FIXME: Worry about freeing user_data */
		free(cur);
		return 0;
	}
	/* Couldn't find it! */
	else 
		return -1;
}


/* _ggzcore_hook_remove() - Remove specific hook from a list
 *
 * Receives:
 * GGZHookList* list : Hooklist from which to remove hoook
 * GGZCallback func  : pointer to hook function 
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int _ggzcore_hook_remove(GGZHookList *list, const GGZCallback func)
{
	struct _GGZHook *cur, *prev = NULL;

	cur = list->hooks;
	while (cur && cur->func != func) {
		prev = cur;
		cur = cur->next;
	}

	if (cur) {
		/* Special case if it was first in the list */
		if (!prev)
			list->hooks = cur->next;
		else
			prev->next = cur->next;

		/* FIXME: Worry about freeing user_data */
		free(cur);
		return 0;
	}
	/* Couldn't find it! */		
	else 
		return -1;
}


/* _ggzcore_hook_list_invoke() - Invoke list of hook functions
 *
 * Receives:
 * GGZHookList *list : list of hooks to invoke
 * void *data        : commin data passed to all hook functions in list
 */
void _ggzcore_hook_list_invoke(GGZHookList *list, void *data)
{
	struct _GGZHook *cur;
	struct _GGZHook copy; 
	
	for (cur = list->hooks; cur != NULL; cur = copy.next) {
		/* We make a copy in case the function removes the
		   hook out from under us */
		copy = *cur;
		(*copy.func)(list->id, data, copy.user_data);
		
		/* Free callback specific data */
		if (copy.user_data && copy.destroy)
			(*copy.destroy)(copy.user_data);
	}
}


/* _ggzcore_hook_list_destroy() - Deallocate and destroy hook list
 *
 * Receives:
 * GGZHookList *list : hooklist to destroy
 */
void _ggzcore_hook_list_destroy(GGZHookList *list)
{
	_ggzcore_hook_remove_all(list);
	free(list);
}


