/*
 * File: callbacks.h
 * Author: Brent Hendricks
 * Project: NetSpades
 * $Id: callbacks.h 3629 2002-03-23 19:31:28Z jdorje $
 *
 * This file contains the functions which operate on the type card. See the
 * header file card.h for a better description of these functions.
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

#include "dlg_exit.h"

void InputOptions(GtkButton * button, gpointer user_data);

void StartSession(GtkButton * button, gpointer user_data);

void show_details(GtkButton * button, gpointer user_data);

void launch_game(GtkButton * button, gpointer user_data);

void join_game(GtkButton * button, gpointer user_data);

void get_game_options(GtkButton * button, gpointer user_data);

void cancel_details(GtkButton * button, gpointer user_data);

void logout(GtkMenuItem * menuitem, gpointer user_data);

void get_types(GtkMenuItem * menuitem, gpointer user_data);

void get_players(GtkMenuItem * menuitem, gpointer user_data);

void get_tables(GtkMenuItem * menuitem, gpointer user_data);

void ExitDialog(GtkWidget *widget, gpointer user_data);
