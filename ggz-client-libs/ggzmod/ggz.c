/*	$Id: ggz.c 2174 2001-08-19 20:01:14Z jdorje $	*/
/*
 * Copyright (C) 2000 GGZ devel team
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
 *
 */
/*
 * @file ggz.c
 * Funciones de ggz para el cliente
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG */

#include <sys/socket.h>

/* The socket will always be FD 3. */
#define GGZ_SOCK_FD 3

int ggz_client_connect(void)
{
	/* TODO: make sure the socket is real */
	return GGZ_SOCK_FD;
}

int ggz_client_init(char *game_name)
{
	return 0;
}

int ggz_client_quit()
{
	return 0;
}

int ggz_client_get_sock()
{
	return GGZ_SOCK_FD;
}
