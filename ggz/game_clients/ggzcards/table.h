/* 
 * File: table.h
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Routines to handle the Gtk game table
 * $Id: table.h 5158 2002-11-03 03:07:10Z jdorje $
 *
 * Copyright (C) 2000-2002 Brent Hendricks.
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

#ifndef __TABLE_H__
#define __TABLE_H__

#include "gtk/gtk.h"

#define DBG_TABLE "table.h"

#ifdef GTK2
extern PangoFontDescription *fixed_font;
#else
extern GtkRcStyle *fixed_font_style;
#endif
extern GtkWidget *table;	/* widget containing the whole table */
extern GtkWidget *table_drawing_area;
extern GtkStyle *table_style;	/* Style for the table */
extern GdkPixmap *card_fronts[4];

void table_initialize(void);
void table_setup(void);

void table_show_player_list(void);
void table_hide_player_list(void);

/* Call this function before exiting, to clean up everything. */
void table_cleanup(void);

void table_handle_expose_event(GdkEventExpose *);

/* Check for a clicked card.  Return TRUE iff the event was handled. */
gboolean table_handle_cardclick_event(GdkEventButton *event);

/* Check for a clicked player.  Return TRUE iff the event was handled. */
gboolean table_handle_menuclick_event(GdkEventButton *event);

/* These functions draw cards in a player's hand. */
void table_display_hand(int p, int write_to_screen);

/* These 3 functions draw the cards that are on the table. */
void table_show_card(int player, card_t card, int write_to_screen);
void table_show_cards(int write_to_screen);

/* Redraw and show the whole table. */
void table_redraw(void);

/* Draw the given portion of the table onto the given pixmap. */
void table_draw_table(GdkPixmap * pixmap, int x, int y, int w, int h);

/* Draw the given portion of the table onto the screen. */
void table_show_table(int x, int y, int w, int h);

void table_set_name(int player, const char *name);
void table_set_player_message(int player, const char *name);

#endif /* __TABLE_H__ */
