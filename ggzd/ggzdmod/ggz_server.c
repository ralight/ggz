/* 
 * File: ggz_server.c
 * Author: GGZ Dev Team
 * Project: GGZDMOD
 * Date: 10/24/01
 * Desc: GGZDMOD wrapper
 * $Id: ggz_server.c 2632 2001-11-03 05:54:37Z jdorje $
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

static GGZdMod *ggzdmod = NULL;

ggzd_assign_t ggzd_get_seat_status(int seat)
{
	GGZSeat *myseat = ggzdmod_get_seat(ggzdmod, seat);
	if (!myseat)
		return GGZ_SEAT_NONE;
	return myseat->type;
}

int ggzd_set_seat_status(int seat, ggzd_assign_t status)
{
	GGZSeat *myseat = ggzdmod_get_seat(ggzdmod, seat);
	if (!myseat)
		return -1;
	myseat->type = status;
	return 0;
}

const char *ggzd_get_player_name(int seat)
{
	GGZSeat *myseat = ggzdmod_get_seat(ggzdmod, seat);
	if (!myseat)
		return NULL;
	return myseat->name;
}

int ggzd_set_player_name(int seat, char *name)
{
	GGZSeat *myseat = ggzdmod_get_seat(ggzdmod, seat);
	if (!myseat)
		return -1;
	if (myseat->name)
		free(myseat->name);
	myseat->name = strdup(name);
	return 0;
}

int ggzd_get_player_socket(int seat)
{
	GGZSeat *myseat = ggzdmod_get_seat(ggzdmod, seat);
	if (!myseat)
		return -1;
	return myseat->fd;
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

int ggzd_get_seats_num(void)
{
	return ggzdmod_get_num_seats(ggzdmod);
}

int ggzd_get_seat_count(ggzd_assign_t status)
{
	int i, count = 0;
	for (i = 0; i < ggzdmod_get_num_seats(ggzdmod); i++)
		if (ggzdmod_get_seat(ggzdmod, i) &&
		    ggzdmod_get_seat(ggzdmod, i)->type == status)
			count++;
	return count;
}

void ggzd_gameover(int status)
{
	ggzdmod_halt_game(ggzdmod);
}

int ggzd_get_gameover(void)
{
	return ggzdmod_get_state(ggzdmod) == GGZ_STATE_GAMEOVER;
}

static GGZDHandler handlers[6] = { NULL };

static void ggzd_ggzdmod_event_handler(GGZdMod * ggzdmod, GGZdModEvent e,
				       void *data)
{
	if (e == GGZ_EVENT_STATE
	    && ggzdmod_get_state(ggzdmod) != GGZ_STATE_LAUNCHED)
		return;
	if (handlers[e])
		(*handlers[e]) (e, data);
}

void ggzd_set_handler(ggzd_event_t event_id, const GGZDHandler handler)
{
	handlers[event_id] = handler;
	if (ggzdmod)
		ggzdmod_set_handler(ggzdmod, event_id,
				    ggzd_ggzdmod_event_handler);
}

int ggzd_connect(void)
{
	ggzdmod = ggzdmod_new(GGZDMOD_GAME);
	ggzdmod_connect(ggzdmod);
	return ggzdmod_get_fd(ggzdmod);
}

int ggzd_disconnect(void)
{
	ggzdmod_disconnect(ggzdmod);
	ggzdmod_free(ggzdmod);
	return 0;
}

void ggzd_dispatch(void)
{
	ggzdmod_dispatch(ggzdmod);
}

void ggzd_io_read_all(void)
{
	ggzdmod_dispatch(ggzdmod);
}

int ggzd_main_loop(void)
{
	ggzd_event_t i;
	ggzd_connect();
	for (i = 0; i < 6; i++)
		ggzdmod_set_handler(ggzdmod, i, ggzd_ggzdmod_event_handler);
	ggzdmod_loop(ggzdmod);
	ggzd_disconnect();
	return 0;
}
