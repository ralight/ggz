/* 
 * File: dlg_bid.c
 * Author: Jason Short, Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Creates the bid request dialog
 * $Id: dlg_bid.c 3431 2002-02-21 02:53:38Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <math.h>

#include <gtk/gtk.h>

#include <ggz.h>
#include "client.h"

#include "dlg_bid.h"
#include "game.h"
#include "main.h"
#include "table.h"

static GtkWidget *window = NULL;

static void destroy_bid_window(void)
{
	if (window != NULL) {
		gtk_object_destroy((GtkObject *) window);
		window = NULL;
	}
}

/* dlg_bid_clicked callback function for when a bid is clicked on submits the 
   bid to the server via make_bid() */
static void dlg_bid_clicked(GtkWidget * widget, gpointer data)
{
	gint bid = GPOINTER_TO_INT(data);	/* trickery */
	game_send_bid(bid);

	gtk_widget_hide(window);
	window = NULL;
	/* TODO: destroy window ?? */
}

static gint dlg_bid_delete(GtkWidget * widget, gpointer data)
{
	/* don't delete the window */
	return TRUE;
}

/* displays a popup window from which the user can choose their bid */
void dlg_bid_display(int possible_bids, char **bid_choices)
{
	GtkWidget *table_box, *label;
	gint i;
	int xw, yw, leftover;

	destroy_bid_window();

	xw = sqrt(possible_bids / 1.5);
	if (xw == 0)
		xw++;
	yw = (possible_bids + xw - 1) / xw;	/* division, rounded up */
	leftover = xw * yw - possible_bids;	/* calculate the part of the
						   rectangle unused */

	table_box = gtk_table_new(yw + 1, xw, FALSE);
	
	/* Create a title label within the table itself. */
	label = gtk_label_new(_("Select your bid:"));
	gtk_table_attach_defaults(GTK_TABLE(table_box), label,
	                          0, xw, 0, 1);
		
	for (i = 0; i < possible_bids; i++) {
		int x, y;
		GtkWidget *button = gtk_button_new_with_label(bid_choices[i]);

		/* trickery - we don't pass a pointer to the data but the
		   data itself */
		(void) gtk_signal_connect(GTK_OBJECT(button), "clicked",
					  GTK_SIGNAL_FUNC(dlg_bid_clicked),
					  GINT_TO_POINTER(i));

		x = i % xw;
		y = i / xw + 1;

		gtk_table_attach_defaults(GTK_TABLE(table_box), button,
		                          x, x + 1, y, y + 1);
		gtk_widget_show(button);

	}

	if (preferences.bid_on_table) {
		int x, y, w, h;
		
		/* Just draw the table_box right on the table. */
		
		window = table_box;
		
		/* Is this the right layout?  I doubt it, but I'm not sure
		   how to do better. */
		get_fulltable_dim(&x, &y, &w, &h);
		gtk_widget_set_usize(window, w - 2 * XWIDTH, h - 2 * XWIDTH);
		(void) gtk_fixed_put(GTK_FIXED(table), window,
		                     x + XWIDTH, y + XWIDTH);
		
		/* This seems to be necessary... */
		table_show_table(0, 0, get_table_width(), get_table_height());
	} else {
		/* Create a dialog window to place the table_box in. */
		window = gtk_window_new(GTK_WINDOW_DIALOG);
		gtk_window_set_title(GTK_WINDOW(window), _("Bid Selection"));
		gtk_container_set_border_width(GTK_CONTAINER(window), 10);
		gtk_container_add(GTK_CONTAINER(window), table_box);
		
		/* If you close the window, it pops right back up again. */
		(void) gtk_signal_connect_object(GTK_OBJECT(window),
			"delete_event", GTK_SIGNAL_FUNC(dlg_bid_delete),
			(gpointer) window);
	}


	gtk_widget_show_all(window);
}
