/* 
 * File: dlg_bid.c
 * Author: Jason Short, Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Creates the bid request dialog
 * $Id: dlg_bid.c 6271 2004-11-05 20:48:41Z jdorje $
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

#include <gtk/gtk.h>
#include <math.h>
#include <string.h>

#include <ggz.h>

#include "ggzintl.h"

#include "client.h"

#include "dlg_bid.h"
#include "game.h"
#include "main.h"
#include "table.h"

static GtkWidget *window = NULL;

void dlg_bid_destroy(void)
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
void dlg_bid_display(int possible_bids,
		     char **bid_texts, char **bid_descriptions)
{
	GtkWidget *table_box, *label;
	gint i;
	int xw, yw, leftover;
	GtkTooltips *tooltips = gtk_tooltips_new();

	dlg_bid_destroy();

	/* Calculate the dimensions of the box of bids */
	xw = sqrt(possible_bids / 1.5);
	if (xw == 0)
		xw++;
	yw = (possible_bids + xw - 1) / xw;	/* division, rounded up */
	leftover = xw * yw - possible_bids;	/* calculate the part of the
						   rectangle unused */

	table_box = gtk_table_new(yw + 1, xw, FALSE);

	if (!BID_ON_TABLE) {
		/* Create a title label within the table itself. */
		/* We don't show this when bid_on_table because the gtk_label
		   won't be drawn properly on top of the table graphics.  It
		   also allows more bids to fit cleanly on the table.  The
		   implementation is a bit ugly, but it gives good results.
		   (Note that down below we adjust "y" to take this into
		   account.) */
		label = gtk_label_new(_("Select your bid:"));
		gtk_table_attach_defaults(GTK_TABLE(table_box), label,
					  0, xw, 0, 1);
	}

	for (i = 0; i < possible_bids; i++) {
		int x, y;
		GtkWidget *button = gtk_button_new_with_label(bid_texts[i]);

		/* trickery - we don't pass a pointer to the data but the data 
		   itself */
		g_signal_connect(button, "clicked",
				 GTK_SIGNAL_FUNC(dlg_bid_clicked),
				 GINT_TO_POINTER(i));

		x = i % xw;
		y = i / xw;

		if (!BID_ON_TABLE) {
			/* If we're not bid_on_table, we have an extra row at
			   the top.  See above. */
			y++;
		}

		gtk_table_attach_defaults(GTK_TABLE(table_box), button,
					  x, x + 1, y, y + 1);
		gtk_widget_show(button);

		if (bid_descriptions[i][0] != '\0'
		    && strcmp(bid_descriptions[i], bid_texts[i])
		    && preferences.bidding_tooltips) {
			gtk_tooltips_set_tip(tooltips, button,
					     bid_descriptions[i], NULL);
		}
	}

	if (BID_ON_TABLE) {
		int x, y, w, h;

		/* Just draw the table_box right on the table. */

		window = table_box;

		/* Is this the right layout? I doubt it, but I'm not sure how
		   to do better. */
		get_fulltable_dim(&x, &y, &w, &h);
		gtk_widget_set_size_request(window, w - 2 * XWIDTH,
					    h - 2 * XWIDTH);
		(void) gtk_fixed_put(GTK_FIXED(table), window,
				     x + XWIDTH, y + XWIDTH);

		/* This seems to be necessary... */
		table_show_table(0, 0, get_table_width(), get_table_height());
	} else {
		/* Create a dialog window to place the table_box in. */
		window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_transient_for(GTK_WINDOW(window),
					     GTK_WINDOW(dlg_main));
		gtk_window_set_title(GTK_WINDOW(window), _("Bid Selection"));
		gtk_container_set_border_width(GTK_CONTAINER(window), 10);
		gtk_container_add(GTK_CONTAINER(window), table_box);

		/* If you close the window, it pops right back up again. */
		g_signal_connect_swapped(window, "delete_event",
					 GTK_SIGNAL_FUNC(dlg_bid_delete),
					 window);
	}

	gtk_widget_show_all(window);
}
