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

#include "ggz.h"


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


static void * _ggz_allocate(const unsigned int size, char *tag, int line)
{
	struct _memptr *newmem;

	/* Try to allocate our memory */
	newmem = malloc(sizeof(_memptr) + size);
	if(newmem == NULL)
		ggz_error_sys_exit("Memory allocation failure: %s/%d", tag, 
				    line);
				    

	/* We've got the memory, so set it up */
	newmem->next = alloc;
	newmem->tag = tag;
	newmem->line = line;
	newmem->ptr = newmem + 1;	/* Add one sizeof(_memptr) */
	newmem->size = size;

	/* Put this at the head of the list */
	alloc = newmem;

	ggz_debug("MEMDETAIL", "%d bytes allocated at %p from %s/%d",
		   size, newmem->ptr, tag, line);

	return newmem->ptr;
}


void * _ggz_malloc(const unsigned int size, char *tag, int line)
{
	void *new;

	/* Get a chunk of memory */
	new = _ggz_allocate(size, tag, line);

	/* Clear the memory to zilcho */
	memset(new, 0, size);

	return new;
}


void * _ggz_realloc(const void *ptr, const unsigned size,char *tag,int line)
{
	struct _memptr *targetmem;
	void *new;

	/* Search through allocated memory for this chunk */
	targetmem = alloc;
	while(targetmem != NULL && ptr != targetmem->ptr) {
		targetmem = targetmem->next;
	}

	/* This memory was never allocated via ggz */
	if(targetmem == NULL) {
		ggz_error_msg("Memory reallocation <%p> failure: %s/%d",
			       ptr, tag, line);
		return NULL;
	}

	/* Try to allocate our memory */
	ggz_debug("MEMDETAIL",
		   "Reallocating %d bytes at %p to %d bytes from %s/%d",
		   targetmem->size, targetmem->ptr, size, tag, line);
	new = _ggz_allocate(size, tag, line);

	/* Copy the old to the new */
	if(size > targetmem->size) {
		memcpy(new, targetmem->ptr, targetmem->size);
		/* And zero out the rest of the block */
		memset(new+targetmem->size, 0, size-targetmem->size);
	} else
		memcpy(new, targetmem->ptr, size);


	/* And free the old chunk */
	_ggz_free(targetmem->ptr, tag, line);

	return new;
}


int _ggz_free(const void *ptr, char *tag, int line)
{
	struct _memptr *prev, *targetmem;

	/* Search through allocated memory for this chunk */
	prev = NULL;
	targetmem = alloc;
	while(targetmem != NULL && ptr != targetmem->ptr) {
		prev = targetmem;
		targetmem = targetmem->next;
	}

	/* This memory was never allocated via ggz */
	if(targetmem == NULL) {
		ggz_error_msg("Memory deallocation <%p> failure: %s/%d",
			       ptr, tag, line);
		return -1;
	}

	/* Juggle pointers so we can remove this item */
	if(prev == NULL)
		alloc = targetmem->next;
	else
		prev->next = targetmem->next;

	ggz_debug("MEMDETAIL", "%d bytes deallocated at %p from %s/%d",
		   targetmem->size, ptr, tag, line);

	free(targetmem);

	return 0;
}


int ggz_memory_check(void)
{
	struct _memptr *memptr;

	ggz_debug("MEMCHECK", "*** Memory Leak Check ***");
	if(alloc != NULL) {
		memptr = alloc;
		while(memptr != NULL) {
			ggz_debug("MEMCHECK",
				   "%d bytes left allocated at %p by %s/%d",
				   memptr->size, memptr->ptr,
				   memptr->tag, memptr->line);
			memptr = memptr->next;
		}
		ggz_debug("MEMCHECK", "*** End Memory Leak Check ***");

		return -1;
	}

	ggz_debug("MEMCHECK", "All clean!");
	ggz_debug("MEMCHECK", "*** End Memory Leak Check ***");

	return 0;
}


char * _ggz_strdup(const char *src, char *tag, int line)
{
	unsigned len;
	char *new;

	if(src == NULL) {
		ggz_error_msg("_ggz_strdup - NULL pointer from %s/%d",
			       tag, line);
		return NULL;
	}

	len = strlen(src);

	ggz_debug("MEMDETAIL",
		   "Allocating memory for length %d string from %s/%d",
		   len+1, tag, line);

	new = _ggz_allocate(len+1, tag, line);

	memcpy(new, src, len+1);

	return new;
}
