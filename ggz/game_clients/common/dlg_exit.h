/*
 * File: dlg_exit.h
 * Author: Brent Hendricks
 * Project: GGZ Gtk Games (taken from NetSpades)
 * Date: 1/29/99
 * $Id: dlg_exit.h 6293 2004-11-07 05:51:47Z jdorje $
 *
 * This fils contains functions for creating and handling the 
 * exit dialog box
 *
 * Copyright (C) 1998-2002 Brent Hendricks.
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

#include <gtk/gtk.h>

void ggz_show_exit_dialog(int can_return, GtkWidget * parent_window);
