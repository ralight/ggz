/*
 * File: tls.c
 * Author: Rich Gade
 * Project: GGZ Core Client Lib
 * Date: 10/21/02
 *
 * Routines to enable easysock to utilize TLS using gnutls
 *
 * Copyright (C) 2002 Brent Hendricks.
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

#include "config.h"
#ifdef GGZ_TLS_NONE

#include "ggz.h"
#include <unistd.h>

/*****************************
 * Empty versions of functions
 * to prevent linkage errors
 *****************************/

void ggz_tls_init(const char *certfile, const char *keyfile, const char *password)
{
}

int ggz_tls_support_query(void)
{
	return 0;
}

int ggz_tls_enable_fd(int fdes, GGZTLSType whoami, GGZTLSVerificationType verify)
{
	return 0;
}

int ggz_tls_disable_fd(int fdes)
{
	return 0;
}

size_t ggz_tls_write(int fd, void *ptr, size_t n)
{
	return write(fd, ptr, n);
}

size_t ggz_tls_read(int fd, void *ptr, size_t n)
{
	return read(fd, ptr, n);
}

#endif

