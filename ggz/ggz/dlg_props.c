/*
 * File: dlg_props.c
 * Project: GGZ
 * Date: 04/22/2000
 *
 * Copyright (C) 1998 Justin Zaun.
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

#include "dlg_props.h"
#include "ggzrc.h"
#include "xtext.h"

/* Globals for the props dialog */
GtkWidget *dlg_props;
extern GtkWidget *main_win;

void props_add_button_clicked (GtkButton *button, gpointer user_data);
void props_modify_button_clicked (GtkButton *button, gpointer user_data);
void props_delete_button_clicked (GtkButton *button, gpointer user_data);
void props_Font_button_clicked (GtkButton *button, gpointer user_data);
void props_ok_button_clicked (GtkButton *button, gpointer user_data);
void props_apply_button_clicked (GtkButton *button, gpointer user_data);
void props_cancel_button_clicked (GtkButton *button, gpointer user_data);
void dlg_props_realize (GtkWidget *widget, gpointer user_data);
void props_update();


void props_update()
{
	GtkWidget *tmp;

	/* Servers Tab */

	/* Chat Tab */
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "sound_check");
	ggzrc_write_int("CHAT","Beep",GTK_TOGGLE_BUTTON(tmp)->active);
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "wrap_check");
	ggzrc_write_int("CHAT","WordWrap",GTK_TOGGLE_BUTTON(tmp)->active);
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "indent_check");
	ggzrc_write_int("CHAT","AutoIndent",GTK_TOGGLE_BUTTON(tmp)->active);
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "ignore_check");
	ggzrc_write_int("CHAT","IgnoreJoinPart",GTK_TOGGLE_BUTTON(tmp)->active);
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "timestamp_check");
	ggzrc_write_int("CHAT","Timestamp",GTK_TOGGLE_BUTTON(tmp)->active);

        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "chat_text");
	GTK_XTEXT(tmp)->auto_indent = ggzrc_read_int("CHAT","AutoIndent",TRUE);
	GTK_XTEXT(tmp)->wordwrap = ggzrc_read_int("CHAT","WordWrap",TRUE);
	GTK_XTEXT(tmp)->time_stamp = ggzrc_read_int("CHAT","Timestamp",FALSE);

	/*User Info Tab*/
}

void props_add_button_clicked (GtkButton *button, gpointer user_data)
{

}

void props_modify_button_clicked (GtkButton *button, gpointer user_data)
{

}

void props_delete_button_clicked (GtkButton *button, gpointer user_data)
{

}

void props_Font_button_clicked (GtkButton *button, gpointer user_data)
{

}

void props_ok_button_clicked (GtkButton *button, gpointer user_data)
{
	/* Save changes, and close the dialog */
	GtkWidget *tmp;
	props_update();

	/* Close the dialog */
        gtk_widget_destroy(dlg_props);
        dlg_props = NULL;
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "props_button");
        gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "properties");
        gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
}

void props_apply_button_clicked (GtkButton *button, gpointer user_data)
{
	/* Save changes, but don't close the dialog */
	props_update();
}

void props_cancel_button_clicked (GtkButton *button, gpointer user_data)
{
	/* Close dialog and don't save any changes */
	GtkWidget *tmp;

        gtk_widget_destroy(dlg_props);
        dlg_props = NULL;
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "props_button");
        gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "properties");
        gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
}

void dlg_props_realize (GtkWidget *widget, gpointer user_data)
{
	/* Set all options to current settings */
	GtkWidget *tmp;

	/* Servers Tab */

	/* Chat Tab */
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "sound_check");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), ggzrc_read_int("CHAT","Beep",TRUE));
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "wrap_check");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), ggzrc_read_int("CHAT","WordWrap",TRUE));
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "indent_check");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), ggzrc_read_int("CHAT","AutoIndent",TRUE));
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "ignore_check");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), ggzrc_read_int("CHAT","IgnoreJoinPart",FALSE));
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "timestamp_check");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), ggzrc_read_int("CHAT","Timestamp",FALSE));

	/*User Info Tab*/
}


GtkWidget*
create_dlg_props (void)
{
  GtkWidget *dlg_props;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *notebook1;
  GtkWidget *hbox2;
  GtkWidget *profile_list;
  GtkWidget *vbox2;
  GtkWidget *hbox3;
  GtkWidget *add_button;
  GtkWidget *modify_button;
  GtkWidget *delete_button;
  GtkWidget *hseparator1;
  GtkWidget *vbox3;
  GtkWidget *hbox4;
  GtkWidget *label7;
  GtkWidget *pro_name;
  GtkWidget *hbox5;
  GtkWidget *label8;
  GtkWidget *pro_server;
  GtkWidget *label9;
  GtkWidget *pro_port;
  GtkWidget *hbox6;
  GtkWidget *label10;
  GtkWidget *pro_username;
  GtkWidget *hbox7;
  GtkWidget *label11;
  GtkWidget *pro_password;
  GtkWidget *label1;
  GtkWidget *vbox4;
  GtkWidget *hbox8;
  GtkWidget *label12;
  GtkWidget *chat_font;
  GtkWidget *Font_button;
  GtkWidget *chat_table;
  GtkWidget *sound_check;
  GtkWidget *color_un_check;
  GtkWidget *color_chat_check;
  GtkWidget *timestamp_check;
  GtkWidget *ignore_check;
  GtkWidget *wrap_check;
  GtkWidget *indent_check;
  GtkWidget *label2;
  GtkWidget *vbox6;
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
  GtkWidget *inco_country;
  GtkWidget *label21;
  GtkWidget *scrolledwindow1;
  GtkWidget *info_comments;
  GtkWidget *label3;
  GtkWidget *dialog_action_area1;
  GtkWidget *hbox1;
  GtkWidget *ok_button;
  GtkWidget *apply_button;
  GtkWidget *cancel_button;

  dlg_props = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (dlg_props), "dlg_props", dlg_props);
  gtk_widget_set_usize (dlg_props, 500, 300);
  gtk_window_set_title (GTK_WINDOW (dlg_props), "Properties");
  gtk_window_set_policy (GTK_WINDOW (dlg_props), FALSE, FALSE, FALSE);

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

  notebook1 = gtk_notebook_new ();
  gtk_widget_ref (notebook1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "notebook1", notebook1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (notebook1);
  gtk_box_pack_start (GTK_BOX (vbox1), notebook1, TRUE, TRUE, 0);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "hbox2", hbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (notebook1), hbox2);
  gtk_container_set_border_width (GTK_CONTAINER (hbox2), 5);

  profile_list = gtk_list_new ();
  gtk_widget_ref (profile_list);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "profile_list", profile_list,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_list);
  gtk_box_pack_start (GTK_BOX (hbox2), profile_list, TRUE, TRUE, 5);
  gtk_widget_set_usize (profile_list, 25, -2);
  gtk_container_set_border_width (GTK_CONTAINER (profile_list), 10);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "vbox2", vbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (hbox2), vbox2, TRUE, TRUE, 0);
  gtk_widget_set_usize (vbox2, 162, -2);

  hbox3 = gtk_hbox_new (TRUE, 0);
  gtk_widget_ref (hbox3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "hbox3", hbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox3, FALSE, TRUE, 0);

  add_button = gtk_button_new_with_label ("Add");
  gtk_widget_ref (add_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "add_button", add_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (add_button);
  gtk_box_pack_start (GTK_BOX (hbox3), add_button, FALSE, FALSE, 0);
  gtk_widget_set_usize (add_button, 55, -2);

  modify_button = gtk_button_new_with_label ("Modify");
  gtk_widget_ref (modify_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "modify_button", modify_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (modify_button);
  gtk_box_pack_start (GTK_BOX (hbox3), modify_button, FALSE, FALSE, 0);
  gtk_widget_set_usize (modify_button, 55, -2);

  delete_button = gtk_button_new_with_label ("Delete");
  gtk_widget_ref (delete_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "delete_button", delete_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (delete_button);
  gtk_box_pack_start (GTK_BOX (hbox3), delete_button, FALSE, FALSE, 0);
  gtk_widget_set_usize (delete_button, 55, -2);

  hseparator1 = gtk_hseparator_new ();
  gtk_widget_ref (hseparator1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "hseparator1", hseparator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hseparator1);
  gtk_box_pack_start (GTK_BOX (vbox2), hseparator1, FALSE, TRUE, 5);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "vbox3", vbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox3);
  gtk_box_pack_start (GTK_BOX (vbox2), vbox3, TRUE, TRUE, 0);

  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox4);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "hbox4", hbox4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox4);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox4, TRUE, TRUE, 0);

  label7 = gtk_label_new ("Profile Name:  ");
  gtk_widget_ref (label7);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label7", label7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label7);
  gtk_box_pack_start (GTK_BOX (hbox4), label7, FALSE, FALSE, 0);

  pro_name = gtk_entry_new_with_max_length (20);
  gtk_widget_ref (pro_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "pro_name", pro_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pro_name);
  gtk_box_pack_start (GTK_BOX (hbox4), pro_name, TRUE, TRUE, 0);

  hbox5 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox5);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "hbox5", hbox5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox5);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox5, TRUE, TRUE, 0);

  label8 = gtk_label_new ("Server:  ");
  gtk_widget_ref (label8);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label8", label8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label8);
  gtk_box_pack_start (GTK_BOX (hbox5), label8, FALSE, FALSE, 0);

  pro_server = gtk_entry_new ();
  gtk_widget_ref (pro_server);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "pro_server", pro_server,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pro_server);
  gtk_box_pack_start (GTK_BOX (hbox5), pro_server, TRUE, TRUE, 0);
  gtk_widget_set_usize (pro_server, 221, -2);

  label9 = gtk_label_new (" Port:  ");
  gtk_widget_ref (label9);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label9", label9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label9);
  gtk_box_pack_start (GTK_BOX (hbox5), label9, FALSE, FALSE, 0);

  pro_port = gtk_entry_new_with_max_length (6);
  gtk_widget_ref (pro_port);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "pro_port", pro_port,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pro_port);
  gtk_box_pack_start (GTK_BOX (hbox5), pro_port, TRUE, TRUE, 0);

  hbox6 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox6);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "hbox6", hbox6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox6);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox6, TRUE, TRUE, 0);

  label10 = gtk_label_new ("Username:  ");
  gtk_widget_ref (label10);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label10", label10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label10);
  gtk_box_pack_start (GTK_BOX (hbox6), label10, FALSE, FALSE, 0);

  pro_username = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (pro_username);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "pro_username", pro_username,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pro_username);
  gtk_box_pack_start (GTK_BOX (hbox6), pro_username, TRUE, TRUE, 0);

  hbox7 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox7);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "hbox7", hbox7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox7);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox7, TRUE, TRUE, 0);

  label11 = gtk_label_new ("Password:  ");
  gtk_widget_ref (label11);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label11", label11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label11);
  gtk_box_pack_start (GTK_BOX (hbox7), label11, FALSE, FALSE, 0);

  pro_password = gtk_entry_new ();
  gtk_widget_ref (pro_password);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "pro_password", pro_password,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pro_password);
  gtk_box_pack_start (GTK_BOX (hbox7), pro_password, TRUE, TRUE, 0);

  label1 = gtk_label_new ("Servers");
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label1);

  vbox4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox4);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "vbox4", vbox4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox4);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox4);
  gtk_container_set_border_width (GTK_CONTAINER (vbox4), 5);

  hbox8 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox8);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "hbox8", hbox8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox8);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox8, FALSE, TRUE, 0);

  label12 = gtk_label_new ("Chat Font: ");
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

  Font_button = gtk_button_new_with_label ("Change");
  gtk_widget_ref (Font_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "Font_button", Font_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (Font_button);
  gtk_box_pack_start (GTK_BOX (hbox8), Font_button, FALSE, FALSE, 0);
  gtk_widget_set_usize (Font_button, 50, -2);

  chat_table = gtk_table_new (3, 3, FALSE);
  gtk_widget_ref (chat_table);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "chat_table", chat_table,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_table);
  gtk_box_pack_start (GTK_BOX (vbox4), chat_table, FALSE, FALSE, 4);

  sound_check = gtk_check_button_new_with_label ("Play Sounds");
  gtk_widget_ref (sound_check);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "sound_check", sound_check,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (sound_check);
  gtk_table_attach (GTK_TABLE (chat_table), sound_check, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  color_un_check = gtk_check_button_new_with_label ("Color Usernames");
  gtk_widget_ref (color_un_check);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "color_un_check", color_un_check,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (color_un_check);
  gtk_table_attach (GTK_TABLE (chat_table), color_un_check, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  color_chat_check = gtk_check_button_new_with_label ("Color In Chat");
  gtk_widget_ref (color_chat_check);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "color_chat_check", color_chat_check,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (color_chat_check);
  gtk_table_attach (GTK_TABLE (chat_table), color_chat_check, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  timestamp_check = gtk_check_button_new_with_label ("Timestamp Chats");
  gtk_widget_ref (timestamp_check);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "timestamp_check", timestamp_check,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (timestamp_check);
  gtk_table_attach (GTK_TABLE (chat_table), timestamp_check, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  ignore_check = gtk_check_button_new_with_label ("Ignore Join/Part Messages");
  gtk_widget_ref (ignore_check);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "ignore_check", ignore_check,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ignore_check);
  gtk_table_attach (GTK_TABLE (chat_table), ignore_check, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  wrap_check = gtk_check_button_new_with_label ("Word Wrap");
  gtk_widget_ref (wrap_check);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "wrap_check", wrap_check,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (wrap_check);
  gtk_table_attach (GTK_TABLE (chat_table), wrap_check, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  indent_check = gtk_check_button_new_with_label ("Auto Indent");
  gtk_widget_ref (indent_check);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "indent_check", indent_check,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (indent_check);
  gtk_table_attach (GTK_TABLE (chat_table), indent_check, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label2 = gtk_label_new ("Chat");
  gtk_widget_ref (label2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);

  vbox6 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox6);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "vbox6", vbox6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox6);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox6);

  label16 = gtk_label_new ("All of the following information is optional.");
  gtk_widget_ref (label16);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label16", label16,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label16);
  gtk_box_pack_start (GTK_BOX (vbox6), label16, FALSE, FALSE, 2);

  vbox7 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox7);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "vbox7", vbox7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox7);
  gtk_box_pack_start (GTK_BOX (vbox6), vbox7, TRUE, TRUE, 0);

  hbox12 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox12);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "hbox12", hbox12,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox12);
  gtk_box_pack_start (GTK_BOX (vbox7), hbox12, TRUE, TRUE, 0);

  label17 = gtk_label_new ("Name:");
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
  gtk_box_pack_start (GTK_BOX (vbox7), vbox8, TRUE, TRUE, 0);

  hbox13 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox13);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "hbox13", hbox13,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox13);
  gtk_box_pack_start (GTK_BOX (vbox8), hbox13, TRUE, TRUE, 0);

  label18 = gtk_label_new ("City:");
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
  gtk_box_pack_start (GTK_BOX (vbox7), hbox14, TRUE, TRUE, 0);

  label19 = gtk_label_new ("State:");
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
  gtk_box_pack_start (GTK_BOX (vbox7), hbox15, TRUE, TRUE, 0);

  label20 = gtk_label_new ("Country:");
  gtk_widget_ref (label20);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label20", label20,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label20);
  gtk_box_pack_start (GTK_BOX (hbox15), label20, FALSE, FALSE, 0);
  gtk_widget_set_usize (label20, 100, -2);
  gtk_label_set_justify (GTK_LABEL (label20), GTK_JUSTIFY_LEFT);
  gtk_label_set_line_wrap (GTK_LABEL (label20), TRUE);
  gtk_misc_set_padding (GTK_MISC (label20), 5, 0);

  inco_country = gtk_entry_new ();
  gtk_widget_ref (inco_country);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "inco_country", inco_country,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (inco_country);
  gtk_box_pack_start (GTK_BOX (hbox15), inco_country, TRUE, TRUE, 5);

  label21 = gtk_label_new ("Comments, Hobies, Etc.");
  gtk_widget_ref (label21);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label21", label21,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label21);
  gtk_box_pack_start (GTK_BOX (vbox7), label21, FALSE, FALSE, 3);
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

  label3 = gtk_label_new ("User Information");
  gtk_widget_ref (label3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label3", label3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label3);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label3);

  dialog_action_area1 = GTK_DIALOG (dlg_props)->action_area;
  gtk_object_set_data (GTK_OBJECT (dlg_props), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  hbox1 = gtk_hbox_new (TRUE, 0);
  gtk_widget_ref (hbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "hbox1", hbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), hbox1, TRUE, TRUE, 0);

  ok_button = gtk_button_new_with_label ("OK");
  gtk_widget_ref (ok_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "ok_button", ok_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ok_button);
  gtk_box_pack_start (GTK_BOX (hbox1), ok_button, FALSE, FALSE, 0);
  gtk_widget_set_usize (ok_button, 47, -2);

  apply_button = gtk_button_new_with_label ("Apply");
  gtk_widget_ref (apply_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "apply_button", apply_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (apply_button);
  gtk_box_pack_start (GTK_BOX (hbox1), apply_button, FALSE, FALSE, 0);
  gtk_widget_set_usize (apply_button, 47, -2);

  cancel_button = gtk_button_new_with_label ("Cancel");
  gtk_widget_ref (cancel_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "cancel_button", cancel_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cancel_button);
  gtk_box_pack_start (GTK_BOX (hbox1), cancel_button, FALSE, FALSE, 0);
  gtk_widget_set_usize (cancel_button, 47, -2);

  gtk_signal_connect (GTK_OBJECT (dlg_props), "realize",
                      GTK_SIGNAL_FUNC (dlg_props_realize),
                      NULL);
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
  gtk_signal_connect (GTK_OBJECT (ok_button), "clicked",
                      GTK_SIGNAL_FUNC (props_ok_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (apply_button), "clicked",
                      GTK_SIGNAL_FUNC (props_apply_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cancel_button), "clicked",
                      GTK_SIGNAL_FUNC (props_cancel_button_clicked),
                      NULL);

  return dlg_props;
}

