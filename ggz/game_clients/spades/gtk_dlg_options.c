/*
 * File: gtk_dlg_option.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 5/11/99
 *
 * This file contains functions for creating and handling the 
 * options dialog box
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

#ifdef HAVE_CONFIG_H
#  include <config.h>	/* Site-specific config */
#endif

#include <gtk/gtk.h>
#include <gtk_dlg_options.h>
#include <gtk_connect.h>
#include <gtk_play.h>
#include <stdio.h>

#include <client.h>
#include <options.h>
#include <callbacks.h>

#include "ggzintl.h"

/* Global state of game variable */
extern gameState_t gameState;
extern playArea_t *playArea;
extern option_t options;

static GtkWidget *optionTabs;
static GtkWidget *connectButton;
static GtkWidget *window;

void OptionsDialog(GtkWidget * widget, gpointer data)
{

	static input_t inputs;
	GtkWidget *mainBox, *serverBox, *portBox, *buttonBox;
	GtkWidget *playerOpt, *gameOpt;
	/*GtkWidget *serverLabel, *portLabel, *tauntLabel; */
	GtkWidget *playerTable;
	GtkObject *minBid_adj, *endGame_adj;
	GtkWidget *nilBidsLabel, *endGameLabel, *minBidLabel;
	GtkWidget *playerTab, *gameTab;
	/*GtkWidget *progressBar; */
	GtkWidget *cancelButton;
	/*GtkWidget *menuItem; */
	GtkTooltips *tooltips;
	GtkWidget *name_label;

	tooltips = gtk_tooltips_new();

	/* Dialog window */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(window),
				     GTK_WINDOW(playArea->window));
	gtk_container_border_width(GTK_CONTAINER(window), 10);
	gtk_window_set_title(GTK_WINDOW(window), _("Options"));
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);

	g_signal_connect(GTK_OBJECT(window), "delete_event",
			 GTK_SIGNAL_FUNC(gtk_widget_destroy), NULL);
	g_signal_connect(GTK_OBJECT(window), "destroy",
			 GTK_SIGNAL_FUNC(gtk_widget_destroy), NULL);

#if 0
	/* Show main window when dialog closes (broken) */
	g_signal_connect_swapped(GTK_OBJECT(window), "delete_event",
				 GTK_SIGNAL_FUNC(gtk_widget_show_now),
				 GTK_OBJECT(playArea->window));
	g_signal_connect_swapped(GTK_OBJECT(window), "destroy",
				 GTK_SIGNAL_FUNC(gtk_widget_show_now),
				 GTK_OBJECT(playArea->window));
#endif


	/* Notebook */
	optionTabs = gtk_notebook_new();
	gtk_widget_show(optionTabs);

	playerTab = gtk_label_new(_("Player Options"));
	gameTab = gtk_label_new(_("Game Options"));

	gtk_widget_show(playerTab);
	gtk_widget_show(gameTab);


	/* Player options tab */
	name_label = gtk_label_new(gameState.userName);
	gtk_widget_show(name_label);

	inputs.playerTog1 = gtk_toggle_button_new_with_label(_("Human"));
	inputs.playerTog2 = gtk_toggle_button_new_with_label(_("Human"));
	inputs.playerTog3 = gtk_toggle_button_new_with_label(_("Human"));

	g_signal_connect(GTK_OBJECT(inputs.playerTog1), "toggled",
			 GTK_SIGNAL_FUNC(PlayerToggled),
			 GINT_TO_POINTER(1));
	g_signal_connect(GTK_OBJECT(inputs.playerTog2), "toggled",
			 GTK_SIGNAL_FUNC(PlayerToggled),
			 GINT_TO_POINTER(2));
	g_signal_connect(GTK_OBJECT(inputs.playerTog3), "toggled",
			 GTK_SIGNAL_FUNC(PlayerToggled),
			 GINT_TO_POINTER(3));

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(inputs.playerTog1),
				     (options.bitOpt & MSK_COMP_1));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(inputs.playerTog2),
				     (options.bitOpt & MSK_COMP_2));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(inputs.playerTog3),
				     (options.bitOpt & MSK_COMP_3));

	gtk_widget_show(inputs.playerTog1);
	gtk_widget_show(inputs.playerTog2);
	gtk_widget_show(inputs.playerTog3);

	gtk_tooltips_set_tip(tooltips, inputs.playerTog1,
			     _("Click to toggle Human/Computer player"),
			     NULL);
	gtk_tooltips_set_tip(tooltips, inputs.playerTog2,
			     _("Click to toggle Human/Computer player"),
			     NULL);
	gtk_tooltips_set_tip(tooltips, inputs.playerTog3,
			     _("Click to toggle Human/Computer player"),
			     NULL);


	/* Gameplay options tab */
	minBid_adj = gtk_adjustment_new(options.minBid, 0, 13, 1, 10, 10);
	endGame_adj = gtk_adjustment_new(options.endGame,
					 100, 1000, 50, 100, 50);

	inputs.minBid =
	    gtk_spin_button_new(GTK_ADJUSTMENT(minBid_adj), 1, 0);
	inputs.endGame =
	    gtk_spin_button_new(GTK_ADJUSTMENT(endGame_adj), 1, 0);

	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(inputs.minBid), TRUE);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(inputs.endGame), TRUE);

	gtk_widget_show(inputs.minBid);
	gtk_widget_show(inputs.endGame);

	inputs.nilBids =
	    gtk_toggle_button_new_with_label((options.bitOpt & MSK_NILS) ?
					     _("Yes") : _("No"));
	g_signal_connect(GTK_OBJECT(inputs.nilBids), "toggled",
			 GTK_SIGNAL_FUNC(NilToggled), NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(inputs.nilBids),
				     (options.bitOpt & MSK_NILS));

#if 0
	gtk_widget_show(inputs.nilBids);
#endif

	minBidLabel = gtk_label_new(_("Minimum Bid :"));
	endGameLabel = gtk_label_new(_("Game ends at :"));
	nilBidsLabel = gtk_label_new(_("Nil bids allowed :"));

	gtk_widget_show(minBidLabel);
	gtk_widget_show(endGameLabel);

#if 0
	gtk_widget_show(nilBidsLabel);
#endif

	/* Dialog buttons */
	connectButton = gtk_button_new_with_label(_("Start"));
	cancelButton = gtk_button_new_with_label(_("Cancel"));

	gtk_widget_show(connectButton);
	gtk_widget_show(cancelButton);

	g_signal_connect(GTK_OBJECT(connectButton), "clicked",
			 GTK_SIGNAL_FUNC(launch_game), &inputs);
	g_signal_connect_swapped(GTK_OBJECT(connectButton), "clicked",
				 GTK_SIGNAL_FUNC(gtk_widget_destroy),
				 GTK_OBJECT(window));

	g_signal_connect_swapped(GTK_OBJECT(cancelButton), "clicked",
				 GTK_SIGNAL_FUNC(gtk_widget_destroy),
				 GTK_OBJECT(window));

	/* And the layout... */
	mainBox = gtk_vbox_new(FALSE, 0);
	playerOpt = gtk_vbox_new(FALSE, 0);
	gameOpt = gtk_table_new(3, 2, FALSE);
	serverBox = gtk_hbox_new(FALSE, 0);
	portBox = gtk_hbox_new(FALSE, 0);
	playerTable = gtk_table_new(3, 3, FALSE);
	buttonBox = gtk_hbutton_box_new();

	gtk_container_border_width(GTK_CONTAINER(gameOpt), 10);
	gtk_container_border_width(GTK_CONTAINER(playerTable), 10);
	gtk_container_border_width(GTK_CONTAINER(buttonBox), 10);
	gtk_table_set_row_spacings(GTK_TABLE(playerTable), 10);
	gtk_table_set_col_spacings(GTK_TABLE(playerTable), 10);
	gtk_table_set_row_spacings(GTK_TABLE(gameOpt), 10);
	gtk_table_set_col_spacings(GTK_TABLE(gameOpt), 10);


	gtk_table_attach(GTK_TABLE(playerTable), name_label, 1, 2, 2, 3,
			 (GtkAttachOptions) 0, (GtkAttachOptions) GTK_FILL,
			 0, 0);
	gtk_table_attach(GTK_TABLE(playerTable), inputs.playerTog3, 2, 3,
			 1, 2, (GtkAttachOptions) GTK_FILL,
			 (GtkAttachOptions) GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(playerTable), inputs.playerTog2, 1, 2,
			 0, 1, (GtkAttachOptions) GTK_EXPAND,
			 (GtkAttachOptions) GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(playerTable), inputs.playerTog1, 0, 1,
			 1, 2, (GtkAttachOptions) GTK_FILL,
			 (GtkAttachOptions) GTK_EXPAND, 0, 0);

	gtk_box_pack_end(GTK_BOX(playerOpt), playerTable, TRUE, TRUE, 0);

	gtk_table_attach(GTK_TABLE(gameOpt), minBidLabel, 0, 1, 0, 1,
			 (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) 0,
			 0, 0);
	gtk_table_attach(GTK_TABLE(gameOpt), endGameLabel, 0, 1, 1, 2,
			 (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) 0,
			 0, 0);
	gtk_table_attach(GTK_TABLE(gameOpt), nilBidsLabel, 0, 1, 2, 3,
			 (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) 0,
			 0, 0);
	gtk_table_attach(GTK_TABLE(gameOpt), inputs.nilBids, 1, 2, 2, 3,
			 (GtkAttachOptions) 0, (GtkAttachOptions) 0, 0, 0);
	gtk_table_attach(GTK_TABLE(gameOpt), inputs.endGame, 1, 2, 1, 2,
			 (GtkAttachOptions) 0, (GtkAttachOptions) 0, 0, 0);
	gtk_table_attach(GTK_TABLE(gameOpt), inputs.minBid, 1, 2, 0, 1,
			 (GtkAttachOptions) 0, (GtkAttachOptions) 0, 0, 0);

	gtk_notebook_append_page(GTK_NOTEBOOK(optionTabs), playerOpt,
				 playerTab);
	gtk_notebook_append_page(GTK_NOTEBOOK(optionTabs), gameOpt,
				 gameTab);

	gtk_container_add(GTK_CONTAINER(buttonBox), connectButton);
	gtk_container_add(GTK_CONTAINER(buttonBox), cancelButton);

	gtk_box_pack_start(GTK_BOX(mainBox), optionTabs, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(mainBox), buttonBox, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(window), mainBox);

	GTK_WIDGET_SET_FLAGS(connectButton, GTK_CAN_DEFAULT);
	GTK_WIDGET_SET_FLAGS(cancelButton, GTK_CAN_DEFAULT);
	gtk_widget_grab_default(connectButton);

	gtk_widget_show(mainBox);
/*	gtk_widget_show(playerOpt); */
	gtk_widget_show(gameOpt);
	gtk_widget_show(serverBox);
	gtk_widget_show(portBox);
	gtk_widget_show(playerTable);
	gtk_widget_show(buttonBox);

	gtk_widget_show(window);

}


/*
 * Callback for "offline" toggle button.  Sets option bit and sensitivity of
 * all online-related widgets
 */
void OfflineToggled(GtkWidget * toggle, input_t * data)
{

	if (GTK_TOGGLE_BUTTON(toggle)->active) {
		options.bitOpt |= MSK_OFFLINE;
		gtk_widget_set_sensitive(data->serverEntry, FALSE);
		gtk_widget_set_sensitive(data->portEntry, FALSE);
		gtk_widget_set_sensitive(data->tauntEntry, FALSE);
		gtk_widget_set_sensitive(data->playerTog1, FALSE);
		gtk_widget_set_sensitive(data->playerTog2, FALSE);
		gtk_widget_set_sensitive(data->playerTog3, FALSE);
	} else {
		options.bitOpt &= ~MSK_OFFLINE;
		gtk_widget_set_sensitive(data->serverEntry, TRUE);
		gtk_widget_set_sensitive(data->portEntry, TRUE);
		gtk_widget_set_sensitive(data->tauntEntry, TRUE);
		gtk_widget_set_sensitive(data->playerTog1, TRUE);
		gtk_widget_set_sensitive(data->playerTog2, TRUE);
		gtk_widget_set_sensitive(data->playerTog3, TRUE);
	}
}


/*
 * Callback for player toggle buttons.  Sets option bit and relabels button.
 */
void PlayerToggled(GtkWidget * toggle, gpointer data)
{

	char mask = 0;

	switch (GPOINTER_TO_INT(data)) {
	case 1:
		mask = MSK_COMP_1;
		break;
	case 2:
		mask = MSK_COMP_2;
		break;
	case 3:
		mask = MSK_COMP_3;
		break;
	}

	if (GTK_TOGGLE_BUTTON(toggle)->active) {
		options.bitOpt |= mask;
		gtk_label_set_text(GTK_LABEL(GTK_BIN(toggle)->child),
				   _("Computer"));
	} else {
		options.bitOpt &= ~mask;
		gtk_label_set_text(GTK_LABEL(GTK_BIN(toggle)->child),
				   _("Human"));
	}
}


/*
 * Callback for nil toggle button.  Sets option bit and relabels button
 */
void NilToggled(GtkWidget * toggle, gpointer data)
{

	if (GTK_TOGGLE_BUTTON(toggle)->active) {
		options.bitOpt |= MSK_NILS;
		gtk_label_set_text(GTK_LABEL(GTK_BIN(toggle)->child),
				   _("Yes"));
	} else {
		options.bitOpt &= ~MSK_NILS;
		gtk_label_set_text(GTK_LABEL(GTK_BIN(toggle)->child),
				   _("No"));
	}

}



/**
 * Reset connect dialog
 */
void ConnectDialogReset(GtkWidget * tmp)
{

	/*gtk_widget_set_sensitive( optionTabs, TRUE );
	   gtk_widget_set_sensitive( connectButton, TRUE  );
	   gtk_widget_set_sensitive( applyButton, TRUE  );
	   gtk_progress_bar_update( GTK_PROGRESS_BAR(pBar ), 0  );
	   gtk_progress_set_format_string( GTK_PROGRESS(pBar ), "Disconnected" );
	 */
}


/**
 * Destroy the dialog, but don't call Disconnect()
 */
void SafeCloseDialog(void)
{

	/*gtk_signal_disconnect_by_func( GTK_OBJECT( window ),
	   GTK_SIGNAL_FUNC(Disconnect), NULL ); 
	   gtk_widget_destroy(window); */
}
