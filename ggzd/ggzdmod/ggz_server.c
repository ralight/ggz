/* 
 * File: ggz_server.c
 * Author: GGZ Dev Team
 * Project: GGZDMOD
 * Date: 10/24/01
 * Desc: GGZDMOD wrapper
 * $Id: ggz_server.c 2820 2001-12-09 07:38:20Z jdorje $
 *
 * Copyright (C) 2001 GGZ Dev Team.
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

#include "ggz_server.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GGZdMod *ggzdmod = NULL;

int ggzd_set_player_name(int seat, char *name)
{
	GGZSeat myseat = ggzdmod_get_seat(ggzdmod, seat);
	myseat.name = name;
	ggzdmod_set_seat(ggzdmod, &myseat);
	return 0;
}

int ggzd_debug(const char *fmt, ...)
{
	char buffer[4096];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, ap);
	va_end(ap);

	return ggzdmod_log(ggzdmod, "%s", buffer);
}
