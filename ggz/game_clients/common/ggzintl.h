/*
 * File: ggzintl.h
 * Author: GGZ Development Team
 * Project: GGZ Gtk Games
 * $Id: ggzintl.h 6293 2004-11-07 05:51:47Z jdorje $
 *
 * This file contains common functions and data for i18n of the
 * gtk-games package.
 *
 * Copyright (C) 2002 GGZ Development Team.
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

#ifndef GGZ_INTL_H
#define GGZ_INTL_H

/* Useful functions to be used in projects with more than one binary/package. */
/* Call ggz_intl_init("foo") first, and do a "touch ../po/foo/de.po" or similar. */

#ifdef ENABLE_NLS
#  include <locale.h>
#  include <libintl.h>
#  define _(x) gettext(x)
#else
#  define _(x) x
#endif

#define N_(x) x

void ggz_intl_init(const char *modulename);

#endif /* GGZ_INTL_H */
