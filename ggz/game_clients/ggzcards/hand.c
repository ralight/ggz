/* $Id: hand.c 2073 2001-07-23 07:47:48Z jdorje $ */
/*
 * File: hand.c
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Functions to get cards from server
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>

#include <easysock.h>

#include "common.h"

#include "game.h"
#include "table.h"
#include "hand.h"
#include "layout.h"

int table_verify_hand_size()
{
	/* the inner table must be at least large enough.
	 * So, if it's not we make the hand sizes larger. */
	int x, y, w, h, w1, h1;
	get_table_dim(&x, &y, &w, &h);
	get_fulltable_size(&w1, &h1);
	if (w1 > w && h1 > h)
		return 1;
	return 0;
}
