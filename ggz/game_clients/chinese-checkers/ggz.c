/*
 * File: ggz.c
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers Client
 * Date: 01/01/2001
 * Desc: GGZ Connection Functions
 *
 * Copyright (C) 2000,2001 Brent Hendricks, Richard Gade.
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
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "ggz.h"
#include "game.h"


void ggz_connect(void)
{
	char fd_name[30];
	struct sockaddr_un addr;

	/* Connect to Unix domain socket */
	sprintf(fd_name, "/tmp/ccheckers.%d", getpid());

	if((game.fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0)
		exit(-1);

	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, fd_name);

	if(connect(game.fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		exit(-1);
}
