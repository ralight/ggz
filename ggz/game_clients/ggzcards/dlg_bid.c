/* $Id: dlg_bid.c 2070 2001-07-23 00:03:11Z jdorje $ */
/*
 * File: dlg_bid.c
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Creates the bid request Gtk fixed dialog
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

#include "cb_bid.h"
#include "dlg_bid.h"
#include "main.h"
#include "game.h"

#include "easysock.h"

static GtkWidget *window = NULL; /* bid/option dialog window */

int option_cnt;
int* options_selected;

/* dlg_bid_clicked
 *   callback function for when a bid is clicked on
 *   submits the bid to the server via make_bid()
 */
static void dlg_bid_clicked( GtkWidget *widget,
               				gpointer   data )
{
	int bid = (int)data; /* trickery */
	game_send_bid( bid );

	gtk_widget_hide(window);
	window = NULL;
	/* TODO: destroy window ?? */
}


void dlg_option_checked (GtkWidget *widget, gpointer data)
{
	int option = (int)data >> 8; /* uber-trickery */
	int choice = (int)data & 255;

	if (GTK_TOGGLE_BUTTON (widget)->active) {
		client_debug("Boolean option %d/%d selected.", option, choice);
		options_selected[option] = choice;
	} else {
		client_debug("Boolean option %d/%d deselected.", option, choice);
		options_selected[option] = 0;
	}	
}

void dlg_option_toggled (GtkWidget *widget, gpointer data)
{
	int option = (int)data >> 8;
	int choice = (int)data & 255;

	if (GTK_TOGGLE_BUTTON (widget)->active) {
		client_debug("Multiple-choice option %d/%d selected.", option, choice);
		options_selected[option] = choice;
	} else
		client_debug("Multiple-choice option %d/%d deselected.", option, choice);
}

void dlg_options_submit (GtkWidget *widget, gpointer data)
{
	game_send_options( option_cnt, options_selected );
	g_free(options_selected);
	options_selected = NULL;
	option_cnt = 0;

	gtk_widget_hide (window);
	window = NULL;
	/* TODO: destroy window ?? */
}


/* dlg_bid_display
 *   displays a popup window from which the user can choose their bid
 */
void dlg_bid_display(int possible_bids, char** bid_choices)
{
	/* JDS: I know very little of GTK; this is practically copied from the tutorial */

	GtkWidget *table;
	GtkWidget *button;
	int i;
	int xw, yw, leftover;

	yw = sqrt(possible_bids);
	xw = (possible_bids+yw-1) / yw; /* division, rounded up */
	leftover = xw * yw - possible_bids; /* calculate the part of the rectangle unused */

	window = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_title (GTK_WINDOW (window), "Select your bid");
	gtk_container_set_border_width (GTK_CONTAINER (window), 10);

	table = gtk_table_new( yw, xw, FALSE );
 	gtk_container_add (GTK_CONTAINER (window), table);

	for(i = 0; i < possible_bids; i++) {
		int x, y;
		button = gtk_button_new_with_label (bid_choices[i]);
		gtk_signal_connect (GTK_OBJECT (button), "clicked",
				    GTK_SIGNAL_FUNC (dlg_bid_clicked),
				    (gpointer)i /* trickery - it's not a pointer to the data but the data itself */);

		x = i % xw;
		y = i / xw;

		gtk_table_attach_defaults( GTK_TABLE(table), button, x, x+1, y, y+1 );
		gtk_widget_show( button );
		
	}

	gtk_widget_show( table );
	gtk_widget_show( window );
}

static void dlg_option_display(int option_cnt, int* option_sizes, char*** options)
{
	GtkWidget *box;
	GtkWidget *button;
	int i, j;
	
	window = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_title (GTK_WINDOW (window), "Select Options");
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	box = gtk_vbox_new(FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), box);

	for (i = 0; i < option_cnt; i++) {
                GtkWidget *subbox = NULL;
		if (option_sizes[i] == 1) {
			subbox = gtk_check_button_new_with_label( options[i][0] );
			gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(subbox), options_selected[i]);
			gtk_signal_connect (GTK_OBJECT (subbox), "toggled",
					    GTK_SIGNAL_FUNC (dlg_option_checked),
					    (gpointer)( (i << 8) | 1 ) /* super-hack */ );			
		} else {
			GSList *group = NULL;	
			GtkWidget *active_radio = NULL;
			subbox = gtk_vbox_new(FALSE, 0);
			for (j = 0; j < option_sizes[i]; j++) {
				GtkWidget *radio;
				radio = gtk_radio_button_new_with_label(group, options[i][j]);
				group = gtk_radio_button_group( GTK_RADIO_BUTTON(radio));
				gtk_signal_connect (GTK_OBJECT(radio), "toggled",
						    GTK_SIGNAL_FUNC (dlg_option_toggled),
						    (gpointer)( (i << 8) | j ) );
	
				gtk_box_pack_start( GTK_BOX(subbox), radio, FALSE, FALSE, 0);
				gtk_widget_show(radio);
				if (j == options_selected[i]) active_radio = radio;	
			}
			if (active_radio) gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(active_radio), TRUE);
		}
		gtk_box_pack_start( GTK_BOX(box), subbox, FALSE, FALSE, 0);
		gtk_widget_show( subbox );
	}
	
	button = gtk_button_new_with_label( "Send options" );
	gtk_signal_connect( GTK_OBJECT(button), "clicked",
			    GTK_SIGNAL_FUNC(dlg_options_submit),
			    (gpointer)0 );
	gtk_box_pack_start( GTK_BOX(box), button, FALSE, FALSE, 0);
	gtk_widget_show( button );

	gtk_widget_show( box );
	gtk_widget_show( window );	
}

/* handle_bid_request
 *   handles a bid request from the server
 *   this is mostly gui-independent, and should probably go somewhere else
 */
int handle_bid_request(void)
{
	int i;
	int possible_bids;
	char** bid_choices;

	if (game.state == WH_STATE_BID)
		/* TODO: the new bid request should override the old one */
		return -1;
	set_game_state( WH_STATE_BID );

	if (es_read_int(ggzfd, &possible_bids) < 0)
		return -1;

	client_debug("     Handling bid request: %d possible bids.", possible_bids);
	bid_choices = (char**)g_malloc(possible_bids * sizeof(char*));

	for(i = 0; i < possible_bids; i++) {
		if (es_read_string_alloc(ggzfd, &bid_choices[i]) < 0) {
			client_debug("Error reading string %d.", i);
			return -1;
		}

	}

	dlg_bid_display(possible_bids, bid_choices);

	for(i = 0; i < possible_bids; i++)
		g_free(bid_choices[i]);
	g_free(bid_choices);

	statusbar_message( _("Your turn to bid") );

	return 0;
}


int handle_option_request(void)
{
	int i, j;
	char*** option_choices; /* ugly! */
	int* option_sizes;

	if (es_read_int(ggzfd, &option_cnt) < 0)
		return -1;
	client_debug("     Handling option request: %d possible options.", option_cnt);
	option_choices = (char***)g_malloc(option_cnt * sizeof(char**));
	option_sizes = (int*)g_malloc(option_cnt * sizeof(int));
	options_selected = (int*)g_malloc(option_cnt * sizeof(int));

	for (i = 0; i < option_cnt; i++) {
		if (es_read_int(ggzfd, &option_sizes[i]) < 0) return -1;
		if (es_read_int(ggzfd, &options_selected[i]) < 0) return -1; /* read the default */
		option_choices[i] = (char**)g_malloc(option_sizes[i] * sizeof(char*));
		for (j = 0; j < option_sizes[i]; j++)
			if (es_read_string_alloc(ggzfd, &option_choices[i][j]) < 0)
				return -1;
	}

	if (game.state == WH_STATE_OPTIONS) {
		client_debug("Received second option request.  Ignoring it.");
	} else {
		set_game_state( WH_STATE_OPTIONS );
		dlg_option_display(option_cnt, option_sizes, option_choices);
	}

	for(i = 0; i < option_cnt; i++) {
		for(j = 0; j < option_sizes[i]; j++)
			g_free(option_choices[i][j]);
		g_free(option_choices[i]);
	}
	g_free(option_choices);
	g_free(option_sizes);

	statusbar_message( _("Please select options.") );

	return 0;
}


