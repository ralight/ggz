/*
 * File: memory.c
 * Author: Rich Gade
 * Project: GGZ Core Client Lib
 * Date: 02/15/01
 *
 * This is the code for handling memory allocation for ggzcore
 *
 * Copyright (C) 2001 Brent Hendricks.
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

#include "ggzcore.h"
#include "memory.h"


/* This is used basically as a LIFO stack, on the basis that most malloc/free
 * pairs are somewhat close together, so keep recent mallocs near the top */
typedef struct _memptr {
	struct _memptr	*next;
	char		*tag;
	int		line;
	void		*ptr;
	unsigned int	size;
} _memptr;

static struct _memptr *alloc = NULL;


static void * _ggzcore_allocate(const unsigned int size, char *tag, int line)
{
	struct _memptr *newmem;

	/* Try to allocate our memory */
	newmem = malloc(sizeof(_memptr) + size);
	if(newmem == NULL)
		ggzcore_error_sys_exit("Memory allocation failure: %s/%d",
					tag, line);

	/* We've got the memory, so set it up */
	newmem->next = alloc;
	newmem->tag = tag;
	newmem->line = line;
	newmem->ptr = newmem + 1;	/* Add one sizeof(_memptr) */
	newmem->size = size;

	/* Put this at the head of the list */
	alloc = newmem;

	ggzcore_debug(GGZ_DBG_MEMDETAIL,
		      "%d bytes allocated at %p from %s/%d\n",
		      size, newmem->ptr, tag, line);

	return newmem->ptr;
}


void * _ggzcore_malloc(const unsigned int size, char *tag, int line)
{
	void *new;

	/* Get a chunk of memory */
	new = _ggzcore_allocate(size, tag, line);

	/* Clear the memory to zilcho */
	memset(new, 0, size);

	return new;
}


void * _ggzcore_realloc(const void *ptr, const unsigned size,char *tag,int line)
{
	struct _memptr *prev, *targetmem;
	void *new;

	/* Search through allocated memory for this chunk */
	prev = NULL;
	targetmem = alloc;
	while(targetmem != NULL && ptr != targetmem->ptr) {
		prev = targetmem;
		targetmem = targetmem->next;
	}

	/* This memory was never allocated via ggzcore */
	if(targetmem == NULL) {
		ggzcore_error_msg("Memory reallocation <%p> failure: %s/%d",
				  ptr, tag, line);
		return NULL;
	}

	/* Try to allocate our memory */
	ggzcore_debug(GGZ_DBG_MEMDETAIL,
		      "Reallocating %d bytes at %p to %d bytes from %s/%d\n",
		      targetmem->size, targetmem->ptr, size, tag, line);
	new = _ggzcore_allocate(sizeof(_memptr) + size, tag, line);

	/* Copy the old to the new */
	memcpy(new, targetmem->ptr, targetmem->size);

	/* And zero out the rest of the block */
	memset(new+targetmem->size, 0, size-targetmem->size);

	/* And free the old chunk */
	_ggzcore_free(targetmem->ptr, tag, line);

	return new;
}


int _ggzcore_free(const void *ptr, char *tag, int line)
{
	struct _memptr *prev, *targetmem;

	/* Search through allocated memory for this chunk */
	prev = NULL;
	targetmem = alloc;
	while(targetmem != NULL && ptr != targetmem->ptr) {
		prev = targetmem;
		targetmem = targetmem->next;
	}

	/* This memory was never allocated via ggzcore */
	if(targetmem == NULL) {
		ggzcore_error_msg("Memory deallocation <%p> failure: %s/%d",
				  ptr, tag, line);
		return -1;
	}

	/* Juggle pointers so we can remove this item */
	if(prev == NULL)
		alloc = targetmem->next;
	else
		prev->next = targetmem->next;

	ggzcore_debug(GGZ_DBG_MEMDETAIL,
		      "%d bytes deallocated at %p from %s/%d\n",
		      targetmem->size, ptr, tag, line);

	free(targetmem);

	return 0;
}


int _ggzcore_memory_check(void)
{
	struct _memptr *memptr;

	ggzcore_debug(GGZ_DBG_MEMORY, "*** Memory Leak Check ***");
	if(alloc != NULL) {
		memptr = alloc;
		while(memptr != NULL) {
			ggzcore_debug(GGZ_DBG_MEMORY,
		      		"%d bytes left allocated at %p by %s/%d\n",
		      		memptr->size, memptr->ptr,
				memptr->tag, memptr->line);
			memptr = memptr->next;
		}
		ggzcore_debug(GGZ_DBG_MEMORY, "*** End Memory Leak Check ***");

		return -1;
	}

	ggzcore_debug(GGZ_DBG_MEMORY, "All clean!");
	ggzcore_debug(GGZ_DBG_MEMORY, "*** End Memory Leak Check ***");

	return 0;
}


char * _ggzcore_strdup(const char *src, char *tag, int line)
{
	unsigned len;
	char *new;

	if(src == NULL) {
		ggzcore_error_msg("_ggzcore_strdup - NULL pointer from %s/%d",
				  tag, line);
		return NULL;
	}

	len = strlen(src);

	ggzcore_debug(GGZ_DBG_MEMDETAIL,
		      "Allocating memory for length %d string from %s/%d\n",
		      len+1, tag, line);

	new = _ggzcore_allocate(len+1, tag, line);

	memcpy(new, src, len+1);

	return new;
}
