/*
 * File: seats.c
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 3/3/00
 * Desc: Support functions for table seats
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
#include <config.h>		/* Site specific config */

#include <sys/socket.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include <easysock.h>

#include <datatypes.h>
#include <seats.h>


int seats_open(TableInfo table)
{
	int i, count = 0;
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		if (table.seats[i] == GGZ_SEAT_OPEN)
			count++;
	return count;
}


int seats_num(TableInfo table)
{
	int i;
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		if (table.seats[i] == GGZ_SEAT_NONE)
			break;
	return i;
}


int seats_comp(TableInfo table)
{
	int i, count = 0;
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		if (table.seats[i] == GGZ_SEAT_COMP)
			count++;
	return count;
}


int seats_reserved(TableInfo table)
{
	int i, count = 0;
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		if (table.seats[i] == GGZ_SEAT_RESV)
			count++;
	return count;
}


int seats_human(TableInfo table)
{
	int i, count = 0;
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		if (table.seats[i] >= 0 )
			count++;
	
	return count;
}

