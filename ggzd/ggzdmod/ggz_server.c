/* 
 * File: ggz_server.c
 * Author: GGZ Dev Team
 * Project: GGZDMOD
 * Date: 10/24/01
 * Desc: GGZDMOD wrapper
 * $Id: ggz_server.c 2800 2001-12-07 03:41:16Z jdorje $
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

static GGZDHandler handlers[6] = { NULL };

static void ggzd_ggzdmod_event_handler(GGZdMod * ggzdmod, GGZdModEvent e,
				       void *data)
{
	if (e == GGZDMOD_EVENT_STATE
	    && ggzdmod_get_state(ggzdmod) != GGZDMOD_STATE_WAITING)
		return;
	if (handlers[e])
		(*handlers[e]) (e, data);
}

void ggzd_set_handler(GGZdModEvent event_id, const GGZDHandler handler)
{
	handlers[event_id] = handler;
	if (ggzdmod)
		ggzdmod_set_handler(ggzdmod, event_id,
				    ggzd_ggzdmod_event_handler);
}

int ggzd_main_loop(void)
{
	GGZdModEvent i;
	
	ggzdmod = ggzdmod_new(GGZDMOD_GAME);
	ggzdmod_connect(ggzdmod);
	
	for (i = 0; i < 6; i++)
		ggzdmod_set_handler(ggzdmod, i, ggzd_ggzdmod_event_handler);
		
	ggzdmod_loop(ggzdmod);
	
	ggzdmod_disconnect(ggzdmod);
	ggzdmod_free(ggzdmod);
	
	return 0;
}
