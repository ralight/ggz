/*
 * File: ggzintl.c
 * Author: GGZ Development Team
 * Project: GGZ Gtk Games
 * $Id: ggzintl.c 6293 2004-11-07 05:51:47Z jdorje $
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <ggz.h>

#include "ggzintl.h"

void ggz_intl_init(const char *modulename)
{
#ifdef ENABLE_NLS
	bindtextdomain(modulename, LOCALEDIR);
	textdomain(modulename);
	setlocale(LC_ALL, "");
	ggz_debug("main", _("nls enabled: %s"), modulename);
#endif
}
