/* $id$ */
/*
 * File: common.c
 * Author: Jason Short
 * Project: GGZCards Client-Common
 * Date: 07/22/2001
 * Desc: Frontend to GGZCards Client-Common
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

#include <stdlib.h>

#include "ggz_client.h"

int ggzfd;

void client_initialize(void)
{
	ggz_client_init("GGZCards");
	ggzfd = ggz_client_connect();
	if (ggzfd < 0)
		exit(-1);
}


void client_quit(void)
{
	ggz_client_quit();
}
