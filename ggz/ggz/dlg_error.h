/*
 * File: dlg_error.h
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 3/24/99
 *
 * This fils contains functions for creating and handling the error box
 *
 * Copyright (C) 1998 Brent Hendricks.
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

void err_dlg(const gchar *, ...);
void warn_dlg(const gchar *, ...);
void msg_dlg(gchar *, gboolean);

