/*
 * File: player.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
 *
 * This fils contains functions for handling players
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#ifndef __TABLE_H_
#define __TABLE_H_

#include "gametype.h"


/* Table Information */
struct _GGZTable {
 
        /* Table number */
        int id;
 
        /* Game Type */
	struct _GGZGameType *gametype;

        /* Table state */
        char state;

        /* Total seats */
        int seats;

        /* Open seats */
        int open;
        
        /* Computer seats */
        int bots;

	/* Table description */
	char *desc;        
};



struct _ggzcore_list* _ggzcore_table_list_new(void);

void _ggzcore_table_init(struct _GGZTable *table, 
			 const int id,
			 struct _GGZGameType *gametype,
			 const char state,
			 const int seats,
			 const int open,
			 const int bots,
			 const char *desc);
	

unsigned int          _ggzcore_table_get_num(struct _GGZTable *table);
struct _GGZGameType*  _ggzcore_table_get_type(struct _GGZTable *table);
char                  _ggzcore_table_get_state(struct _GGZTable *table);
int                   _ggzcore_table_get_seats(struct _GGZTable *table);
int                   _ggzcore_table_get_open(struct _GGZTable *table);
int                   _ggzcore_table_get_bots(struct _GGZTable *table);
char*                 _ggzcore_table_get_desc(struct _GGZTable *table);

#endif /* __TABLE_H_ */
