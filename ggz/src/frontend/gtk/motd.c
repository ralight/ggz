/*
 * File: motd.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: motd.c 6272 2004-11-05 21:19:52Z jdorje $
 *
 * Copyright (C) 2000 Justin Zaun.
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "chat.h"
#include "client.h"
#include "motd.h"
#include "support.h"
#include "ggz.h"

static GtkWidget *motd_dialog;
static GtkWidget* create_dlg_motd (void);


void motd_create_or_raise(void)
{
	GtkWidget *tmp;

        if (!motd_dialog) {
                motd_dialog = create_dlg_motd();

		tmp = lookup_widget(motd_dialog, "motd_text");
		gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tmp), GTK_WRAP_WORD);
                gtk_widget_show(motd_dialog);
        }
        else {
                gdk_window_show(motd_dialog->window);
                gdk_window_raise(motd_dialog->window);

		/* Clear out what is currently there */
		tmp = lookup_widget(motd_dialog, "motd_text");
		gtk_text_buffer_set_text(gtk_text_view_get_buffer(
						 GTK_TEXT_VIEW(tmp)),
					 "", -1);
        }

}


void motd_print_line(gchar *line)
{                                       
        gchar *out=NULL;
        gint lindex=0;
        gint oindex=0;                          
	gint outlen=0;
        GtkWidget *temp_widget;
        GdkColormap *cmap;
        GdkFont *fixed_font;
        gint color_index=0; /* Black */
        gint letter;
                        
        /* Make shure the motd window it there */
        if (motd_dialog == NULL)
                return;
                                
        cmap = gdk_colormap_get_system();
        if (!gdk_color_alloc(cmap, &colors[color_index])) {
                g_error("couldn't allocate color");
        }
                                        
        temp_widget = g_object_get_data(G_OBJECT(motd_dialog), "motd_text");

        fixed_font = gdk_font_load ("-misc-fixed-medium-r-normal--10-100-75-75-c-60-iso8859-1");
        while(line[lindex] != '\0')
        {
		if(oindex % 256 == 0) {
			outlen += 256;
			out = ggz_realloc(out, outlen);
		}

                if (line[lindex] == '%')
                {
                        lindex++;
                        if (line[lindex] == 'c')
                        {
                                lindex++;
                                letter = atoi(&line[lindex]);
                                if ((letter>=0) && (letter<=9))
                                {
                                        out[oindex]='\0';
					/* FIXME: handle colors */
					gtk_text_buffer_insert_at_cursor(gtk_text_view_get_buffer(GTK_TEXT_VIEW(temp_widget)), out, -1);
                                        color_index=atoi(&line[lindex]);
                                        cmap = gdk_colormap_get_system();
                                        if (!gdk_color_alloc(cmap, &colors[color_index])) {
                                                g_error("couldn't allocate color");
                                        }
                                        oindex=0;
					outlen=0;
                                        lindex++;
                                }else {
                                        lindex--;
                                        lindex--;
                                }
                        }else{
                                lindex--;
                        }
                }
                out[oindex]=line[lindex];
                lindex++;
                oindex++;
        }
	if(oindex % 256 == 0) {
		outlen += 1;
		out = ggz_realloc(out, outlen);
	}
        out[oindex]='\0';

	/* FIXME: handle colors */
	gtk_text_buffer_insert_at_cursor(gtk_text_view_get_buffer(
						 GTK_TEXT_VIEW(temp_widget)),
					 out, -1);

	ggz_free(out);
}


static GtkWidget*
create_dlg_motd (void)
{
  GtkWidget *dlg_motd;
  GtkWidget *dialog_vbox1;
  GtkWidget *notd_vbox;
  GtkWidget *motd_scrolledwindow;
  GtkWidget *motd_text;
  GtkWidget *dialog_action_area1;
  GtkWidget *hbuttonbox1;
  GtkWidget *close_button;

  dlg_motd = gtk_dialog_new ();
  g_object_set_data(G_OBJECT (dlg_motd), "dlg_motd", dlg_motd);
  gtk_widget_set_usize (dlg_motd, 300, 455);
  gtk_window_set_title (GTK_WINDOW (dlg_motd), _("MOTD"));
  gtk_window_set_policy (GTK_WINDOW (dlg_motd), FALSE, TRUE, TRUE);

  dialog_vbox1 = GTK_DIALOG (dlg_motd)->vbox;
  g_object_set_data(G_OBJECT (dlg_motd), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);

  notd_vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (notd_vbox);
  g_object_set_data_full(G_OBJECT (dlg_motd), "notd_vbox", notd_vbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (notd_vbox);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), notd_vbox, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (notd_vbox), 5);

  motd_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (motd_scrolledwindow);
  g_object_set_data_full(G_OBJECT (dlg_motd), "motd_scrolledwindow", motd_scrolledwindow,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (motd_scrolledwindow);
  gtk_box_pack_start (GTK_BOX (notd_vbox), motd_scrolledwindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (motd_scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  motd_text = gtk_text_view_new_with_buffer(gtk_text_buffer_new(NULL));
  gtk_text_view_set_editable(GTK_TEXT_VIEW(motd_text), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(motd_text), FALSE);
  gtk_widget_ref (motd_text);
  g_object_set_data_full(G_OBJECT (dlg_motd), "motd_text", motd_text,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (motd_text);
  gtk_container_add (GTK_CONTAINER (motd_scrolledwindow), motd_text);

  dialog_action_area1 = GTK_DIALOG (dlg_motd)->action_area;
  g_object_set_data(G_OBJECT (dlg_motd), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  g_object_set_data_full(G_OBJECT (dlg_motd), "hbuttonbox1", hbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), hbuttonbox1, TRUE, TRUE, 0);

  close_button = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
  gtk_widget_ref (close_button);
  g_object_set_data_full(G_OBJECT (dlg_motd),
			    "close_button", close_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (close_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), close_button);
  GTK_WIDGET_SET_FLAGS (close_button, GTK_CAN_DEFAULT);

  g_signal_connect (GTK_OBJECT (dlg_motd), "destroy",
                      GTK_SIGNAL_FUNC (gtk_widget_destroyed),
                      &motd_dialog);
  g_signal_connect_swapped(GTK_OBJECT (close_button), "clicked",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy),
                             GTK_OBJECT (dlg_motd));

  gtk_widget_grab_focus (close_button);
  gtk_widget_grab_default (close_button);
  return dlg_motd;
}
