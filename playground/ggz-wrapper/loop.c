/*
 * File: loop.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/26/00
 * $Id: loop.c 6469 2004-12-14 18:16:16Z josef $
 *
 * Functions for handling main IO loop
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

#include "loop.h"
#include "server.h"

#include <ggz.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>


/* Information about a fd and what to do under various conditions */
struct _fd_info {
	unsigned int fd;
	unsigned char removed;
	callback read;
	callback write; 
	callback destroy;
};


/* Private variables and functions */
static fd_set active_fd_set;  
static struct _fd_info *fds;  
static unsigned int num_fds, fd_max;
static unsigned char done;
static int timeout;



void loop_init(int seconds)
{
	FD_ZERO(&active_fd_set);
	if (fds) {
		ggz_free(fds);
		fds = NULL;
	}
	num_fds = 0;
	fd_max = 0;
	done = 0;
	timeout = seconds;
}


void loop_add_fd(unsigned int fd, callback read, callback destroy)
{
	fds = ggz_realloc(fds, (num_fds + 1) * sizeof(struct _fd_info));
	
	fds[num_fds].fd = fd;
	fds[num_fds].removed = 0;
	fds[num_fds].read = read;
	fds[num_fds].write = NULL;
	fds[num_fds].destroy = destroy;

	num_fds++;
	
	FD_SET(fd, &active_fd_set);
	if (fd > fd_max)
		fd_max = fd;
}

void loop(void)
{
	fd_set read_fd_set;
	unsigned int i;
	int status;
	struct timeval tv;


	while (!done) {

		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		read_fd_set = active_fd_set;

		status = select((fd_max+1), &read_fd_set, NULL, NULL, &tv);
		if (status > 0) {
			for (i = 0; i < num_fds; i++)
				if (FD_ISSET(fds[i].fd, &read_fd_set))
					if(fds[i].removed == 0) fds[i].read();
		}
		if (status == 0) {
			/* time-out occurred */
		}

		if (status < 0) {
			/* select() error occurred */
		}
		
	}

	if (num_fds > 0) {
		/* FIXME: should we close the FD's? */
		ggz_free(fds);
		num_fds = 0;
	}

}


void loop_quit(void)
{
	done = 1;
}
