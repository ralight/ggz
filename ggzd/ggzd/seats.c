/*
 * File: seats.c
 * Author: Brent Hendricks
 * Project: GGZ Server
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

#include <string.h>

#include <ggzd.h>
#include <table.h>
#include <seats.h>


int seats_open(GGZTable* table)
{
	int i, count = 0;
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		if (strcmp(table->seats[i], "<open>") == 0)
			count++;
	return count;
}


int seats_num(GGZTable* table)
{
	int i;
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		if (strcmp(table->seats[i], "<none>") == 0)
			break;
	return i;
}


int seats_bot(GGZTable* table)
{
	int i, count = 0;
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		if (strcmp(table->seats[i], "<bot>") == 0)
			count++;
	return count;
}


int seats_reserved(GGZTable* table)
{
	int i, count = 0;
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		if (strcmp(table->seats[i], "<reserved>") == 0)
			count++;
	return count;
}


int seats_human(GGZTable* table)
{
	int i, count = 0;
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		if (strcmp(table->seats[i], "<none>")
		    && strcmp(table->seats[i], "<open>")
		    && strcmp(table->seats[i], "<bot>")
		    && strcmp(table->seats[i], "<reserved>"))
			count++;
	
	return count;
}


int seats_type(GGZTable* table, int seat)
{
	if (strcmp(table->seats[seat], "<none>") == 0)
		return GGZ_SEAT_NONE;
	if (strcmp(table->seats[seat], "<open>") == 0)
		return GGZ_SEAT_OPEN;
	if (strcmp(table->seats[seat], "<bot>") == 0)
		return GGZ_SEAT_BOT;
	if (strcmp(table->seats[seat], "<reserved>") == 0)
		return GGZ_SEAT_RESV;
	else /* Some player */
		return GGZ_SEAT_PLAYER;
}
