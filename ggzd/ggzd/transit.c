/*
 * File: transit.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/26/00
 * Desc: Functions for handling table transits
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

#include <config.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include <ggzd.h>
#include <datatypes.h>
#include <protocols.h>
#include <err_func.h>
#include <transit.h>
#include <easysock.h>
#include <seats.h>

/* Server wide data structures*/
extern struct GameTypes game_types;
extern struct GameTables tables;
extern struct Users players;

/* Local functions for handling transits */
static int   transit_join(int index, int fd);
static int   transit_leave(int index, int fd);


/*
 * transit_handle check for any impending transits.
 * Before calling, the transit must be *unlocked*
 * If it returns a 1, the transit is locked.
 * If it returns <= 0, the transit is unlocked.
 */
int transit_handle(int index, int fd)
{
	unsigned char flag;
	char status;
	
	pthread_mutex_lock(&tables.info[index].transit_lock);
	flag = tables.info[index].transit_flag;

	if (flag == GGZ_TRANSIT_JOIN)
		status = transit_join(index, fd);
	else if (flag == GGZ_TRANSIT_LEAVE)
		status = transit_leave(index, fd);
	else {
		pthread_mutex_unlock(&tables.info[index].transit_lock);
		return 0;
	}

	dbg_msg(GGZ_DBG_TABLE, "Transit handled with result %d", status);
	
	/* Signal failure immediately */
	if (status < 0) {
		/* Mark ERR and clear JOIN and LEAVE in process flags */
		tables.info[index].transit_flag |= GGZ_TRANSIT_ERR;
		tables.info[index].transit_flag &= ~GGZ_TRANSIT_JOIN;
		tables.info[index].transit_flag &= ~GGZ_TRANSIT_LEAVE;
		pthread_cond_broadcast(&tables.info[index].transit_cond);
		pthread_mutex_unlock(&tables.info[index].transit_lock);
		return status;
	}

	/* Indicate that transit is now locked */
	return 1;
}


/*
 * transit_join sends REQ_GAME_JOIN to table
 * Must be called with transit locked
 * Returns with transit locked
 */
static int transit_join(int index, int t_fd)
{
	int seats, i, p, fd[2];
	char name[MAX_USER_NAME_LEN + 1];

	dbg_msg(GGZ_DBG_TABLE, "Handling transit join for table %d", index);
		
	/* Find my seat or unoccupied one */
	/* FIXME: look for reserved seat */
	pthread_rwlock_rdlock(&tables.lock);
	seats  = seats_num(tables.info[index]);
	for (i = 0; i < seats; i++)
		if (tables.info[index].seats[i] == GGZ_SEAT_OPEN)
			break;
	pthread_rwlock_unlock(&tables.lock);
	
	/* If table already full */
	if (i == seats)
		return -1;
	
	/* Create socket for communication with player thread */
	if (socketpair(PF_UNIX, SOCK_STREAM, 0, fd) < 0)
		err_sys_exit("socketpair failed");
	
	tables.info[index].transit_seat = i;
	tables.info[index].transit_fd = fd[1];
	p = tables.info[index].transit;

	pthread_rwlock_rdlock(&players.lock);
	strcpy(name, players.info[p].name);
	pthread_rwlock_unlock(&players.lock);

	/* Send MSG_TABLE_JOIN to table */
	if (es_write_int(t_fd, REQ_GAME_JOIN) < 0
	    || es_write_int(t_fd, i) < 0
	    || es_write_string(t_fd, name) < 0
	    || es_write_fd(t_fd, fd[0]) < 0)
		return -1;

	/* Must close remote end of socketpair*/
	close(fd[0]);
	tables.info[index].transit_flag |= GGZ_TRANSIT_SENT;
	
	return 0;
}

	
/*
 * transit_join sends REQ_GAME_LEAVE to table
 * Must be called with transit locked
 * Returns with transit locked
 */
static int transit_leave(int index, int t_fd)
{
	int seats, i, p;
	char name[MAX_USER_NAME_LEN + 1];
	
	dbg_msg(GGZ_DBG_TABLE, "Handling transit leave for table %d", index);
	
	p = tables.info[index].transit;
	pthread_rwlock_rdlock(&tables.lock);
	seats  = seats_num(tables.info[index]);
	for (i = 0; i < seats; i++)
		if (tables.info[index].seats[i] == p)
			break;
	pthread_rwlock_unlock(&tables.lock);

	/* If player not there, we have a problem! */
	if (i == seats)
		return -1;

	tables.info[index].transit_seat = i;

	pthread_rwlock_rdlock(&players.lock);
	strcpy(name, players.info[p].name);
	pthread_rwlock_unlock(&players.lock);
	
	/* Send MSG_TABLE_LEAVE to table */
	if (es_write_int(t_fd, REQ_GAME_LEAVE) < 0
	    || es_write_string(t_fd, name) < 0)
		return -1;

	tables.info[index].transit_flag |= GGZ_TRANSIT_SENT;
		
	return 0;
}




