/* 
 * File: table.h
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Routines to handle the Gtk game table
 * $Id: table.h 3303 2002-02-10 12:00:00Z jdorje $
 *
 * Copyright (C) 2000 Brent Hendricks.
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

extern GtkRcStyle *fixed_font_style;
extern GtkWidget *table;	/* widget containing the whole table */
extern GtkStyle *table_style;	/* Style for the table */
extern GdkPixmap *card_fronts[4];

void table_initialize(void);
void table_setup(void);
void table_handle_expose_event(GdkEventExpose *);
void table_handle_click_event(GdkEventButton *);

/* These functions draw cards in a player's hand. */
void table_display_hand(int);
void table_display_all_hands(void);

/* These 3 functions draw the cards that are on the table. */
void table_show_card(int player, card_t card);
void table_show_cards(void);

void table_redraw(void);

void table_show_table(int x, int y, int w, int h);
void get_card_coordinates(card_t card, int orientation, int *x, int *y);
void draw_card(card_t card, int orientation, int x, int y, GdkPixmap * image);

void table_set_name(int player, const char *name);
void table_set_player_message(int player, const char *name);
