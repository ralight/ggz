/*
 * File: loop.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/26/00
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
#include "input.h"
#include "output.h"
#include "server.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>

/* FIXME: make this a parameter ? */
#define TIMEOUT 1


struct _fd_info {
	unsigned int fd;
	unsigned char removed;
	callback read;
	callback destroy;
};


static void loop_process_remove(void);

static fd_set active_fd_set;
static struct _fd_info *fds;
static int num_fds, fd_max;
static unsigned char done;


void loop_init(void)
{
	FD_ZERO(&active_fd_set);
	if (fds)
		free(fds);
	num_fds = 0;
	fd_max = 0;
	done = 0;
}


void loop_add_fd(unsigned int fd, callback read, callback destroy)
{
	if (!(fds = realloc(fds, (num_fds + 1) * sizeof(struct _fd_info))))
		ggzcore_error_sys_exit("realloc failed in loop_init");
	
	fds[num_fds].fd = fd;
	fds[num_fds].removed = 0;
	fds[num_fds].read = read;
	fds[num_fds].destroy = destroy;

	num_fds++;
	
	FD_SET(fd, &active_fd_set);
	if (fd > fd_max)
		fd_max = fd;
}


void loop_remove_fd(unsigned int fd)
{
	int i;

	for (i = 0; i < num_fds; i++) 
		if (fds[i].fd == fd) {
#if 0
			output_text("removing fd %d", fd);
#endif
			FD_CLR(fd, &active_fd_set);
			/* FIXME: for efficiency, we should recalc fd_max */
			fds[i].removed = 1;
			break;
		}
}


void loop(void)
{
	fd_set read_fd_set;
	int i, status;
	struct timeval tv;


	while (!done) {

		tv.tv_sec = TIMEOUT;
		tv.tv_usec = 0;
		read_fd_set = active_fd_set;

		status = select((fd_max+1), &read_fd_set, NULL, NULL, &tv);
		if (status > 0) {
			for (i = 0; i < num_fds; i++)
				if (FD_ISSET(fds[i].fd, &read_fd_set))
					fds[i].read();
		}
#if 0
		if (status == 0)
			output_text("timedout");

		if (status < 0)
			output_text("err");
#endif
		loop_process_remove();

		/* FIXME: make this an "idle" type func? */
		output_status();
	}
}


void loop_quit(void)
{
	done = 1;
}


/* FIXME: come with with good algorithm for removing fds from array */
static void loop_process_remove(void)
{
	int i;
	
	for (i = 0; i < num_fds; i++)
		if (fds[i].removed && fds[i].destroy) {
			fds[i].destroy();
			fds[i].removed = 0;
		}
#if 0
	int cur = 0, next = 0;

	while (cur < num_fds) {

		if (!fds[cur].removed) {
			cur++;
			continue;
		}

		/* if we're here, the current node needs replaced */
#endif

}


