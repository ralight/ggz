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
#include <pthread.h>

#include "ggz.h"
#include "msg.h"


/* This is used basically as a LIFO stack, on the basis that most malloc/free
 * pairs are somewhat close together, so keep recent mallocs near the top */
typedef struct _memptr {
	struct _memptr	*next;
	const char	*tag;
	int		line;
	void		*ptr;
	unsigned int	size;
} _memptr;

static struct _memptr	*alloc = NULL;

#define SET_LOCK 1
#define GOT_LOCK 2
static pthread_mutex_t	mut = PTHREAD_MUTEX_INITIALIZER;


static void * _ggz_allocate(const unsigned int size,
                            const char *tag, int line, int lock)
{
	struct _memptr *newmem;

	/* Try to allocate our memory */
	newmem = malloc(sizeof(_memptr) + size); /* should be only "malloc" call. */
	if(newmem == NULL)
		ggz_error_sys_exit("Memory allocation failure: %s/%d", tag, 
				    line);

	/* We've got the memory, so set it up */
	newmem->tag = tag;
	newmem->line = line;
	newmem->ptr = newmem + 1;	/* Add one sizeof(_memptr) */
	newmem->size = size;

	/* Put this at the head of the list */
	if(lock == SET_LOCK)
		pthread_mutex_lock(&mut);
	newmem->next = alloc;
	alloc = newmem;
	if(lock == SET_LOCK)
		pthread_mutex_unlock(&mut);

	_ggz_debug("MEMDETAIL", "%d bytes allocated at %p from %s/%d",
		   size, newmem->ptr, tag, line);

	return newmem->ptr;
}


void * _ggz_malloc(const size_t size, const char *tag, int line)
{
	void *new;

	/* Sanity checks */
	if(!tag)
		tag = "<unknown>";
	if(!size) {
		ggz_error_msg("ggz_malloc: 0 byte malloc requested from %s/%d",
			      tag, line);
		return NULL;
	}

	/* Get a chunk of memory */
	new = _ggz_allocate(size, tag, line, SET_LOCK);

	/* Clear the memory to zilcho */
	memset(new, 0, size);

	return new;
}


void * _ggz_realloc(const void *ptr, const size_t size,
                    const char *tag, int line)
{
	struct _memptr *targetmem;
	void *new;

	/* Sanity checks */
	if(!tag)
		tag = "<unknown>";
	if(!size) {
		_ggz_free(ptr, tag, line);
		return NULL;
	}

	/* If ptr is NULL, treat this like a call to malloc */
	if (ptr == NULL)
		return _ggz_malloc(size, tag, line);

	/* Search through allocated memory for this chunk */
	pthread_mutex_lock(&mut);
	targetmem = alloc;
	while(targetmem != NULL && ptr != targetmem->ptr) {
		targetmem = targetmem->next;
	}

	/* This memory was never allocated via ggz */
	if(targetmem == NULL) {
		pthread_mutex_unlock(&mut);
		ggz_error_msg("Memory reallocation <%p> failure: %s/%d",
			       ptr, tag, line);
		return NULL;
	}

	/* Try to allocate our memory */
	new = _ggz_allocate(size, tag, line, GOT_LOCK);

	/* Copy the old to the new */
	if(size > targetmem->size) {
		memcpy(new, targetmem->ptr, targetmem->size);
		/* And zero out the rest of the block */
		memset(new+targetmem->size, 0, size-targetmem->size);
	} else
		memcpy(new, targetmem->ptr, size);
	pthread_mutex_unlock(&mut);

	_ggz_debug("MEMDETAIL",
		   "Reallocated %d bytes at %p to %d bytes from %s/%d",
		   targetmem->size, targetmem->ptr, size, tag, line);

	/* And free the old chunk */
	_ggz_free(targetmem->ptr, tag, line);

	return new;
}


int _ggz_free(const void *ptr, const char *tag, int line)
{
	struct _memptr *prev, *targetmem;
	unsigned int oldsize;

	/* Sanity checks */
	if(!tag)
		tag = "<unknown>";

	/* Search through allocated memory for this chunk */
	prev = NULL;
	pthread_mutex_lock(&mut);
	targetmem = alloc;
	while(targetmem != NULL && ptr != targetmem->ptr) {
		prev = targetmem;
		targetmem = targetmem->next;
	}

	/* This memory was never allocated via ggz */
	if(targetmem == NULL) {
		pthread_mutex_unlock(&mut);
		ggz_error_msg("Memory deallocation <%p> failure: %s/%d",
			       ptr, tag, line);
		return -1;
	}

	/* Juggle pointers so we can remove this item */
	if(prev == NULL)
		alloc = targetmem->next;
	else
		prev->next = targetmem->next;
	oldsize = targetmem->size;
	pthread_mutex_unlock(&mut);

	_ggz_debug("MEMDETAIL", "%d bytes deallocated at %p from %s/%d",
		   oldsize, ptr, tag, line);

	free(targetmem); /* should be the only "free" call */

	return 0;
}


int ggz_memory_check(void)
{
	int flag = 0;
	struct _memptr *memptr;

	_ggz_msg("*** Memory Leak Check ***");

	pthread_mutex_lock(&mut);
	if(alloc != NULL) {
		memptr = alloc;
		while(memptr != NULL) {
			_ggz_msg("%d bytes left allocated at %p by %s/%d",
				 memptr->size, memptr->ptr,
				 memptr->tag, memptr->line);
			memptr = memptr->next;
		}
		
		flag = -1;
	}
	else
		_ggz_msg("All clean!");
	pthread_mutex_unlock(&mut);
	
	_ggz_msg("*** End Memory Leak Check ***");

	return flag;
}


char * _ggz_strdup(const char *src, const char *tag, int line)
{
	unsigned len;
	char *new;

	/* If you ask for a copy of NULL, you get NULL back */
	if (src == NULL)
		return NULL;

	/* Sanity check */
	if(!tag)
		tag = "<unknown>";


	len = strlen(src);

	_ggz_debug("MEMDETAIL",
		   "Allocating memory for length %d string from %s/%d",
		   len+1, tag, line);

	new = _ggz_allocate(len+1, tag, line, SET_LOCK);

	memcpy(new, src, len+1);

	return new;
}
