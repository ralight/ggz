/*
 * File: display.h
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers Client
 * Date: 01/01/2001
 * Desc: Game display support structures and functions
 *
 * Copyright (C) 2001 Richard Gade.
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

extern GtkWidget *dlg_main;

extern int display_init(void);
void display_resized(void);
extern void display_handle_expose_event(GdkEventExpose * event);
extern void display_refresh_board(void);
extern void display_handle_click_event(GdkEventButton * event);
extern void display_statusbar(char *);
extern void display_show_path(GSList *);
extern void display_set_name(int, char *);
extern void display_set_label_colors(void);
