/*
 * File: client.h
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: client.h 5197 2002-11-04 00:31:34Z jdorje $
 *
 * This is the main program body for the GGZ client
 *
 * Copyright (C) 2000 Justin Zaun.
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


void client_clear_tables(void);
void display_tables(void);
void display_players(void);
void client_join_table(void);

GtkWidget* create_win_main (void);
GtkWidget* create_mnu_table (void);

/* Default font */
#define DEFAULT_FONT "-*-fixed-medium-r-semicondensed--*-120-*-*-c-*-iso8859-1"

extern GtkWidget *win_main;
