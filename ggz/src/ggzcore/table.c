/*
 * File: table.c
 * Author: Justin Zaun
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
 *
 * This fils contains functions for handling tables
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


#include <config.h>
#include <table.h>
#include <lists.h>
#include <msg.h>

#include <stdlib.h>
#include <string.h>

/* 
 * The Table structure is meant to be a node in a linked list of
 * the tables in the current room .
 */

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


/* List of tables in current room */
static struct _ggzcore_list *table_list;
static unsigned int num_tables;

/* Local functions for manipulating table list */
static void _ggzcore_table_list_print(void);
static void _ggzcore_table_print(struct _GGZTable*);

/* Utility functions used by _ggzcore_list */
static int   _ggzcore_table_compare(void* p, void* q);
static void* _ggzcore_table_create(void* p);
static void  _ggzcore_table_destroy(void* p);


void _ggzcore_table_list_clear(void)
{
	if (table_list)
		_ggzcore_list_destroy(table_list);
	
	table_list = _ggzcore_list_create(_ggzcore_table_compare,
					  _ggzcore_table_create,
					  _ggzcore_table_destroy,
					  0);
	num_tables = 0;
}


int _ggzcore_table_list_add(const int table, const int type, const char* desc, const char state, const int seats, const int open)
{
	int status;
	struct _GGZTable new_table;

	ggzcore_debug(GGZ_DBG_PLAYER, "Adding table number %d to table list", table);

	new_table.number = table;
	new_table.type = type;
	new_table.seats = seats;
	new_table.open = open;
	new_table.computers = 0;
	new_table.state = strdup(&state);
	new_table.desc = strdup(desc);

	status = _ggzcore_list_insert(table_list, (void*)&new_table);
	if(status == 0)
		num_tables++;
	_ggzcore_table_list_print();

	return 0;
}


int _ggzcore_table_list_remove(const int number)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZTable table;

	ggzcore_debug(GGZ_DBG_PLAYER, "Removing table number %d.", number);

	table.number = number;
	if (!(entry = _ggzcore_list_search(table_list, &table)))
		return -1;

	_ggzcore_list_delete_entry(table_list, entry);
	num_tables--;
	_ggzcore_table_list_print();

	return 0;
}


int _ggzcore_table_list_join(const int number)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZTable data, *table;

	ggzcore_debug(GGZ_DBG_PLAYER, "Player joined table %d", number);

	data.number = number;
	if (!(entry = _ggzcore_list_search(table_list, &data)))
		return -1;

	/* Update information */
	table = _ggzcore_list_get_data(entry);
	table->open--;

	_ggzcore_table_list_print();

	return 0;
}


int _ggzcore_table_list_leave(const int number)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZTable data, *table;

	ggzcore_debug(GGZ_DBG_PLAYER, "Player left table %d", number);

	data.number = number;
	if (!(entry = _ggzcore_list_search(table_list, &data)))
		return -1;

	/* Update information */
	table = _ggzcore_list_get_data(entry);
	table->open++;

	_ggzcore_table_list_print();

	return 0;
}


static int _ggzcore_table_compare(void* p, void* q)
{
	if(((struct _GGZTable*)p)->number == ((struct _GGZTable*)q)->number)
		return 0;
	if(((struct _GGZTable*)p)->number > ((struct _GGZTable*)q)->number)
		return -1;
	if(((struct _GGZTable*)p)->number < ((struct _GGZTable*)q)->number)
		return 1;

	return 0;
}


static void* _ggzcore_table_create(void* p)
{
	struct _GGZTable *new, *src = p;

	if (!(new = malloc(sizeof(struct _GGZTable))))
		ggzcore_error_sys_exit("malloc failed in table_create");

	new->number = src->number;
	new->type = src->type;
	new->seats = src->seats;
	new->open = src->open;
	new->computers = src->computers;
	new->state = src->state;
	new->desc = strdup(src->desc);

	return (void*)new;
}


static void  _ggzcore_table_destroy(void* p)
{
	struct _GGZTable *table = p;

	if (table->desc)
		free(table->desc);
	free(p);
}


static void _ggzcore_table_list_print(void)
{
	struct _ggzcore_list_entry *cur;
	
	for (cur = _ggzcore_list_head(table_list); cur; cur = _ggzcore_list_next(cur))
		_ggzcore_table_print(_ggzcore_list_get_data(cur));
}


static void _ggzcore_table_print(struct _GGZTable *table)
{
	ggzcore_debug(GGZ_DBG_PLAYER, "Table number: %d", table->number);
	ggzcore_debug(GGZ_DBG_PLAYER, "Table type: %d\n", table->number);
	ggzcore_debug(GGZ_DBG_PLAYER, "Table seats: %d\n", table->seats);
	ggzcore_debug(GGZ_DBG_PLAYER, "Table open: %d\n", table->open);
	ggzcore_debug(GGZ_DBG_PLAYER, "Table computers: %d\n", table->computers);
	ggzcore_debug(GGZ_DBG_PLAYER, "Table state: %d\n", table->state);
	ggzcore_debug(GGZ_DBG_PLAYER, "Table desc: %s\n", table->desc);
}



/*
 * Lookup functions
 */

unsigned int ggzcore_table_get_num(void)
{
	return num_tables;
}

int ggzcore_table_get_seats(int number) 
{
        struct _ggzcore_list_entry *cur;
        struct _GGZTable *table;

        if (num_tables >= 0) {
                cur = _ggzcore_list_head(table_list);
                while (cur) {
                        table = _ggzcore_list_get_data(cur);
                        if(table->number == number)
                                return table->seats;
                        cur = _ggzcore_list_next(cur);
                }
        }
                
	return -1;
}

int ggzcore_table_get_open(int number) 
{
        struct _ggzcore_list_entry *cur;
        struct _GGZTable *table;

        if (num_tables >= 0) {
                cur = _ggzcore_list_head(table_list);
                while (cur) {
                        table = _ggzcore_list_get_data(cur);
                        if(table->number == number)
                                return table->open;
                        cur = _ggzcore_list_next(cur);
                }
        }
                
	return -1;
}

char *ggzcore_table_get_desc(int number) 
{
        struct _ggzcore_list_entry *cur;
        struct _GGZTable *table;

        if (num_tables >= 0) {
                cur = _ggzcore_list_head(table_list);
                while (cur) {
                        table = _ggzcore_list_get_data(cur);
                        if(table->number == number)
                                return table->desc;
                        cur = _ggzcore_list_next(cur);
                }
        }
                
	return "";
}

int* ggzcore_table_get_numbers(void)
{
        int i = 0;
        int *nums = NULL;
        struct _ggzcore_list_entry *cur;
        struct _GGZTable *table;
        
        if (num_tables >= 0) {
                if (!(nums = calloc((num_tables + 1), sizeof(int*))))
                        ggzcore_error_sys_exit("calloc() failed in table_get_numbers");
                cur = _ggzcore_list_head(table_list);
                while (cur) {
                        table = _ggzcore_list_get_data(cur);
                        nums[i++] = table->number;
                        cur = _ggzcore_list_next(cur);
                }
        }

        return nums;
}

