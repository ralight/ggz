/*
 * File: gtk_dlg_stat.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 2/18/99
 *
 * This file contains functions for creating and handling the session 
 * statistics dialog box.  This file replaces gtk_dlg_query.c since its 
 * functionality was absorbed by the statistics box
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


#include <config.h>

#include <gtk/gtk.h>

#include <gtk_dlg_stat.h>
#include <gtk_connect.h>
#include <client.h>

#include "ggzintl.h"


/* Global state of game variable */
extern gameState_t gameState;

static void ReadQuery(GtkWidget *, gpointer);

void StatDialog(GtkWidget * widget, gpointer data)
{

	GtkWidget *window;
	GtkWidget *dialog_vbox1;
	GtkWidget *queryLabel;
	GtkWidget *statsLabel;
	GtkWidget *titleLabel;
	GtkWidget *dialog_action_area1;
	GtkWidget *buttonBox;
	GtkWidget *labelBox;
	GtkWidget *yesButton;
	GtkWidget *noButton;

	char *scores;
	int i, sc0, sc1;

	window = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(window), _("Session Statistics"));
	gtk_window_set_policy(GTK_WINDOW(window), FALSE, FALSE, FALSE);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);

	/* Label widgets */
	titleLabel = gtk_label_new(_("Game Statistics"));
	queryLabel = gtk_label_new(_("Would you like to play again?"));
	gtk_widget_show(titleLabel);
	gtk_widget_show(queryLabel);


	/* Button widgets */
	if (gameState.gameOver & GAME_QUERY) {
		yesButton = gtk_button_new_with_label(_("Yes"));
		noButton = gtk_button_new_with_label(_("No"));
		g_signal_connect(GTK_OBJECT(yesButton), "clicked",
				   GTK_SIGNAL_FUNC(ReadQuery),
				   GINT_TO_POINTER(1));

		g_signal_connect(GTK_OBJECT(noButton), "clicked",
				   GTK_SIGNAL_FUNC(ReadQuery),
				   GINT_TO_POINTER(0));
		g_signal_connect_swapped(GTK_OBJECT(noButton), "clicked",
					  GTK_SIGNAL_FUNC
					  (gtk_widget_destroy),
					  GTK_OBJECT(window));

		gtk_widget_show(noButton);

	} else {
		yesButton = gtk_button_new_with_label(_("OK"));
		noButton = NULL; /* avoid compiler warning */
	}
	g_signal_connect_swapped(GTK_OBJECT(yesButton), "clicked",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  GTK_OBJECT(window));
	gtk_widget_show(yesButton);


	/* And the layout... */
	labelBox = gtk_vbox_new(FALSE, 0);
	buttonBox = gtk_hbutton_box_new();
	dialog_vbox1 = GTK_DIALOG(window)->vbox;
	dialog_action_area1 = GTK_DIALOG(window)->action_area;

	gtk_container_set_border_width(GTK_CONTAINER(labelBox), 10);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area1),
				       10);

	gtk_box_pack_start(GTK_BOX(labelBox), titleLabel, FALSE, FALSE,
			   10);

	/* Loop through game scores */
	for (i = 0; i < gameState.gameCount; i++) {
		sc0 = gameState.record[2 * i + (gameState.playerId % 2)];
		sc1 =
		    gameState.record[2 * i +
				     ((gameState.playerId + 1) % 2)];
		scores =
		    g_strdup_printf(_("Game %d:  %4d  %4d  (%c)"), (i + 1),
				    sc0, sc1, (sc0 > sc1 ? 'W' : 'L'));
		statsLabel = gtk_label_new(scores);
		gtk_widget_show(statsLabel);
		gtk_label_set_justify(GTK_LABEL(statsLabel),
				      GTK_JUSTIFY_LEFT);
		gtk_box_pack_start(GTK_BOX(labelBox), statsLabel, FALSE,
				   FALSE, 0);
	}
	/* If no scores yet, show message as such */
	if (i == 0) {
		statsLabel = gtk_label_new(_("(No statistics yet)"));
		gtk_widget_show(statsLabel);
		gtk_box_pack_start(GTK_BOX(labelBox), statsLabel, FALSE,
				   FALSE, 0);
	}

	gtk_box_pack_start(GTK_BOX(dialog_action_area1), buttonBox, TRUE,
			   TRUE, 0);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), labelBox, TRUE, TRUE, 0);

	/* Pack buttons and handle defaulting */
	gtk_box_pack_start(GTK_BOX(buttonBox), yesButton, FALSE, FALSE, 0);
	GTK_WIDGET_SET_FLAGS(yesButton, GTK_CAN_DEFAULT);
	gtk_widget_grab_default(yesButton);

	if (gameState.gameOver & GAME_QUERY) {
		gtk_box_pack_start(GTK_BOX(labelBox), queryLabel, FALSE,
				   FALSE, 5);
		gtk_box_pack_start(GTK_BOX(buttonBox), noButton, FALSE,
				   FALSE, 0);
		GTK_WIDGET_SET_FLAGS(noButton, GTK_CAN_DEFAULT);
	}

	gtk_widget_show(labelBox);
	gtk_widget_show(buttonBox);
	gtk_widget_show(dialog_vbox1);
	gtk_widget_show(dialog_action_area1);

	gtk_widget_show(window);

}


static void ReadQuery(GtkWidget * widget, gpointer again)
{

	gameState.gameOver = GAME_OVER;
	SendQuery(GPOINTER_TO_INT(again));

}
