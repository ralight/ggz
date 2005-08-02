/* 
 * File: support.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 11/05/2004 (adapted from Glade-generated file)
 * Desc: Provides miscellaneous support code
 * $Id: support.h 6271 2004-11-05 20:48:41Z jdorje $
 *
 * Copyright (C) 2004 GGZ Development Team
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

/* 
 * Public Functions.
 */

/* 
 * This function returns a widget in a component created by Glade.
 * Call it with the toplevel widget in the component (i.e. a window/dialog),
 * or alternatively any widget in the component, and the name of the widget
 * you want returned.
 */
GtkWidget *lookup_widget(GtkWidget * widget, const gchar * widget_name);

/* get_widget() is deprecated. Use lookup_widget instead. */
#define get_widget lookup_widget
