/*
 * File: ggzmod.c
 * Author: GGZ Development Team
 * Project: GGZMod library
 * Desc: GGZ game client module functions
 * $Id: ggzmod.c 2916 2001-12-17 07:25:59Z jdorje $
 *
 * Copyright (C) 2000 GGZ Development Team
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
# include <config.h>
#endif /* HAVE_CONFIG */

/* #include <sys/socket.h> */

#include "ggzmod.h"

int ggzmod_connect(void)
{
	return 3;
}

int ggzmod_disconnect(void)
{
	/* FIXME: we should close the connection(s).  This actually is
	   important since it will flush the buffer and make sure
	   everything went through.  --JDS */
	return 0;
}

int ggzmod_get_fd(void)
{
	return 3;
}

int ggzmod_get_udp_fd(void)
{
	/* udp socket not implemented */
	return -1;
}
