/*
 * File: support.h
 * Author: Jason Short
 * Project: GGZ TXT Client
 * $Id: support.h 6696 2005-01-16 07:10:13Z jdorje $
 *
 * Support code
 *
 * Copyright (C) 2005 GGZ Development Team
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

/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  ifdef HAVE_LIBINTL_H
#    include <libintl.h>
#  endif
#  ifdef gettext_noop
#    define N_(x) gettext_noop(x)
#  else
#    define N_(x) (x)
#  endif
#  define _(x) gettext(x)
#  define PL_(x, y, n) ngettext((x), (y), (n))
#else
#  define N_(x) (x)
#  define _(x) (x)
#  define PL_(x) (x)
#endif

#ifndef HAVE_STRSEP
char *strsep(char **stringp, const char *delim);
#endif
