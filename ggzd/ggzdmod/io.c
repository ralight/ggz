/* 
 * File: io.c
 * Author: GGZ Dev Team
 * Project: ggzdmod
 * Date: 10/14/01
 * Desc: Functions for reading/writing messages from/to game modules
 * $Id: io.c 2754 2001-11-18 23:58:02Z bmh $
 *
 * This file contains the backend for the ggzdmod library.  This
 * library facilitates the communication between the GGZ server (ggzd)
 * and game servers.  This file provides backend code that can be
 * used at both ends.
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

#include <config.h>		/* site-specific config */
#include <easysock.h>
#include <ggz.h>

#include "ggzdmod.h"
#include "io.h"
#include "protocol.h"


/* Functions for sending IO messages */
int io_send_req_join(int fd, GGZSeat *seat)
{
	ggz_debug("GGZDMOD", "Sending seat data");
	if (es_write_int(fd, REQ_GAME_JOIN) < 0
	    || es_write_int(fd, seat->num) < 0
	    || es_write_string(fd, seat->name) < 0
	    || es_write_fd(fd, seat->fd) < 0)
		return -1;
	else
		return 0;
}


int io_send_req_leave(int fd, GGZSeat *seat)
{
	if (es_write_int(fd, REQ_GAME_LEAVE) < 0
	    || es_write_string(fd, seat->name) < 0)
		return -1;
	else
		return 0;
}
