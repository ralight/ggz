/*
 * File: tablelist.h
 * Author: GGZ Dev Team
 * Project: GGZ GTK Client
 * Date: 11/03/2002
 * $Id: tablelist.h 5198 2002-11-04 01:47:47Z jdorje $
 * 
 * List of tables in the current room
 * 
 * Copyright (C) 2000-2002 Justin Zaun.
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

/* Update the table list. */
void display_tables(void);

/* Clear the table list. */
void clear_tables(void);

/* Return the currently selected row, or -1. */
int get_selected_table_row(void);

/* Return the ID of the currently selected table, or -1. */
int get_selected_table_id(void);

/* Create the table list.  Pass it the main window. */
GtkWidget *create_table_list(GtkWidget * window);
