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

/* Table Information */
struct _GGZTable {
 
        /* Table number */
        int number;
 
        /* Game Type */
        int type;

        /* Table state */
        char *state;

        /* Total seats */
        int seats;

        /* Open seats */
        int open;
        
        /* Computer seats */
        int computers;

	/* Table description */
	char *desc;        
};



void _ggzcore_table_list_clear(void);

int _ggzcore_table_list_add(const int table, const int type, const char* desc, const char state, const int seats, const int open);

int _ggzcore_table_list_remove(const int number);

int _ggzcore_table_list_join(const int number);

int _ggzcore_table_list_leave(const int number);

int _ggzcore_table_list_replace(const char* name, const int table);


#endif /* __TABLE_H_ */
