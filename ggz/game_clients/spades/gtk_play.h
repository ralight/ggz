/*
 * File: gtk_play.h
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 1/23/99
 *
 * This fils contains function declarations for creating and handling menus  
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#include<gtk/gtk.h>

typedef struct playArea_t {

	GtkWidget *window;
	GtkWidget *field;
	GtkWidget *hand;
	GdkPixmap *handBuf;
	GdkPixmap *cards;
#if 0 /* Card backs aren't used */
	GdkPixmap *cardbacks;
#endif
	GtkWidget *bidButton;
	GtkWidget *names[4];
	GtkWidget *table;
	GdkPixmap *tableBuf;
	GtkWidget *tallys[4];
	GtkWidget *bids[2];
	GtkWidget *scores[2];
	GtkWidget *teams[2];
	GtkItemFactory *menu;

} playArea_t;

extern playArea_t *playArea;

void CreatePlayArea(void);
