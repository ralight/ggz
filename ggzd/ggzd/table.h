/*
 * File: table.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 1/9/00
 * Desc: Functions for handling tables
 *
 * Copyright (C) 1999 Brent Hendricks.
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

#ifndef _GGZ_TABLE_H
#define _GGZ_TABLE_H

#include <pthread.h>

#include <ggzd.h>

/* 
 * The TableInfo structure contains information about a single game
 * table 
 */
typedef struct TableInfo {

	/* Type of game being played on table */
	int type_index;

	/* Room in which game exists */
	int room;

	/* State of game: One of GGZ_TABLE_XXXX */
	char state;

	/* 
	 * Condition variable and corresponding mutex for
	 * signaling a change in state across threads
	 */
	pthread_cond_t state_cond;
	pthread_mutex_t state_lock;
	
	/* 
	 * Variables to hold player and related data during transit to
	 * and from table
	 */
	int transit;
	int transit_fd;
	int transit_seat;

	/* Transit status flags: Bitmak of GGZ_TRANSIT_XXX */
	unsigned char transit_flag;

	/* 
	 * Condition variable and corresponding mutex for
	 * signaling a change in transit state across threads
	 */
	pthread_cond_t transit_cond;
	pthread_mutex_t transit_lock;

	/* File descriptor for communicating withgame server module */
	int fd_to_game;

	/* Process ID of game server module running this game-table */
	int pid;

	/* Seat assignments */
	char* seats[MAX_TABLE_SIZE];

	/* Seat reservations */
	char* reserve[MAX_TABLE_SIZE];

	/* Client-provided description of this table */
	char desc[MAX_GAME_DESC_LEN + 1];
	
} TableInfo;


/* Array of game-tables, their mutex, and a counter */
struct GameTables {
	TableInfo info[MAX_TABLES];
	int count;
	pthread_rwlock_t lock;
};


/* Launch a table */
int table_launch(int p, TableInfo table, int* t_index);

/* Join a player to the table */
int table_join(int p, int t_index, int* t_fd);

/* Pull player from table */
int table_leave(int p, int t_index);

#endif





