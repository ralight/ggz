/* 
 * File: dlg_about.c
 * Author: GGZ Development Team
 * Project: GGZ GTK games
 * Date: 10/12/2002
 * Desc: Create the "About" Gtk dialog
 * $Id: dlg_about.h 6293 2004-11-07 05:51:47Z jdorje $
 *
 * Copyright (C) 2000-2002 Brent Hendricks.
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

/* Call this function to set the title and content for the
 * about dialog.  Make sure all of the values are translated. */
void init_dlg_about(const char *title,
		    const char *header, const char *about);

/* Call this function to show the about dialog. */
void create_or_raise_dlg_about(void);
