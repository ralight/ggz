/* 
 * File: dlg_options.c
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 12/09/2001
 * Desc: Creates the option request dialog
 * $Id: dlg_options.c 2940 2001-12-18 22:17:50Z jdorje $
 *
 * Copyright (C) 2001 GGZ Dev Team.
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

#include <assert.h>
#include <string.h>

#include <gtk/gtk.h>

#include <ggz.h>
#include "common.h"

#include "dlg_options.h"
#include "main.h"
#include "game.h"

static GtkWidget *window = NULL;

static int option_count;
static int *options_selected;

/* encode_option_selection and decode_option_selection do some hackish
   trickery so that we can encode the option selection within a gpointer to
   pass to the callback function. */
static gpointer encode_option_selection(gint option, gint choice)
{
	gint selection = (option << 8) | choice;
	assert(option < 256 && choice < 256);
	return GINT_TO_POINTER(selection);
}

static void decode_option_selection(gpointer selection, gint * option,
				    gint * choice)
{
	*option = GPOINTER_TO_INT(selection) >> 8;
	*choice = GPOINTER_TO_INT(selection) & 255;
}

static void destroy_options_selection(void)
{
	if (window != NULL) {
		gtk_object_destroy((GtkObject *) window);
		window = NULL;

		/* options_selected was allocated in dlg_option_display */
		g_free(options_selected);
		options_selected = NULL;
		option_count = 0;
	}
}

static void dlg_option_checked(GtkWidget * widget, gpointer data)
{
	gint option, choice;
	decode_option_selection(data, &option, &choice);

	if (GTK_TOGGLE_BUTTON(widget)->active) {
		ggz_debug("table", "Boolean option %d/%d selected.", option,
			  choice);
		options_selected[option] = choice;
	} else {
		ggz_debug("table", "Boolean option %d/%d deselected.", option,
			  choice);
		options_selected[option] = 0;
	}
}

static void dlg_option_toggled(GtkWidget * widget, gpointer data)
{
	gint option, choice;
	decode_option_selection(data, &option, &choice);

	if (GTK_TOGGLE_BUTTON(widget)->active) {
		ggz_debug("table", "Multiple-choice option %d/%d selected.",
			  option, choice);
		options_selected[option] = choice;
	} else
		ggz_debug("table", "Multiple-choice option %d/%d deselected.",
			  option, choice);
}

static gint dlg_opt_delete(GtkWidget * widget, gpointer data)
{
	/* don't delete the window */
	return TRUE;
}

static void dlg_options_submit(GtkWidget * widget, gpointer data)
{
	client_send_options(option_count, options_selected);

	statusbar_message(_("Sending options to server"));

	destroy_options_selection();
}

void dlg_option_display(int option_cnt, int *option_sizes, int *defaults,
			char ***options)
{
	GtkWidget *box;
	GtkWidget *button;
	gint i, j;

	/* options_selected is freed in destroy_options_selection */
	option_count = option_cnt;
	options_selected = g_malloc(option_cnt * sizeof(*defaults));
	memcpy(options_selected, defaults, option_cnt * sizeof(*defaults));

	window = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_title(GTK_WINDOW(window), _("Select Options"));
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), box);

	for (i = 0; i < option_cnt; i++) {
		GtkWidget *subbox = NULL;
		if (option_sizes[i] == 1) {
			gpointer user_data = encode_option_selection(i, 1);
			char *choice = options[i][0];
			subbox = gtk_check_button_new_with_label(choice);

			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
						     (subbox),
						     options_selected[i]);
			gtk_signal_connect(GTK_OBJECT(subbox), "toggled",
					   GTK_SIGNAL_FUNC
					   (dlg_option_checked), user_data);
		} else {
			GSList *group = NULL;
			GtkWidget *active_radio = NULL;
			subbox = gtk_vbox_new(FALSE, 0);
			assert(option_sizes[i] <= 255);
			for (j = 0; j < option_sizes[i]; j++) {
				GtkWidget *radio;
				gpointer user_data =
					encode_option_selection(i, j);
				char *choice = options[i][j];

				radio = gtk_radio_button_new_with_label(group,
									choice);
				group = gtk_radio_button_group
					(GTK_RADIO_BUTTON(radio));
				gtk_signal_connect(GTK_OBJECT(radio),
						   "toggled",
						   GTK_SIGNAL_FUNC
						   (dlg_option_toggled),
						   user_data);

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
				  GTK_SIGNAL_FUNC(dlg_opt_delete),
				  (gpointer) window);

	gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
	gtk_widget_show(button);

	gtk_widget_show(box);
	gtk_widget_show(window);
}
