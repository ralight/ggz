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

	/* Callback ID (unique within this event) */
	unsigned int id;
	
	/* Actual callback function */
	GGZHookFunc func;
	
	/* Pointer to user data */
	void* user_data;
	
	/* Pointer to next GGZHook */
	struct _GGZHook* next;
};


/* GGZHookList : list of hook functions */
typedef struct _GGZHookList {

	/* Object ID unique to this hooklist: gets passed to callbacks */
	unsigned int id;
	
	/* Sequence number for callbacks */
	int seq_id;

	/* Linked list of hooks */
	struct _GGZHook* hooks;

} GGZHookList;


/* _ggzcore_hook_list_init() - Allocate room for and initialize a hook
 *                             list structure 
 *
 * Receives:
 * unsigned int id       : ID code of event for this hook list
 *                       : to be passed to all hook functions
 * Returns:
 * GGZHookList* : pointer to new hooklist structure
 */
GGZHookList* _ggzcore_hook_list_init(const unsigned int id);


/* _ggzcore_hook_add()      - Add hook function to list
 * _ggzcore_hook_add_full() - Add hook function to list, specifying
 *                            all parameters 
 * 
 * Receives:
 * GGZHookList* list     : Hooklist to which we are adding function
 * GGZHookFunc func      : Callback function
 * void* user_data       : "User" data to pass to callback
 *
 * Returns:
 * int : id for this callback 
 */
int _ggzcore_hook_add(GGZHookList* list, const GGZHookFunc func);
int _ggzcore_hook_add_full(GGZHookList* list, const GGZHookFunc func, void* data);
			   
			   
/* _ggzcore_hook_remove()     - Remove specific hook from a list
 * _ggzcore_hook_remove_id()  - Remove specific hook from a list
 * _ggzcore_hook_remove_all() - Remove all hooks from a list
 *
 * Receives:
 * GGZHookList* list : Hooklist from which to remove hooks
 * unsigned int id   : ID of hook to remove
 * GGZCallback func  : pointer to hook function to remove
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int _ggzcore_hook_remove(GGZHookList *list, const GGZHookFunc func);
int _ggzcore_hook_remove_id(GGZHookList *list, const unsigned int id);
void _ggzcore_hook_remove_all(GGZHookList *list);


/* _ggzcore_hook_list_invoke() - Invoke list of hook functions
 *
 * Receives:
 * GGZHookList *list : list of hooks to invoke
 * void *data        : commin data passed to all hook functions in list
 *
 * Returns:
 * GGZ_HOOK_OK     if all hooks execute properly (even if some are removed)
 * GGZ_HOOK_ERROR  if at least one hook returned an error message
 * GGZ_HOOK_CRISIS if a hook terminated the sequence with a crisis
 */
GGZHookReturn _ggzcore_hook_list_invoke(GGZHookList *list, void* data);

			       
/* _ggzcore_hook_list_destroy() - Deallocate and destroy hook list
 *
 * Receives:
 * GGZHookList *list : hooklist to destroy
 */
void _ggzcore_hook_list_destroy(GGZHookList *list);


/* _ggzcore_hook_list_dump() - Dump list of hooks to screen for debugging
 *
 * Receives:
 * GGZHookList *list : hooklist to destroy
 */
void _ggzcore_hook_list_dump(GGZHookList *list);

#endif /* __HOOK_H__ */
