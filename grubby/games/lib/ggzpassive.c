/*
 * Noninteractive game module support for Guru
 * Copyright (C) 2004 Josef Spillner, josef@ggzgamingzone.org
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <ggzmod.h>

#include "ggzpassive.h"

static GGZMod *ggz_mod = NULL;
static GGZGameFunction ggz_gamehandler = NULL;
int ggz_gamefd = -1;
int ggz_done = 0;

static void ggz_handle_ggz()
{
	ggzmod_dispatch(ggz_mod);
}

static void ggz_handle_server(GGZMod *ggzmod, GGZModEvent e, const void *data)
{
	const int *fd = data;

	ggz_gamefd = *fd;

	ggzmod_set_state(ggz_mod, GGZMOD_STATE_PLAYING);
}

static void ggz_init()
{
	int ret;

	ggz_mod = ggzmod_new(GGZMOD_GAME);
	ggzmod_set_handler(ggz_mod, GGZMOD_EVENT_SERVER, &ggz_handle_server);

	ret = ggzmod_connect(ggz_mod);
	if(ret < 0)
	{
		fprintf(stderr, "Couldn't initialize GGZ.\n");
		exit(-1);
	}
}

static void ggz_finish()
{
	ggzmod_disconnect(ggz_mod);
	ggzmod_free(ggz_mod);
}

static void ggz_network()
{
	int maxfd;
	fd_set set;
	struct timeval tv;
	int ret;
	int serverfd;

	FD_ZERO(&set);

	serverfd = ggzmod_get_fd(ggz_mod);

	FD_SET(serverfd, &set);
	maxfd = serverfd;
	if(ggz_gamefd >= 0)
	{
		FD_SET(ggz_gamefd, &set);
		if(ggz_gamefd > maxfd) maxfd = ggz_gamefd;
	}

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	ret = select(maxfd + 1, &set, NULL, NULL, &tv);

	if(ret)
	{
		if(ggz_gamefd >= 0)
		{
			if(FD_ISSET(ggz_gamefd, &set)) (ggz_gamehandler)();
		}
		if(FD_ISSET(serverfd, &set)) ggz_handle_ggz();
	}
}

void ggzpassive_sethandler(GGZGameFunction func)
{
	ggz_gamehandler = func;
}

void ggzpassive_start()
{
	ggz_init();

	while (!ggz_done)
	{
		ggz_network();
	}

	ggz_finish();
	sleep(1);
}

