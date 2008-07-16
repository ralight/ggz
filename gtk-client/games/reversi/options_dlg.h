/*
 * File: options_dlg.c
 * Author: Ismael Orenstein
 * Project: GGZ Reversi game module
 * Date: 2000
 * Desc: Options dialog code
 * $Id: options_dlg.h 6293 2004-11-07 05:51:47Z jdorje $
 *
 * Copyright (C) 2000-2004 GGZ Development Team
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

GtkWidget *create_options_dialog(GdkColor *, GdkColor *);
GtkWidget *create_colorselectiondialog(GdkColor *);

void on_back_button_clicked(GtkButton * button, gpointer user_data);

void update_data(GtkWidget *, GdkColor *);
void prepare_data(GtkButton * button, gchar * data);

void on_last_button_clicked(GtkButton * button, gpointer user_data);
