/*
 * File: ggzdb_db2.h
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 06/11/2000
 * Desc: Back-end functions for handling the db2 style database
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

/* Exported functions */
extern int _ggzdb_init(char *datadir);
extern int _ggzdb_init_player(char *datadir);
extern int _ggzdb_player_add(ggzdbPlayerEntry *);

/* Use these to define any code which MUST be called upon entry/exit
 * from the database code. 
 *
 * NOTE: These execute in the front-end and as such cannot access variaables
 * private to the back-end.  One way around this would be to have them call
 * a back-end function.
 */
#define DB_ENTER	pthread_rwlock_wrlock(&db_thread_lock)
#define DB_EXIT		pthread_rwlock_unlock(&db_thread_lock)
