/* 
 * File: dlg_bid.c
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Creates the bid request Gtk fixed dialog
 * $Id: dlg_bid.c 2700 2001-11-08 21:20:22Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "common.h"

#include "dlg_bid.h"
#include "main.h"
#include "game.h"

#include "easysock.h"

static GtkWidget *window = NULL;	/* bid/option dialog window */

static int option_count;
static int *options_selected;

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

static void dlg_bid_delete(GtkWidget * widget, gpointer data)
{
	gtk_widget_show_all(window);
}


void dlg_option_checked(GtkWidget * widget, gpointer data)
{
	gint option = GPOINTER_TO_INT(data) >> 8;	/* uber-trickery */
	gint choice = GPOINTER_TO_INT(data) & 255;

	if (GTK_TOGGLE_BUTTON(widget)->active) {
		client_debug("Boolean option %d/%d selected.", option,
			     choice);
		options_selected[option] = choice;
	} else {
		client_debug("Boolean option %d/%d deselected.", option,
			     choice);
		options_selected[option] = 0;
	}
}

void dlg_option_toggled(GtkWidget * widget, gpointer data)
{
	gint option = GPOINTER_TO_INT(data) >> 8;
	gint choice = GPOINTER_TO_INT(data) & 255;

	if (GTK_TOGGLE_BUTTON(widget)->active) {
		client_debug("Multiple-choice option %d/%d selected.", option,
			     choice);
		options_selected[option] = choice;
	} else
		client_debug("Multiple-choice option %d/%d deselected.",
			     option, choice);
}

void dlg_options_submit(GtkWidget * widget, gpointer data)
{
	client_send_options(option_count, options_selected);

	statusbar_message(_("Sending options to server"));

	/* options_selected was allocated in table_get_options */
	g_free(options_selected);
	options_selected = NULL;
	option_count = 0;

	gtk_widget_hide(window);
	window = NULL;
	/* TODO: destroy window ?? */
}


/* dlg_bid_display displays a popup window from which the user can choose
   their bid */
void dlg_bid_display(int possible_bids, char **bid_choices)
{
	/* JDS: I know very little of GTK; this is practically copied from
	   the tutorial */

	GtkWidget *table;
	GtkWidget *button;
	gint i;
	int xw, yw, leftover;

	yw = sqrt(possible_bids);
	xw = (possible_bids + yw - 1) / yw;	/* division, rounded up */
	leftover = xw * yw - possible_bids;	/* calculate the part of the
						   rectangle unused */

	window = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_title(GTK_WINDOW(window), _("Select your bid"));
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	table = gtk_table_new(yw, xw, FALSE);
	gtk_container_add(GTK_CONTAINER(window), table);

	for (i = 0; i < possible_bids; i++) {
		int x, y;
		button = gtk_button_new_with_label(bid_choices[i]);

		/* trickery - we don't pass a pointer to the data but the data itself */
		gtk_signal_connect(GTK_OBJECT(button), "clicked",
				   GTK_SIGNAL_FUNC(dlg_bid_clicked),
				   GINT_TO_POINTER(i));

		x = i % xw;
		y = i / xw;

		gtk_table_attach_defaults(GTK_TABLE(table), button, x, x + 1,
					  y, y + 1);
		gtk_widget_show(button);

	}

	/* If you close the window, it pops right back up again. */
	gtk_signal_connect_object(GTK_OBJECT(window), "delete_event",
				  GTK_SIGNAL_FUNC(dlg_bid_delete),
				  (gpointer) window);

	gtk_widget_show(table);
	gtk_widget_show(window);
}

static void dlg_option_display(int option_cnt, int *option_sizes,
			       char ***options)
{
	GtkWidget *box;
	GtkWidget *button;
	gint i, j;

	window = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_title(GTK_WINDOW(window), _("Select Options"));
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), box);

	for (i = 0; i < option_cnt; i++) {
		GtkWidget *subbox = NULL;
		if (option_sizes[i] == 1) {
			subbox = gtk_check_button_new_with_label(options[i]
								 [0]);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
						     (subbox),
						     options_selected[i]);
			gtk_signal_connect(GTK_OBJECT(subbox), "toggled", GTK_SIGNAL_FUNC(dlg_option_checked), GINT_TO_POINTER((i << 8) | 1)	/* super-hack 
																		 */ );
		} else {
			GSList *group = NULL;
			GtkWidget *active_radio = NULL;
			subbox = gtk_vbox_new(FALSE, 0);
			for (j = 0; j < option_sizes[i]; j++) {
				GtkWidget *radio;
				radio = gtk_radio_button_new_with_label(group,
									options
									[i]
									[j]);
				group = gtk_radio_button_group
					(GTK_RADIO_BUTTON(radio));
				gtk_signal_connect(GTK_OBJECT(radio),
						   "toggled",
						   GTK_SIGNAL_FUNC
						   (dlg_option_toggled),
						   GINT_TO_POINTER((i << 8) |
								   j));

				gtk_box_pack_start(GTK_BOX(subbox), radio,
						   FALSE, FALSE, 0);
				gtk_widget_show(radio);
				if (j == options_selected[i])
					active_radio = radio;
			}
			if (active_radio)
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
							     (active_radio),
							     TRUE);
		}
		gtk_box_pack_start(GTK_BOX(box), subbox, FALSE, FALSE, 0);
		gtk_widget_show(subbox);
	}

	button = gtk_button_new_with_label(_("Send options"));
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
			   GTK_SIGNAL_FUNC(dlg_options_submit), (gpointer) 0);

	/* If you close the window, it pops right back up again. */
	gtk_signal_connect_object(GTK_OBJECT(window), "delete_event",
				  GTK_SIGNAL_FUNC(dlg_bid_delete),
				  (gpointer) window);

	gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
	gtk_widget_show(button);

	gtk_widget_show(box);
	gtk_widget_show(window);
}

void table_get_options(int option_cnt, int *choice_cnt, int *defaults,
		       char ***option_choices)
{
	/* options_selected is freed in dlg_options_submit */
	option_count = option_cnt;
	options_selected = g_malloc(option_cnt * sizeof(*defaults));
	memcpy(options_selected, defaults, option_cnt * sizeof(*defaults));

	dlg_option_display(option_cnt, choice_cnt, option_choices);

	statusbar_message(_("Please select options."));
}
