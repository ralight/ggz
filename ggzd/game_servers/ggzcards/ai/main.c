/* 
 * File: ai/main.c
 * Author: Jason Short
 * Project: GGZCards AI Client
 * Date: 02/19/2002
 * Desc: AI client main loop and core logic
 * $Id: main.c 3567 2002-03-16 05:48:45Z jdorje $
 *
 * Copyright (C) 2000-2002 Brent Hendricks.
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

#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <ggz.h>		/* libggz */

#include "client.h"

#include "aicommon.h"

int main(int argc, char *argv[])
{
#ifdef DEBUG
	const char* debug_types[] = {DBG_BID, DBG_PLAY, DBG_AI, NULL};
#else
	const char* debug_types[] = {NULL};
#endif
	fd_set active_fd_set;
	int fd;
	
	ggz_debug_init(debug_types, NULL);

	/* Standard initializations. */
	fd = client_initialize();

	FD_ZERO(&active_fd_set);
	FD_SET(fd, &active_fd_set);
	
	while (1) {
		fd_set read_fd_set = active_fd_set;
		int status;
		
		status = select(fd + 1, &read_fd_set, NULL, NULL, NULL);
		
		if (status <= 0) {
			if (errno != EINTR)
				break;
			continue;
		}
		
		if (FD_ISSET(fd, &read_fd_set))
			if (client_handle_server() < 0)
				exit(0);		
	}

	client_quit();
	
	return 0;
}
