/*
 * File: stack.h
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

#ifndef __STACK_H__
#define __STACK_H__

#include "lists.h"

/* Use list for implementing stack */
typedef struct _ggzcore_list GGZStack;

GGZStack* _ggzcore_stack_new(void);
void _ggzcore_stack_push(GGZStack*, void*);
void* _ggzcore_stack_pop(GGZStack*);
void* _ggzcore_stack_top(GGZStack*);
void _ggzcore_stack_free(GGZStack*);


#endif /* __STACK_H__ */
