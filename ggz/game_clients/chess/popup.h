/*
 * File: popup.h
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 09/17/2000
 * Desc: Chess client popup dialogs
 *
 * Copyright (C) 2001 Ismael Orenstein.
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

GtkWidget *create_draw_dialog(void);
GtkWidget *create_clock_dialog(void);
GtkWidget *create_promote_dialog(void);

/* Callbacks */

void clock_option_select(GtkButton * button, gpointer user_data);

void clock_option_cancel(GtkButton * button, gpointer user_data);
