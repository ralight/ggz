/* $Id: common.c 2072 2001-07-23 00:38:33Z jdorje $ */
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

#include <assert.h>
#include <stdlib.h>

#include <easysock.h>
#include <ggz_client.h>

#include "common.h"

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


void client_debug(const char *fmt, ...)
{
	/* Currently the output goes to stderr, but it could be sent elsewhere. */
	/* TODO: having this within #ifdef's wouldn't work if it was an external lib */
#ifdef DEBUG
	char buf[512];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	fprintf(stderr, "DEBUG: %s\n", buf);
	va_end(ap);
#endif /* DEBUG */
}


int handle_message_global()
{
	char *mark;
	char *message;

	if (es_read_string_alloc(ggzfd, &mark) < 0 ||
	    es_read_string_alloc(ggzfd, &message) < 0)
		return -1;
	assert(message);

	client_debug("     Global message received, marked as '%s':%s", mark,
		     message);

	table_set_global_message(mark, message);

	free(message);
	free(mark);

	return 0;
}


int handle_message_player()
{
	int p;
	char *message;
	if (es_read_int(ggzfd, &p) < 0 ||
	    es_read_string_alloc(ggzfd, &message) < 0)
		return -1;
	assert(p >= 0);
	/* assert(p < game.num_players); */

	client_debug("    Player message recived:%s", message);

	table_set_player_message(p, message);

	free(message);

	return 0;
}
