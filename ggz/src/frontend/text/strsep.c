/*
 * File: strsep.c
 * Author: Brent Hendricks, Jason Short
 * Project: GGZ
 * Date: 11/06/01
 * $Id: strsep.c 6696 2005-01-16 07:10:13Z jdorje $
 *
 * Local copy of strsep for systems that don't have it.  This file should
 * always be compiled (but only for executables, not for libraries - don't
 * use strsep in libraries), and requires that AC_CHECK_FUNCS([strsep]) be
 * in configure.ac.  We could use AC_REPLACE_FUNCS but that seems more
 * complicated.
 *
 * Copyright (C) 2001-2005 GGZ Development Team
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

#include <string.h>

#include "support.h"

#ifndef HAVE_STRSEP
char *strsep(char **stringp, const char *delim)
{
	char *end, *begin = *stringp;

	if (begin == NULL) return NULL;

	end = strpbrk(*stringp, delim);

	if (end) {
		*end++ = '\0';
		*stringp = end;
	} else {
		*stringp = NULL;
	}

	return begin;
}
#endif
