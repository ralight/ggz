/*
 * File: dlg_options.c
 * Author: GGZ Development Team
 * Project: GGZ Combat Client
 * Date: 2001?
 * Desc: Options dialog
 * $Id: dlg_options.h 6343 2004-11-13 01:44:11Z jdorje $
 *
 * Copyright (C) 2001-2004 GGZ Development Team
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

GtkWidget *create_dlg_options(int number);
GtkWidget *create_dlg_save(void);
GtkWidget *create_yes_no_dlg(char *text, GtkSignalFunc function,
			     gpointer user_data);
void dlg_options_update(GtkWidget *);
void cancel_button_clicked(GtkButton *, gpointer dialog);

void load_button_clicked(GtkButton *, gpointer dialog);
void save_button_clicked(GtkButton *, gpointer dialog);
void save_map(GtkButton *, GtkWidget *);
gboolean preview_expose(GtkWidget * widget, GdkEventExpose * event,
			gpointer);

gboolean
init_preview(GtkWidget * widget,
	     GdkEventConfigure * event, gpointer user_data);

gboolean draw_preview(GtkWidget * dlg_options);

void delete_button_clicked(GtkButton * button, gpointer dialog);

void delete_map(GtkButton * button, gpointer dialog);

void load_map(char *filename, GtkWidget * dialog);

void update_counters(GtkWidget *);

combat_game *quick_load_map_on_struct(char *filename);

gboolean mini_board_expose(GtkWidget * widget,
			   GdkEventExpose * event, gpointer user_data);

gboolean mini_board_configure(GtkWidget * widget,
			      GdkEventConfigure * event,
			      gpointer user_data);

gboolean mini_board_click(GtkWidget * widget,
			  GdkEventButton * event, gpointer user_data);

void init_mini_board(GtkWidget *);
void draw_mini_board(GtkWidget *);

void init_map_data(GtkWidget *);

#define OPEN T_OPEN
#define LAKE T_LAKE
#define BLACK T_NULL
#define PLAYER_1 OWNER(0) + T_OPEN
#define PLAYER_2 OWNER(1) + T_OPEN
