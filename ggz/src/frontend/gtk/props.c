/*
 * File: props.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 *
 * This is the main program body for the GGZ client
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

#include "chat.h"
#include "ggzcore.h"
#include "props.h"
#include "support.h"
#include "xtext.h"

static void props_update(void);
static void dlg_props_realize(GtkWidget *widget, gpointer user_data);
static void props_profile_box_realized(GtkWidget *widget, gpointer user_data);
static void props_profile_list_select(GtkCList *clist,
                               gint      row,
                               gint      column,
                               GdkEvent *event,
                               gpointer  user_data);
static void props_profile_entry_changed(GtkWidget *widget, gpointer user_data);
static void props_normal_toggled(GtkWidget *widget, gpointer user_data);
static void props_add_button_clicked(GtkWidget *widget, gpointer user_data);
static void props_modify_button_clicked(GtkWidget *widget, gpointer user_data);
static void props_delete_button_clicked(GtkWidget *widget, gpointer user_data);
static void props_Font_button_clicked(GtkWidget *widget, gpointer user_data);
static void props_color_toggled(GtkWidget *widget, gpointer user_data);
static void props_color_type_toggled(GtkWidget *widget, gpointer user_data);
static void props_ok_button_clicked(GtkWidget *widget, gpointer user_data);
static void props_apply_button_clicked(GtkWidget *widget, gpointer user_data);
static void props_cancel_button_clicked(GtkWidget *widget, gpointer user_data);
static void props_fok_button_clicked(GtkWidget *widget, gpointer user_data);
static void props_fcancel_button_clicked(GtkWidget *widget, gpointer user_data);
static void props_fapply_button_clicked(GtkWidget *widget, gpointer user_data);
static GtkWidget* create_dlg_props (void);
static GtkWidget* create_dlg_props_font (void);

extern GtkWidget *win_main;
GtkWidget *props_dialog;
GtkWidget *props_font_dialog;

void props_create_or_raise(void)
{
        if (!props_dialog) {
                props_dialog = create_dlg_props();
                gtk_widget_show(props_dialog);
        }
        else {
                gdk_window_show(props_dialog->window);
                gdk_window_raise(props_dialog->window);
        }
}
   
static void props_update(void)
{
	GtkWidget *tmp;
	GdkFont *font;

	/* Save Changes */

	/* Set XText font */
	tmp = lookup_widget((props_dialog), "chat_font");
	ggzcore_conf_write_string("CHAT", "FONT", gtk_entry_get_text(GTK_ENTRY(tmp)));

	/* Auto-Indent */
	tmp = lookup_widget((props_dialog), "indent_check");
	ggzcore_conf_write_int("CHAT", "AUTO_INDENT", GTK_TOGGLE_BUTTON(tmp)->active);

	/* Timestamp */
	tmp = lookup_widget((props_dialog), "timestamp_check");
	ggzcore_conf_write_int("CHAT", "TIMESTAMP", GTK_TOGGLE_BUTTON(tmp)->active);

	/* Word Wrap */
	tmp = lookup_widget((props_dialog), "wrap_check");
	ggzcore_conf_write_int("CHAT", "WORD_WRAP", GTK_TOGGLE_BUTTON(tmp)->active);

	/* Sound */
	tmp = lookup_widget((props_dialog), "sound_check");
	ggzcore_conf_write_int("CHAT", "SOUND", GTK_TOGGLE_BUTTON(tmp)->active);

	/* Join/Part Messages */
	tmp = lookup_widget((props_dialog), "ignore_check");
	ggzcore_conf_write_int("CHAT", "IGNORE", GTK_TOGGLE_BUTTON(tmp)->active);

	/* Color */
	tmp = lookup_widget((props_dialog), "color_check");
	ggzcore_conf_write_int("CHAT", "COLOR", GTK_TOGGLE_BUTTON(tmp)->active);

	/* Some Color */
	tmp = lookup_widget((props_dialog), "some_radio");
	ggzcore_conf_write_int("CHAT", "SOME_COLOR", GTK_TOGGLE_BUTTON(tmp)->active);

	/* FULL Color */
	tmp = lookup_widget((props_dialog), "full_radio");
	ggzcore_conf_write_int("CHAT", "FULL_COLOR", GTK_TOGGLE_BUTTON(tmp)->active);

	/* Your Color */
	tmp = lookup_widget((props_dialog), "y_spin");
	ggzcore_conf_write_int("CHAT", "Y_COLOR", gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(tmp)));

	/* Friend Color */
	tmp = lookup_widget((props_dialog), "f_spin");
	ggzcore_conf_write_int("CHAT", "F_COLOR", gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(tmp)));

	/* Other Color */
	tmp = lookup_widget((props_dialog), "o_spin");
	ggzcore_conf_write_int("CHAT", "O_COLOR", gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(tmp)));

	ggzcore_conf_commit();



	/* Activate Changes */

	/* Set XText font */
	tmp = lookup_widget((props_dialog), "chat_font");
	font = gdk_font_load(gtk_entry_get_text(GTK_ENTRY(tmp)));
	tmp = lookup_widget((win_main), "xtext_custom");
	gtk_xtext_set_font(GTK_XTEXT(tmp), font, 0);

	/* Auto-Indent */
	tmp = lookup_widget((props_dialog), "indent_check");
	if (GTK_TOGGLE_BUTTON(tmp)->active)
	{
		tmp = lookup_widget((win_main), "xtext_custom");
		GTK_XTEXT(tmp)->auto_indent = TRUE;
	}else{
		tmp = lookup_widget((win_main), "xtext_custom");
		GTK_XTEXT(tmp)->auto_indent = FALSE;
		GTK_XTEXT(tmp)->indent = 0;
	}

	/* Timestamp */
	tmp = lookup_widget((props_dialog), "timestamp_check");
	if (GTK_TOGGLE_BUTTON(tmp)->active)
	{
		tmp = lookup_widget((win_main), "xtext_custom");
		GTK_XTEXT(tmp)->time_stamp = TRUE;
		/* GTK_XTEXT(tmp)->indent = GTK_XTEXT(tmp)->indent + GTK_XTEXT(tmp)->stamp_width; */
	}else{
		tmp = lookup_widget((win_main), "xtext_custom");
		GTK_XTEXT(tmp)->time_stamp = FALSE;
		/* GTK_XTEXT(tmp)->indent = GTK_XTEXT(tmp)->indent - GTK_XTEXT(tmp)->stamp_width; */
	}

	/* Word Wrap */
	tmp = lookup_widget((props_dialog), "wrap_check");
	if (GTK_TOGGLE_BUTTON(tmp)->active)
	{
		tmp = lookup_widget((win_main), "xtext_custom");
		GTK_XTEXT(tmp)->wordwrap = TRUE;
	}else{
		tmp = lookup_widget((win_main), "xtext_custom");
		GTK_XTEXT(tmp)->wordwrap = FALSE;
	}

	/* Refresh XText to make changed take affect */
	tmp = lookup_widget((win_main), "xtext_custom");
	gtk_xtext_refresh(GTK_XTEXT(tmp), 0);

	/* Display a status Message */
	chat_display_message(CHAT_BEEP, "---", _("Properties Updated"));
}


void dlg_props_realize(GtkWidget *widget, gpointer user_data)
{
	GtkWidget *tmp;

	/* Set XText font */
	tmp = lookup_widget((props_dialog), "chat_font");
	gtk_entry_set_text(GTK_ENTRY(tmp), ggzcore_conf_read_string("CHAT", "FONT", "-*-fixed-medium-r-semicondensed--*-120-*-*-c-*-iso8859-1"));

	/* Auto-Indent */
	tmp = lookup_widget((props_dialog), "indent_check");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), ggzcore_conf_read_int("CHAT", "AUTO_INDENT", TRUE));

	/* Timestamp */
	tmp = lookup_widget((props_dialog), "timestamp_check");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), ggzcore_conf_read_int("CHAT", "TIMESTAMP", FALSE));

	/* Word Wrap */
	tmp = lookup_widget((props_dialog), "wrap_check");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), ggzcore_conf_read_int("CHAT", "WORD_WRAP", TRUE));

	/* Sound */
	tmp = lookup_widget((props_dialog), "sound_check");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), ggzcore_conf_read_int("CHAT", "SOUND", TRUE));

	/* Join/Part Messages */
	tmp = lookup_widget((props_dialog), "ignore_check");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), ggzcore_conf_read_int("CHAT", "IGNORE", FALSE));

	/* Color */
	tmp = lookup_widget((props_dialog), "color_check");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), ggzcore_conf_read_int("CHAT", "COLOR", TRUE));

	/* Some Color */
	tmp = lookup_widget((props_dialog), "some_radio");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), ggzcore_conf_read_int("CHAT", "SOME_COLOR", TRUE));

	/* FULL Color */
	tmp = lookup_widget((props_dialog), "full_radio");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), ggzcore_conf_read_int("CHAT", "FULL_COLOR", FALSE));

	/* Your Color */
	tmp = lookup_widget((props_dialog), "y_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(tmp), ggzcore_conf_read_int("CHAT", "Y_COLOR", 8));

	/* Friend Color */
	tmp = lookup_widget((props_dialog), "f_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(tmp), ggzcore_conf_read_int("CHAT", "F_COLOR", 0));

	/* Other Color */
	tmp = lookup_widget((props_dialog), "o_spin");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(tmp), ggzcore_conf_read_int("CHAT", "O_COLOR", 2));

}


void props_profile_box_realized(GtkWidget *widget, gpointer user_data)
{

}


void props_profile_list_select(GtkCList *clist,
                               gint      row,
                               gint      column,
                               GdkEvent *event,
                               gpointer  user_data)
{

}


void props_profile_entry_changed(GtkWidget *widget, gpointer user_data)
{

}


void props_normal_toggled(GtkWidget *widget, gpointer user_data)
{

}


void props_add_button_clicked(GtkWidget *widget, gpointer user_data)
{

}


void props_modify_button_clicked(GtkWidget *widget, gpointer user_data)
{

}


void props_delete_button_clicked(GtkWidget *widget, gpointer user_data)
{

}


void props_Font_button_clicked(GtkWidget *widget, gpointer user_data)
{
        /* If it already exists, bring it to the front */
        if (props_font_dialog) {
                gdk_window_show(props_font_dialog->window);
                gdk_window_raise(props_font_dialog->window);
        } else {
                props_font_dialog = create_dlg_props_font();
                gtk_widget_show(props_font_dialog);
        }
}


void props_color_toggled(GtkWidget *widget, gpointer user_data)
{

}


void props_color_type_toggled(GtkWidget *widget, gpointer user_data)
{

}


void props_ok_button_clicked(GtkWidget *widget, gpointer user_data)
{
	/* update the settings */
	props_update();

        /* Close font selector if open */
        if (props_font_dialog)
                gtk_widget_destroy(props_font_dialog);
	gtk_widget_destroy(props_dialog);
}


void props_apply_button_clicked(GtkWidget *widget, gpointer user_data)
{
	/* update the settings */
	props_update();
}


void props_cancel_button_clicked(GtkWidget *widget, gpointer user_data)
{
        /* Close font selector if open */
        if (props_font_dialog)
                gtk_widget_destroy(props_font_dialog);
	gtk_widget_destroy(props_dialog);
}


void props_fok_button_clicked(GtkWidget *widget, gpointer user_data)
{
	GtkWidget *tmp;

	/* Set font */
	tmp = lookup_widget(props_dialog, "chat_font");
	if (gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(props_font_dialog)) != NULL )
	{
		gtk_entry_set_text(GTK_ENTRY(tmp),
			gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(props_font_dialog)));
	}

	/* Close the font selector */
	gtk_widget_destroy(props_font_dialog);
}


void props_fcancel_button_clicked(GtkWidget *widget, gpointer user_data)
{
	/* Close the font selector */
	gtk_widget_destroy(props_font_dialog);
}


void props_fapply_button_clicked(GtkWidget *widget, gpointer user_data)
{
	GtkWidget *tmp;

	/* Set font */
	tmp = lookup_widget(props_dialog, "chat_font");
	if (gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(props_font_dialog)) != NULL )
	{
		gtk_entry_set_text(GTK_ENTRY(tmp),
			gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(props_font_dialog)));
	}
}

static GtkWidget*
create_dlg_props (void)
{
  GtkWidget *dlg_props;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *notebook;
  GtkWidget *props_profile_box;
  GtkWidget *scrolledwindow4;
  GtkWidget *profile_list;
  GtkWidget *label66;
  GtkWidget *edit_box;
  GtkWidget *data_box;
  GtkWidget *profile_box;
  GtkWidget *profile_label;
  GtkWidget *profile_entry;
  GtkWidget *server_box;
  GtkWidget *server_label;
  GtkWidget *server_entry;
  GtkWidget *port_label;
  GtkWidget *port_entry;
  GtkWidget *user_box;
  GtkWidget *user_data_box;
  GtkWidget *username_box;
  GtkWidget *username_label;
  GtkWidget *username_entry;
  GtkWidget *password_box;
  GtkWidget *password_label;
  GtkWidget *password_entry;
  GtkWidget *confirm_box;
  GtkWidget *confirm_label;
  GtkWidget *confirm_entry;
  GtkWidget *radio_box;
  GSList *login_type_group = NULL;
  GtkWidget *normal_radio;
  GtkWidget *guest_radio;
  GtkWidget *buttonbox;
  GtkWidget *add_button;
  GtkWidget *modify_button;
  GtkWidget *delete_button;
  GtkWidget *label1;
  GtkWidget *chat_box;
  GtkWidget *hbox8;
  GtkWidget *label12;
  GtkWidget *chat_font;
  GtkWidget *Font_button;
  GtkWidget *chat_table;
  GtkWidget *ignore_check;
  GtkWidget *wrap_check;
  GtkWidget *indent_check;
  GtkWidget *timestamp_check;
  GtkWidget *sound_check;
  GtkWidget *frame1;
  GtkWidget *vbox9;
  GtkWidget *color_check;
  GtkWidget *color_table;
  GSList *Chat_Color_group = NULL;
  GtkWidget *some_radio;
  GtkWidget *full_radio;
  GtkWidget *label26;
  GtkWidget *label25;
  GtkWidget *label24;
  GtkObject *f_spin_adj;
  GtkWidget *f_spin;
  GtkObject *o_spin_adj;
  GtkWidget *o_spin;
  GtkObject *y_spin_adj;
  GtkWidget *y_spin;
  GtkWidget *label27;
  GtkWidget *label2;
  GtkWidget *userinfo_box;
  GtkWidget *label16;
  GtkWidget *vbox7;
  GtkWidget *hbox12;
  GtkWidget *label17;
  GtkWidget *info_name;
  GtkWidget *vbox8;
  GtkWidget *hbox13;
  GtkWidget *label18;
  GtkWidget *info_city;
  GtkWidget *hbox14;
  GtkWidget *label19;
  GtkWidget *info_state;
  GtkWidget *hbox15;
  GtkWidget *label20;
  GtkWidget *info_country;
  GtkWidget *label21;
  GtkWidget *scrolledwindow1;
  GtkWidget *info_comments;
  GtkWidget *label3;
  GtkWidget *dialog_action_area1;
  GtkWidget *hbuttonbox1;
  GtkWidget *button1;
  GtkWidget *button2;
  GtkWidget *button3;

  dlg_props = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (dlg_props), "dlg_props", dlg_props);
  gtk_widget_set_usize (dlg_props, 550, 350);
  gtk_window_set_title (GTK_WINDOW (dlg_props), _("Properties"));
  gtk_window_set_policy (GTK_WINDOW (dlg_props), TRUE, TRUE, TRUE);

  dialog_vbox1 = GTK_DIALOG (dlg_props)->vbox;
  gtk_object_set_data (GTK_OBJECT (dlg_props), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox1), 6);

  notebook = gtk_notebook_new ();
  gtk_widget_ref (notebook);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "notebook", notebook,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (notebook);
  gtk_box_pack_start (GTK_BOX (vbox1), notebook, TRUE, TRUE, 0);

  props_profile_box = gtk_hbox_new (FALSE, 10);
  gtk_widget_ref (props_profile_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "props_profile_box", props_profile_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (props_profile_box);
  gtk_container_add (GTK_CONTAINER (notebook), props_profile_box);
  gtk_container_set_border_width (GTK_CONTAINER (props_profile_box), 10);

  scrolledwindow4 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow4);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "scrolledwindow4", scrolledwindow4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow4);
  gtk_box_pack_start (GTK_BOX (props_profile_box), scrolledwindow4, TRUE, TRUE, 0);
  gtk_widget_set_usize (scrolledwindow4, 175, -2);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  profile_list = gtk_clist_new (1);
  gtk_widget_ref (profile_list);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "profile_list", profile_list,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_list);
  gtk_container_add (GTK_CONTAINER (scrolledwindow4), profile_list);
  gtk_clist_set_column_width (GTK_CLIST (profile_list), 0, 80);
  gtk_clist_column_titles_show (GTK_CLIST (profile_list));

  label66 = gtk_label_new (_("Profiles"));
  gtk_widget_ref (label66);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label66", label66,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label66);
  gtk_clist_set_column_widget (GTK_CLIST (profile_list), 0, label66);

  edit_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (edit_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "edit_box", edit_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (edit_box);
  gtk_box_pack_start (GTK_BOX (props_profile_box), edit_box, TRUE, TRUE, 0);

  data_box = gtk_vbox_new (FALSE, 7);
  gtk_widget_ref (data_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "data_box", data_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (data_box);
  gtk_box_pack_start (GTK_BOX (edit_box), data_box, TRUE, FALSE, 0);

  profile_box = gtk_hbox_new (FALSE, 5);
  gtk_widget_ref (profile_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "profile_box", profile_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_box);
  gtk_box_pack_start (GTK_BOX (data_box), profile_box, FALSE, FALSE, 0);

  profile_label = gtk_label_new (_("Profile:"));
  gtk_widget_ref (profile_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "profile_label", profile_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_label);
  gtk_box_pack_start (GTK_BOX (profile_box), profile_label, FALSE, FALSE, 0);
  gtk_widget_set_usize (profile_label, 62, -2);
  gtk_label_set_justify (GTK_LABEL (profile_label), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (profile_label), 1, 0.5);

  profile_entry = gtk_entry_new_with_max_length (256);
  gtk_widget_ref (profile_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "profile_entry", profile_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_entry);
  gtk_box_pack_start (GTK_BOX (profile_box), profile_entry, TRUE, TRUE, 5);

  server_box = gtk_hbox_new (FALSE, 5);
  gtk_widget_ref (server_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "server_box", server_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (server_box);
  gtk_box_pack_start (GTK_BOX (data_box), server_box, FALSE, FALSE, 0);

  server_label = gtk_label_new (_("Server:"));
  gtk_widget_ref (server_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "server_label", server_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (server_label);
  gtk_box_pack_start (GTK_BOX (server_box), server_label, FALSE, FALSE, 0);
  gtk_widget_set_usize (server_label, 62, -2);
  gtk_label_set_justify (GTK_LABEL (server_label), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (server_label), 1, 0.5);

  server_entry = gtk_entry_new_with_max_length (256);
  gtk_widget_ref (server_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "server_entry", server_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (server_entry);
  gtk_box_pack_start (GTK_BOX (server_box), server_entry, TRUE, TRUE, 5);

  port_label = gtk_label_new (_("Port:"));
  gtk_widget_ref (port_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "port_label", port_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (port_label);
  gtk_box_pack_start (GTK_BOX (server_box), port_label, FALSE, TRUE, 2);
  gtk_misc_set_alignment (GTK_MISC (port_label), 1, 0.5);

  port_entry = gtk_entry_new_with_max_length (6);
  gtk_widget_ref (port_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "port_entry", port_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (port_entry);
  gtk_box_pack_start (GTK_BOX (server_box), port_entry, FALSE, TRUE, 5);
  gtk_widget_set_usize (port_entry, 50, -2);

  user_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (user_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "user_box", user_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (user_box);
  gtk_box_pack_start (GTK_BOX (data_box), user_box, FALSE, FALSE, 0);

  user_data_box = gtk_vbox_new (FALSE, 10);
  gtk_widget_ref (user_data_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "user_data_box", user_data_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (user_data_box);
  gtk_box_pack_start (GTK_BOX (user_box), user_data_box, FALSE, FALSE, 0);

  username_box = gtk_hbox_new (FALSE, 5);
  gtk_widget_ref (username_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "username_box", username_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (username_box);
  gtk_box_pack_start (GTK_BOX (user_data_box), username_box, TRUE, TRUE, 0);

  username_label = gtk_label_new (_("Username:"));
  gtk_widget_ref (username_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "username_label", username_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (username_label);
  gtk_box_pack_start (GTK_BOX (username_box), username_label, FALSE, FALSE, 0);
  gtk_widget_set_usize (username_label, 62, -2);
  gtk_label_set_justify (GTK_LABEL (username_label), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (username_label), 1, 0.5);

  username_entry = gtk_entry_new_with_max_length (16);
  gtk_widget_ref (username_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "username_entry", username_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (username_entry);
  gtk_box_pack_start (GTK_BOX (username_box), username_entry, FALSE, FALSE, 5);
  gtk_widget_set_usize (username_entry, 140, -2);

  password_box = gtk_hbox_new (FALSE, 5);
  gtk_widget_ref (password_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "password_box", password_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (password_box);
  gtk_box_pack_start (GTK_BOX (user_data_box), password_box, TRUE, TRUE, 0);

  password_label = gtk_label_new (_("Password:"));
  gtk_widget_ref (password_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "password_label", password_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (password_label);
  gtk_box_pack_start (GTK_BOX (password_box), password_label, FALSE, FALSE, 0);
  gtk_widget_set_usize (password_label, 62, -2);
  gtk_label_set_justify (GTK_LABEL (password_label), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (password_label), 1, 0.5);

  password_entry = gtk_entry_new ();
  gtk_widget_ref (password_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "password_entry", password_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (password_entry);
  gtk_box_pack_start (GTK_BOX (password_box), password_entry, FALSE, FALSE, 5);
  gtk_widget_set_usize (password_entry, 140, -2);
  gtk_entry_set_visibility (GTK_ENTRY (password_entry), FALSE);

  confirm_box = gtk_hbox_new (FALSE, 5);
  gtk_widget_ref (confirm_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "confirm_box", confirm_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (confirm_box);
  gtk_box_pack_start (GTK_BOX (user_data_box), confirm_box, TRUE, TRUE, 0);

  confirm_label = gtk_label_new (_("Confirm:"));
  gtk_widget_ref (confirm_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "confirm_label", confirm_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (confirm_label);
  gtk_box_pack_start (GTK_BOX (confirm_box), confirm_label, FALSE, FALSE, 0);
  gtk_widget_set_usize (confirm_label, 62, -2);
  gtk_label_set_justify (GTK_LABEL (confirm_label), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (confirm_label), 1, 0.5);

  confirm_entry = gtk_entry_new ();
  gtk_widget_ref (confirm_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "confirm_entry", confirm_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (confirm_entry);
  gtk_box_pack_start (GTK_BOX (confirm_box), confirm_entry, FALSE, FALSE, 5);
  gtk_widget_set_usize (confirm_entry, 140, -2);
  gtk_entry_set_visibility (GTK_ENTRY (confirm_entry), FALSE);

  radio_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (radio_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "radio_box", radio_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radio_box);
  gtk_box_pack_start (GTK_BOX (user_box), radio_box, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (radio_box), 15);

  normal_radio = gtk_radio_button_new_with_label (login_type_group, _("Normal Login"));
  login_type_group = gtk_radio_button_group (GTK_RADIO_BUTTON (normal_radio));
  gtk_widget_ref (normal_radio);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "normal_radio", normal_radio,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (normal_radio);
  gtk_box_pack_start (GTK_BOX (radio_box), normal_radio, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (normal_radio), TRUE);

  guest_radio = gtk_radio_button_new_with_label (login_type_group, _("Guest Login"));
  login_type_group = gtk_radio_button_group (GTK_RADIO_BUTTON (guest_radio));
  gtk_widget_ref (guest_radio);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "guest_radio", guest_radio,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (guest_radio);
  gtk_box_pack_start (GTK_BOX (radio_box), guest_radio, FALSE, FALSE, 0);

  buttonbox = gtk_hbutton_box_new ();
  gtk_widget_ref (buttonbox);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "buttonbox", buttonbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (buttonbox);
  gtk_box_pack_end (GTK_BOX (edit_box), buttonbox, FALSE, FALSE, 0);

  add_button = gtk_button_new_with_label (_("Add"));
  gtk_widget_ref (add_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "add_button", add_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (add_button);
  gtk_container_add (GTK_CONTAINER (buttonbox), add_button);
  GTK_WIDGET_SET_FLAGS (add_button, GTK_CAN_DEFAULT);

  modify_button = gtk_button_new_with_label (_("Modify"));
  gtk_widget_ref (modify_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "modify_button", modify_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (modify_button);
  gtk_container_add (GTK_CONTAINER (buttonbox), modify_button);
  GTK_WIDGET_SET_FLAGS (modify_button, GTK_CAN_DEFAULT);

  delete_button = gtk_button_new_with_label (_("Delete"));
  gtk_widget_ref (delete_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "delete_button", delete_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (delete_button);
  gtk_container_add (GTK_CONTAINER (buttonbox), delete_button);
  GTK_WIDGET_SET_FLAGS (delete_button, GTK_CAN_DEFAULT);

  label1 = gtk_label_new (_("Servers"));
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 0), label1);

  chat_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (chat_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "chat_box", chat_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_box);
  gtk_container_add (GTK_CONTAINER (notebook), chat_box);
  gtk_container_set_border_width (GTK_CONTAINER (chat_box), 5);

  hbox8 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox8);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "hbox8", hbox8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox8);
  gtk_box_pack_start (GTK_BOX (chat_box), hbox8, FALSE, TRUE, 0);

  label12 = gtk_label_new (_("Chat Font: "));
  gtk_widget_ref (label12);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label12", label12,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label12);
  gtk_box_pack_start (GTK_BOX (hbox8), label12, FALSE, FALSE, 0);

  chat_font = gtk_entry_new ();
  gtk_widget_ref (chat_font);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "chat_font", chat_font,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_font);
  gtk_box_pack_start (GTK_BOX (hbox8), chat_font, TRUE, TRUE, 5);
  gtk_entry_set_editable (GTK_ENTRY (chat_font), FALSE);

  Font_button = gtk_button_new_with_label (_("Change"));
  gtk_widget_ref (Font_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "Font_button", Font_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (Font_button);
  gtk_box_pack_start (GTK_BOX (hbox8), Font_button, FALSE, FALSE, 0);
  gtk_widget_set_usize (Font_button, 65, -2);

  chat_table = gtk_table_new (3, 2, TRUE);
  gtk_widget_ref (chat_table);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "chat_table", chat_table,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_table);
  gtk_box_pack_start (GTK_BOX (chat_box), chat_table, FALSE, FALSE, 4);

  ignore_check = gtk_check_button_new_with_label (_("Ignore Join/Part Messages"));
  gtk_widget_ref (ignore_check);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "ignore_check", ignore_check,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ignore_check);
  gtk_table_attach (GTK_TABLE (chat_table), ignore_check, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  wrap_check = gtk_check_button_new_with_label (_("Word Wrap"));
  gtk_widget_ref (wrap_check);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "wrap_check", wrap_check,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (wrap_check);
  gtk_table_attach (GTK_TABLE (chat_table), wrap_check, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  indent_check = gtk_check_button_new_with_label (_("Auto Indent"));
  gtk_widget_ref (indent_check);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "indent_check", indent_check,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (indent_check);
  gtk_table_attach (GTK_TABLE (chat_table), indent_check, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  timestamp_check = gtk_check_button_new_with_label (_("Timestamp Chats"));
  gtk_widget_ref (timestamp_check);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "timestamp_check", timestamp_check,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (timestamp_check);
  gtk_table_attach (GTK_TABLE (chat_table), timestamp_check, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  sound_check = gtk_check_button_new_with_label (_("Play Sounds"));
  gtk_widget_ref (sound_check);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "sound_check", sound_check,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (sound_check);
  gtk_table_attach (GTK_TABLE (chat_table), sound_check, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  frame1 = gtk_frame_new (_("Chat Color"));
  gtk_widget_ref (frame1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "frame1", frame1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (chat_box), frame1, TRUE, TRUE, 0);

  vbox9 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox9);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "vbox9", vbox9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox9);
  gtk_container_add (GTK_CONTAINER (frame1), vbox9);

  color_check = gtk_check_button_new_with_label (_("Color In Chat"));
  gtk_widget_ref (color_check);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "color_check", color_check,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (color_check);
  gtk_box_pack_start (GTK_BOX (vbox9), color_check, FALSE, FALSE, 0);

  color_table = gtk_table_new (3, 4, FALSE);
  gtk_widget_ref (color_table);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "color_table", color_table,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (color_table);
  gtk_box_pack_start (GTK_BOX (vbox9), color_table, TRUE, TRUE, 0);

  some_radio = gtk_radio_button_new_with_label (Chat_Color_group, _("Some"));
  Chat_Color_group = gtk_radio_button_group (GTK_RADIO_BUTTON (some_radio));
  gtk_widget_ref (some_radio);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "some_radio", some_radio,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (some_radio);
  gtk_table_attach (GTK_TABLE (color_table), some_radio, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 15, 0);

  full_radio = gtk_radio_button_new_with_label (Chat_Color_group, _("Full"));
  Chat_Color_group = gtk_radio_button_group (GTK_RADIO_BUTTON (full_radio));
  gtk_widget_ref (full_radio);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "full_radio", full_radio,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (full_radio);
  gtk_table_attach (GTK_TABLE (color_table), full_radio, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 15, 0);

  label26 = gtk_label_new (_("Other's Color"));
  gtk_widget_ref (label26);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label26", label26,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label26);
  gtk_table_attach (GTK_TABLE (color_table), label26, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 8, 0);
  gtk_label_set_justify (GTK_LABEL (label26), GTK_JUSTIFY_LEFT);
  gtk_label_set_line_wrap (GTK_LABEL (label26), TRUE);

  label25 = gtk_label_new (_("Friend's Color"));
  gtk_widget_ref (label25);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label25", label25,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label25);
  gtk_table_attach (GTK_TABLE (color_table), label25, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 7, 0);
  gtk_label_set_justify (GTK_LABEL (label25), GTK_JUSTIFY_LEFT);
  gtk_label_set_line_wrap (GTK_LABEL (label25), TRUE);

  label24 = gtk_label_new (_("Your Color"));
  gtk_widget_ref (label24);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label24", label24,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label24);
  gtk_table_attach (GTK_TABLE (color_table), label24, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 7, 0);
  gtk_label_set_justify (GTK_LABEL (label24), GTK_JUSTIFY_LEFT);
  gtk_label_set_line_wrap (GTK_LABEL (label24), TRUE);

  f_spin_adj = gtk_adjustment_new (1, 0, 15, 1, 1, 1);
  f_spin = gtk_spin_button_new (GTK_ADJUSTMENT (f_spin_adj), 1, 0);
  gtk_widget_ref (f_spin);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "f_spin", f_spin,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (f_spin);
  gtk_table_attach (GTK_TABLE (color_table), f_spin, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_usize (f_spin, 45, -2);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (f_spin), TRUE);

  o_spin_adj = gtk_adjustment_new (1, 0, 15, 1, 1, 1);
  o_spin = gtk_spin_button_new (GTK_ADJUSTMENT (o_spin_adj), 1, 0);
  gtk_widget_ref (o_spin);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "o_spin", o_spin,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (o_spin);
  gtk_table_attach (GTK_TABLE (color_table), o_spin, 2, 3, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_usize (o_spin, 45, -2);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (o_spin), TRUE);

  y_spin_adj = gtk_adjustment_new (1, 0, 15, 1, 1, 1);
  y_spin = gtk_spin_button_new (GTK_ADJUSTMENT (y_spin_adj), 1, 0);
  gtk_widget_ref (y_spin);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "y_spin", y_spin,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (y_spin);
  gtk_table_attach (GTK_TABLE (color_table), y_spin, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_usize (y_spin, 45, -2);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (y_spin), TRUE);

  label27 = gtk_label_new (_("  "));
  gtk_widget_ref (label27);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label27", label27,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label27);
  gtk_table_attach (GTK_TABLE (color_table), label27, 3, 4, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_usize (label27, 240, -2);

  label2 = gtk_label_new (_("Chat"));
  gtk_widget_ref (label2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 1), label2);

  userinfo_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (userinfo_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "userinfo_box", userinfo_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (userinfo_box);
  gtk_container_add (GTK_CONTAINER (notebook), userinfo_box);

  label16 = gtk_label_new (_("All of the following information is optional."));
  gtk_widget_ref (label16);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label16", label16,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label16);
  gtk_box_pack_start (GTK_BOX (userinfo_box), label16, FALSE, FALSE, 2);

  vbox7 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox7);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "vbox7", vbox7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox7);
  gtk_box_pack_start (GTK_BOX (userinfo_box), vbox7, TRUE, TRUE, 0);

  hbox12 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox12);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "hbox12", hbox12,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox12);
  gtk_box_pack_start (GTK_BOX (vbox7), hbox12, FALSE, TRUE, 0);

  label17 = gtk_label_new (_("Name:"));
  gtk_widget_ref (label17);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label17", label17,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label17);
  gtk_box_pack_start (GTK_BOX (hbox12), label17, FALSE, FALSE, 0);
  gtk_widget_set_usize (label17, 100, -2);
  gtk_label_set_justify (GTK_LABEL (label17), GTK_JUSTIFY_LEFT);
  gtk_label_set_line_wrap (GTK_LABEL (label17), TRUE);
  gtk_misc_set_padding (GTK_MISC (label17), 5, 0);

  info_name = gtk_entry_new ();
  gtk_widget_ref (info_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "info_name", info_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (info_name);
  gtk_box_pack_start (GTK_BOX (hbox12), info_name, TRUE, TRUE, 5);

  vbox8 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox8);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "vbox8", vbox8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox8);
  gtk_box_pack_start (GTK_BOX (vbox7), vbox8, FALSE, TRUE, 5);

  hbox13 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox13);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "hbox13", hbox13,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox13);
  gtk_box_pack_start (GTK_BOX (vbox8), hbox13, FALSE, TRUE, 0);

  label18 = gtk_label_new (_("City:"));
  gtk_widget_ref (label18);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label18", label18,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label18);
  gtk_box_pack_start (GTK_BOX (hbox13), label18, FALSE, FALSE, 0);
  gtk_widget_set_usize (label18, 100, -2);
  gtk_label_set_justify (GTK_LABEL (label18), GTK_JUSTIFY_LEFT);
  gtk_label_set_line_wrap (GTK_LABEL (label18), TRUE);
  gtk_misc_set_padding (GTK_MISC (label18), 5, 0);

  info_city = gtk_entry_new ();
  gtk_widget_ref (info_city);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "info_city", info_city,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (info_city);
  gtk_box_pack_start (GTK_BOX (hbox13), info_city, TRUE, TRUE, 5);

  hbox14 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox14);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "hbox14", hbox14,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox14);
  gtk_box_pack_start (GTK_BOX (vbox7), hbox14, FALSE, TRUE, 0);

  label19 = gtk_label_new (_("State:"));
  gtk_widget_ref (label19);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label19", label19,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label19);
  gtk_box_pack_start (GTK_BOX (hbox14), label19, FALSE, FALSE, 0);
  gtk_widget_set_usize (label19, 100, -2);
  gtk_label_set_justify (GTK_LABEL (label19), GTK_JUSTIFY_LEFT);
  gtk_label_set_line_wrap (GTK_LABEL (label19), TRUE);
  gtk_misc_set_padding (GTK_MISC (label19), 5, 0);

  info_state = gtk_entry_new ();
  gtk_widget_ref (info_state);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "info_state", info_state,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (info_state);
  gtk_box_pack_start (GTK_BOX (hbox14), info_state, TRUE, TRUE, 5);

  hbox15 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox15);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "hbox15", hbox15,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox15);
  gtk_box_pack_start (GTK_BOX (vbox7), hbox15, FALSE, TRUE, 5);

  label20 = gtk_label_new (_("Country:"));
  gtk_widget_ref (label20);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label20", label20,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label20);
  gtk_box_pack_start (GTK_BOX (hbox15), label20, FALSE, FALSE, 0);
  gtk_widget_set_usize (label20, 100, -2);
  gtk_label_set_justify (GTK_LABEL (label20), GTK_JUSTIFY_LEFT);
  gtk_label_set_line_wrap (GTK_LABEL (label20), TRUE);
  gtk_misc_set_padding (GTK_MISC (label20), 5, 0);

  info_country = gtk_entry_new ();
  gtk_widget_ref (info_country);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "info_country", info_country,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (info_country);
  gtk_box_pack_start (GTK_BOX (hbox15), info_country, TRUE, TRUE, 5);

  label21 = gtk_label_new (_("Comments, Hobies, Etc."));
  gtk_widget_ref (label21);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label21", label21,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label21);
  gtk_box_pack_start (GTK_BOX (vbox7), label21, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label21), GTK_JUSTIFY_LEFT);
  gtk_label_set_line_wrap (GTK_LABEL (label21), TRUE);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "scrolledwindow1", scrolledwindow1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (vbox7), scrolledwindow1, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow1), 5);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  info_comments = gtk_text_new (NULL, NULL);
  gtk_widget_ref (info_comments);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "info_comments", info_comments,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (info_comments);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), info_comments);
  gtk_text_set_editable (GTK_TEXT (info_comments), TRUE);

  label3 = gtk_label_new (_("User Information"));
  gtk_widget_ref (label3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label3", label3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label3);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 2), label3);

  dialog_action_area1 = GTK_DIALOG (dlg_props)->action_area;
  gtk_object_set_data (GTK_OBJECT (dlg_props), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "hbuttonbox1", hbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), hbuttonbox1, TRUE, TRUE, 0);

  button1 = gtk_button_new_with_label (_("OK"));
  gtk_widget_ref (button1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "button1", button1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button1);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
  GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

  button2 = gtk_button_new_with_label (_("Apply"));
  gtk_widget_ref (button2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "button2", button2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button2);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);
  GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

  button3 = gtk_button_new_with_label (_("Cancel"));
  gtk_widget_ref (button3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "button3", button3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button3);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button3);
  GTK_WIDGET_SET_FLAGS (button3, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (dlg_props), "realize",
                      GTK_SIGNAL_FUNC (dlg_props_realize),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (dlg_props), "destroy",
                      GTK_SIGNAL_FUNC (gtk_widget_destroyed),
                      &props_dialog);
  gtk_signal_connect (GTK_OBJECT (props_profile_box), "realize",
                      GTK_SIGNAL_FUNC (props_profile_box_realized),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (profile_list), "select_row",
                      GTK_SIGNAL_FUNC (props_profile_list_select),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (profile_entry), "changed",
                      GTK_SIGNAL_FUNC (props_profile_entry_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (normal_radio), "toggled",
                      GTK_SIGNAL_FUNC (props_normal_toggled),
                      dlg_props);
  gtk_signal_connect (GTK_OBJECT (add_button), "clicked",
                      GTK_SIGNAL_FUNC (props_add_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (modify_button), "clicked",
                      GTK_SIGNAL_FUNC (props_modify_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (delete_button), "clicked",
                      GTK_SIGNAL_FUNC (props_delete_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (Font_button), "clicked",
                      GTK_SIGNAL_FUNC (props_Font_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (color_check), "toggled",
                      GTK_SIGNAL_FUNC (props_color_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (some_radio), "toggled",
                      GTK_SIGNAL_FUNC (props_color_type_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (full_radio), "toggled",
                      GTK_SIGNAL_FUNC (props_color_type_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (button1), "clicked",
                      GTK_SIGNAL_FUNC (props_ok_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (button2), "clicked",
                      GTK_SIGNAL_FUNC (props_apply_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (button3), "clicked",
                      GTK_SIGNAL_FUNC (props_cancel_button_clicked),
                      NULL);

  return dlg_props;
}

static GtkWidget*
create_dlg_props_font (void)
{
  GtkWidget *dlg_props_font;
  GtkWidget *ok_button1;
  GtkWidget *cancel_button1;
  GtkWidget *apply_button1;

  dlg_props_font = gtk_font_selection_dialog_new (_("Select Font"));
  gtk_object_set_data (GTK_OBJECT (dlg_props_font), "dlg_props_font", dlg_props_font);
  gtk_container_set_border_width (GTK_CONTAINER (dlg_props_font), 4);
  gtk_window_set_policy (GTK_WINDOW (dlg_props_font), FALSE, TRUE, TRUE);

  ok_button1 = GTK_FONT_SELECTION_DIALOG (dlg_props_font)->ok_button;
  gtk_object_set_data (GTK_OBJECT (dlg_props_font), "ok_button1", ok_button1);
  gtk_widget_show (ok_button1);
  GTK_WIDGET_SET_FLAGS (ok_button1, GTK_CAN_DEFAULT);

  cancel_button1 = GTK_FONT_SELECTION_DIALOG (dlg_props_font)->cancel_button;
  gtk_object_set_data (GTK_OBJECT (dlg_props_font), "cancel_button1", cancel_button1);
  gtk_widget_show (cancel_button1);
  GTK_WIDGET_SET_FLAGS (cancel_button1, GTK_CAN_DEFAULT);

  apply_button1 = GTK_FONT_SELECTION_DIALOG (dlg_props_font)->apply_button;
  gtk_object_set_data (GTK_OBJECT (dlg_props_font), "apply_button1", apply_button1);
  gtk_widget_show (apply_button1);
  GTK_WIDGET_SET_FLAGS (apply_button1, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (dlg_props_font), "destroy",
                      GTK_SIGNAL_FUNC (gtk_widget_destroyed),
                      &props_font_dialog);
  gtk_signal_connect (GTK_OBJECT (ok_button1), "clicked",
                      GTK_SIGNAL_FUNC (props_fok_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cancel_button1), "clicked",
                      GTK_SIGNAL_FUNC (props_fcancel_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (apply_button1), "clicked",
                      GTK_SIGNAL_FUNC (props_fapply_button_clicked),
                      NULL);

  return dlg_props_font;
}



