/*
 * File: info.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: roominfo.c 6273 2004-11-05 21:49:00Z jdorje $
 *
 * This dialog is used to display information about a selected room to
 * the user. 
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
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <ggzcore.h>

#include "roominfo.h"
#include "support.h"


static GtkWidget *dialog;
static GtkWidget *create_dlg_info(void);


/* info_create_or_raise() - Displays the dialog or updates current dialog
 *                          with new room's information
 *
 * Recieves:
 * gint		room	: Room number to display info about
 *
 * Returns:
 */

void room_info_create_or_raise(GGZRoom * room)
{
	GtkWidget *tmp;
	GGZGameType *gt = ggzcore_room_get_gametype(room);
	const char * text;

	if (!dialog) {
		dialog = create_dlg_info();
		gtk_widget_show(dialog);
	} else {
		gdk_window_show(dialog->window);
		gdk_window_raise(dialog->window);
	}

	tmp = g_object_get_data(G_OBJECT(dialog), "name");
	if (gt)
		text = ggzcore_gametype_get_name(gt);
	else
		text = _("This room has no game");
	gtk_label_set_text(GTK_LABEL(tmp), text);

	tmp = g_object_get_data(G_OBJECT(dialog), "author");
	if (gt)
		text = ggzcore_gametype_get_author(gt);
	else
		text = _("N/A");
	gtk_label_set_text(GTK_LABEL(tmp), text);

	tmp = g_object_get_data(G_OBJECT(dialog), "www");
	if (gt)
		text = ggzcore_gametype_get_url(gt);
	else
		text = _("N/A");
	gtk_label_set_text(GTK_LABEL(tmp), text);

	tmp = g_object_get_data(G_OBJECT(dialog), "desc");
	text = ggzcore_room_get_name(room);
	if (!text)
		text = _("Unknown room");
	gtk_label_set_text(GTK_LABEL(tmp), text);
}



GtkWidget *create_dlg_info(void)
{
	GtkWidget *dlg_info;
	GtkWidget *dialog_vbox;
	GtkWidget *display_hbox;
	GtkWidget *game_pixmap;
	GtkWidget *info_vbox;
	GtkWidget *name_hbox;
	GtkWidget *label1;
	GtkWidget *name;
	GtkWidget *author_hbox;
	GtkWidget *label2;
	GtkWidget *author;
	GtkWidget *www_hbox;
	GtkWidget *label3;
	GtkWidget *www;
	GtkWidget *desc_hbox;
	GtkWidget *label4;
	GtkWidget *desc;
	GtkWidget *dialog_action_area1;
	GtkWidget *button_box;
	GtkWidget *ok_button;

	dlg_info = gtk_dialog_new();
	g_object_set_data(G_OBJECT(dlg_info), "dlg_info", dlg_info);
	gtk_window_set_title(GTK_WINDOW(dlg_info), _("Room Information"));

	dialog_vbox = GTK_DIALOG(dlg_info)->vbox;
	g_object_set_data(G_OBJECT(dlg_info), "dialog_vbox", dialog_vbox);
	gtk_widget_show(dialog_vbox);

	display_hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(display_hbox);
	g_object_set_data_full(G_OBJECT(dlg_info), "display_hbox",
				 display_hbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(display_hbox);
	gtk_box_pack_start(GTK_BOX(dialog_vbox), display_hbox, FALSE, TRUE,
			   0);
	gtk_container_set_border_width(GTK_CONTAINER(display_hbox), 5);

	game_pixmap = create_pixmap(dlg_info, NULL);
	gtk_widget_ref(game_pixmap);
	g_object_set_data_full(G_OBJECT(dlg_info), "game_pixmap",
				 game_pixmap,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(game_pixmap);
	gtk_box_pack_start(GTK_BOX(display_hbox), game_pixmap, FALSE, TRUE,
			   0);
	gtk_misc_set_alignment(GTK_MISC(game_pixmap), 0.5, 0);

	info_vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(info_vbox);
	g_object_set_data_full(G_OBJECT(dlg_info), "info_vbox", info_vbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(info_vbox);
	gtk_box_pack_start(GTK_BOX(display_hbox), info_vbox, TRUE, TRUE, 0);

	name_hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(name_hbox);
	g_object_set_data_full(G_OBJECT(dlg_info), "name_hbox", name_hbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(name_hbox);
	gtk_box_pack_start(GTK_BOX(info_vbox), name_hbox, TRUE, TRUE, 0);

	label1 = gtk_label_new(_("Game Name:"));
	gtk_widget_ref(label1);
	g_object_set_data_full(G_OBJECT(dlg_info), "label1", label1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label1);
	gtk_box_pack_start(GTK_BOX(name_hbox), label1, FALSE, FALSE, 0);
	gtk_misc_set_alignment(GTK_MISC(label1), 0.02, 0.5);

	name = gtk_label_new("");
	gtk_widget_ref(name);
	g_object_set_data_full(G_OBJECT(dlg_info), "name", name,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(name);
	gtk_box_pack_start(GTK_BOX(name_hbox), name, TRUE, TRUE, 0);
	gtk_misc_set_alignment(GTK_MISC(name), 0, 0.5);

	author_hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(author_hbox);
	g_object_set_data_full(G_OBJECT(dlg_info), "author_hbox",
				 author_hbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(author_hbox);
	gtk_box_pack_start(GTK_BOX(info_vbox), author_hbox, TRUE, TRUE, 0);

	label2 = gtk_label_new(_("Author:"));
	gtk_widget_ref(label2);
	g_object_set_data_full(G_OBJECT(dlg_info), "label2", label2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label2);
	gtk_box_pack_start(GTK_BOX(author_hbox), label2, FALSE, FALSE, 0);
	gtk_misc_set_alignment(GTK_MISC(label2), 0.02, 0.5);

	author = gtk_label_new("");
	gtk_widget_ref(author);
	g_object_set_data_full(G_OBJECT(dlg_info), "author", author,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(author);
	gtk_box_pack_start(GTK_BOX(author_hbox), author, TRUE, TRUE, 0);
	gtk_misc_set_alignment(GTK_MISC(author), 0, 0.5);

	www_hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(www_hbox);
	g_object_set_data_full(G_OBJECT(dlg_info), "www_hbox", www_hbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(www_hbox);
	gtk_box_pack_start(GTK_BOX(info_vbox), www_hbox, TRUE, TRUE, 0);

	label3 = gtk_label_new(_("Homepage:"));
	gtk_widget_ref(label3);
	g_object_set_data_full(G_OBJECT(dlg_info), "label3", label3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label3);
	gtk_box_pack_start(GTK_BOX(www_hbox), label3, FALSE, FALSE, 0);
	gtk_misc_set_alignment(GTK_MISC(label3), 0.02, 0.5);

	www = gtk_label_new("");
	gtk_widget_ref(www);
	g_object_set_data_full(G_OBJECT(dlg_info), "www", www,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(www);
	gtk_box_pack_start(GTK_BOX(www_hbox), www, TRUE, TRUE, 0);
	gtk_misc_set_alignment(GTK_MISC(www), 0, 0.5);

	desc_hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(desc_hbox);
	g_object_set_data_full(G_OBJECT(dlg_info), "desc_hbox", desc_hbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(desc_hbox);
	gtk_box_pack_start(GTK_BOX(info_vbox), desc_hbox, TRUE, TRUE, 0);

	label4 = gtk_label_new(_("Room Description:"));
	gtk_widget_ref(label4);
	g_object_set_data_full(G_OBJECT(dlg_info), "label4", label4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label4);
	gtk_box_pack_start(GTK_BOX(desc_hbox), label4, FALSE, FALSE, 0);
	gtk_misc_set_alignment(GTK_MISC(label4), 0.02, 0);

	desc = gtk_label_new("");
	gtk_widget_ref(desc);
	g_object_set_data_full(G_OBJECT(dlg_info), "desc", desc,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(desc);
	gtk_box_pack_start(GTK_BOX(desc_hbox), desc, TRUE, TRUE, 0);
	gtk_label_set_line_wrap(GTK_LABEL(desc), TRUE);
	gtk_misc_set_alignment(GTK_MISC(desc), 0, 0.5);

	dialog_action_area1 = GTK_DIALOG(dlg_info)->action_area;
	g_object_set_data(G_OBJECT(dlg_info), "dialog_action_area1",
			  dialog_action_area1);
	gtk_widget_show(dialog_action_area1);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area1),
				       10);

	button_box = gtk_hbutton_box_new();
	gtk_widget_ref(button_box);
	g_object_set_data_full(G_OBJECT(dlg_info), "button_box",
				 button_box,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_box);
	gtk_box_pack_start(GTK_BOX(dialog_action_area1), button_box, TRUE,
			   TRUE, 0);

	ok_button = gtk_button_new_with_label(_("OK"));
	gtk_widget_ref(ok_button);
	g_object_set_data_full(G_OBJECT(dlg_info), "ok_button", ok_button,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ok_button);
	gtk_container_add(GTK_CONTAINER(button_box), ok_button);
	GTK_WIDGET_SET_FLAGS(ok_button, GTK_CAN_DEFAULT);

	g_signal_connect(GTK_OBJECT(dlg_info), "destroy",
			   GTK_SIGNAL_FUNC(gtk_widget_destroyed), &dialog);
	g_signal_connect_swapped(GTK_OBJECT(ok_button), "clicked",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  GTK_OBJECT(dlg_info));

	return dlg_info;
}
