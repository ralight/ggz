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
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "chat.h"
#include "dlg_props.h"
#include "ggzrc.h"
#include "xtext.h"
#include "server.h"
#include "support.h"

/* Globals for the props dialog */
GtkWidget *dlg_props;
GtkWidget *dlg_props_font = NULL;
extern GtkWidget *main_win;

void props_update();
void props_add_button_clicked (GtkButton *button, gpointer user_data);
void props_modify_button_clicked (GtkButton *button, gpointer user_data);
void props_delete_button_clicked (GtkButton *button, gpointer user_data);
void props_Font_button_clicked (GtkButton *button, gpointer user_data);
void props_ok_button_clicked (GtkButton *button, gpointer user_data);
void props_apply_button_clicked (GtkButton *button, gpointer user_data);
void props_cancel_button_clicked (GtkButton *button, gpointer user_data);
void dlg_props_realize (GtkWidget *widget, gpointer user_data);
void props_color_toggled (GtkWidget *button, gpointer user_data);
void props_color_type_toggled (GtkButton *button, gpointer user_data);
void props_fok_button_clicked (GtkButton *button, gpointer user_data);
void props_fapply_button_clicked (GtkButton *button, gpointer user_data);
void props_fcancel_button_clicked (GtkButton *button, gpointer user_data);
void on_profile_list_select_row (GtkWidget *widget, gint row, gint column,
				 GdkEventButton *event, gpointer data);
GtkWidget *create_fontselect (void);


void props_update()
{
	GtkWidget *tmp;
	GdkFont *font;
	/* Servers Tab */

	/* Chat Tab */
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "chat_font");
	ggzrc_write_string("CHAT","Font", gtk_entry_get_text(GTK_ENTRY(tmp)));
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

        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "color_check");
	if(GTK_TOGGLE_BUTTON(tmp)->active != FALSE)
	{
	        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "full_radio");
		if(GTK_TOGGLE_BUTTON(tmp)->active != FALSE)
		{
			ggzrc_write_int("CHAT","ColorNames", CHAT_COLOR_FULL);
		} else {
			ggzrc_write_int("CHAT","ColorNames", CHAT_COLOR_SOME);
		        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "y_spin");
			ggzrc_write_int("CHAT","YourColor",
				gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(tmp)));
		        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "f_spin");
			ggzrc_write_int("CHAT","FriendsColor",
				gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(tmp)));
		        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "o_spin");
			ggzrc_write_int("CHAT","OthersColor",
				gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(tmp)));
		}
	} else
		ggzrc_write_int("CHAT","ColorNames", CHAT_COLOR_NONE);

        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "chat_text");
	GTK_XTEXT(tmp)->auto_indent = ggzrc_read_int("CHAT","AutoIndent",TRUE);
	GTK_XTEXT(tmp)->wordwrap = ggzrc_read_int("CHAT","WordWrap",TRUE);
	GTK_XTEXT(tmp)->time_stamp = ggzrc_read_int("CHAT","Timestamp",FALSE);
	font = gdk_font_load(ggzrc_read_string("CHAT","Font",
		"-*-fixed-medium-r-semicondensed--*-120-*-*-c-*-iso8859-8"));
	gtk_xtext_set_font(GTK_XTEXT(tmp), font, 0);
	gtk_xtext_refresh(GTK_XTEXT(tmp),0);

	/*User Info Tab*/
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "info_name");
	ggzrc_write_string("UserInfo","Name",gtk_entry_get_text(GTK_ENTRY(tmp)));
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "info_city");
	ggzrc_write_string("UserInfo","City",gtk_entry_get_text(GTK_ENTRY(tmp)));
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "info_state");
	ggzrc_write_string("UserInfo","State",gtk_entry_get_text(GTK_ENTRY(tmp)));
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "info_country");
	ggzrc_write_string("UserInfo","Country",gtk_entry_get_text(GTK_ENTRY(tmp)));
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "info_comments");
	ggzrc_write_string("UserInfo","Comments",gtk_editable_get_chars(GTK_EDITABLE(tmp), 0, -1));
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

void on_profile_list_select_row (GtkWidget *widget, gint row, gint column,
				 GdkEventButton *event, gpointer data)
{
	GtkWidget *tmp;
	char *profile_name;
	Server *profile;
	char *port;

	tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "profile_list");
	gtk_clist_get_text( GTK_CLIST(tmp), row, column, &profile_name);
	profile = server_get(profile_name);

	tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "pro_name");
	if(profile->name != NULL)
		gtk_entry_set_text( GTK_ENTRY(tmp), profile->name );
	else
		gtk_entry_set_text( GTK_ENTRY(tmp), "" );

	tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "pro_server");
	if(profile->host != NULL)
		gtk_entry_set_text( GTK_ENTRY(tmp), profile->host );
	else
		gtk_entry_set_text( GTK_ENTRY(tmp), "" );

	tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "pro_port");
	port = g_strdup_printf("%d", profile->port);
	gtk_entry_set_text( GTK_ENTRY(tmp), port );
	g_free(port);

	tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "pro_username");
	if(profile->login != NULL)
		gtk_entry_set_text( GTK_ENTRY(tmp), profile->login );
	else
		gtk_entry_set_text( GTK_ENTRY(tmp), "" );

	tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "pro_password");
	if(profile->password != NULL)
		gtk_entry_set_text( GTK_ENTRY(tmp), profile->password );
	else
		gtk_entry_set_text( GTK_ENTRY(tmp), "" );

	if( profile->type == 0 )
	{
		tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "radiobutton3");
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(tmp), TRUE );
	}
	if( profile->type == 1 )
	{
		tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "radiobutton4");
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(tmp), TRUE );
	}
}

void props_Font_button_clicked (GtkButton *button, gpointer user_data)
{
	if( dlg_props_font == NULL )
	{
		dlg_props_font = create_fontselect();
		gtk_widget_show(dlg_props_font);
	}
}

void props_ok_button_clicked (GtkButton *button, gpointer user_data)
{
	/* Save changes, and close the dialog */
	GtkWidget *tmp;
	props_update();

	/* Close font selector if open */
	if( dlg_props_font != NULL )
	{
	        gtk_widget_destroy(dlg_props_font);
        	dlg_props_font = NULL;
	}

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

	/* Close font selector if open */
	if( dlg_props_font != NULL )
	{
	        gtk_widget_destroy(dlg_props_font);
        	dlg_props_font = NULL;
	}

	/* Close the dialog */
        gtk_widget_destroy(dlg_props);
        dlg_props = NULL;
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "props_button");
        gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "properties");
        gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
}

void props_color_toggled (GtkWidget *button, gpointer user_data)
{
	GtkWidget *tmp;
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "color_table");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),GTK_TOGGLE_BUTTON(button)->active);
}

void props_color_type_toggled (GtkButton *button, gpointer user_data)
{
	GtkWidget *tmp;
	GtkWidget *option;

        option = gtk_object_get_data(GTK_OBJECT(dlg_props), "some_radio");
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "y_spin");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),GTK_TOGGLE_BUTTON(option)->active);
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "f_spin");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),GTK_TOGGLE_BUTTON(option)->active);
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "o_spin");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),GTK_TOGGLE_BUTTON(option)->active);
}

void dlg_props_realize (GtkWidget *widget, gpointer user_data)
{
	/* Set all options to current settings */
	GtkWidget *tmp;
	GList *items = NULL;
	GList *item;
	guint x;
	gchar *entry[1];

	/* Servers Tab */
	items = server_get_names();
	if (items != NULL)
	{
		tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "profile_list");
		for(x = 0; x != g_list_length(items); x++)
		{
			item = g_list_nth( items, x );
		        entry[0] = g_strdup_printf("%s",
						   (char*)item->data);
		        gtk_clist_append(GTK_CLIST(tmp), entry);
        		g_free(entry[0]);
		}
	}

	/* Chat Tab */
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "chat_font");
	gtk_entry_set_text(GTK_ENTRY(tmp), ggzrc_read_string("CHAT","Font",
		"-*-fixed-medium-r-semicondensed--*-120-*-*-c-*-iso8859-8"));
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
	if(ggzrc_read_int("CHAT","ColorNames",CHAT_COLOR_SOME) != CHAT_COLOR_NONE)
	{
	        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "color_check");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
		if(ggzrc_read_int("CHAT","ColorNames",CHAT_COLOR_SOME) == CHAT_COLOR_FULL)
		{
		        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "full_radio");
		} else {
		        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "some_radio");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
		        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "y_spin");
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(tmp),
				ggzrc_read_int("CHAT","YourColor",1));
		        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "f_spin");
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(tmp),
				ggzrc_read_int("CHAT","FriendsColor",2));
		        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "o_spin");
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(tmp),
				ggzrc_read_int("CHAT","OthersColor",12));
		}
	} else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), FALSE);
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "color_check");
	props_color_toggled(GTK_WIDGET(tmp), NULL);
	props_color_type_toggled(NULL, NULL);
	
	/*User Info Tab*/
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "info_name");
	gtk_entry_set_text(GTK_ENTRY(tmp), ggzrc_read_string("UserInfo","Name","."));
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "info_city");
	gtk_entry_set_text(GTK_ENTRY(tmp), ggzrc_read_string("UserInfo","City","."));
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "info_state");
	gtk_entry_set_text(GTK_ENTRY(tmp), ggzrc_read_string("UserInfo","State","."));
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "info_country");
	gtk_entry_set_text(GTK_ENTRY(tmp), ggzrc_read_string("UserInfo","Country","."));
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "info_comments");
	gtk_text_insert (GTK_TEXT (tmp), NULL, NULL, NULL, ggzrc_read_string("UserInfo","Comments","."), -1);
}

void props_fok_button_clicked (GtkButton *button, gpointer user_data)
{
	GtkWidget *tmp;

	/* Set font */
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "chat_font");
	gtk_entry_set_text(GTK_ENTRY(tmp),
		gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(dlg_props_font)));

	/* Close the font selector */
	gtk_widget_destroy(dlg_props_font);
	dlg_props_font = NULL;
}

void props_fapply_button_clicked (GtkButton *button, gpointer user_data)
{
	GtkWidget *tmp;

	/* Set font */
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_props), "chat_font");
	gtk_entry_set_text(GTK_ENTRY(tmp),
		gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(dlg_props_font)));
}

void props_fcancel_button_clicked (GtkButton *button, gpointer user_data)
{
	/* Close the font selector */
	gtk_widget_destroy(dlg_props_font);
	dlg_props_font = NULL;
}



GtkWidget*
create_dlg_props (void)
{
  GtkWidget *dlg_props;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *notebook1;
  GtkWidget *hbox2;
  GtkWidget *scrolledwindow2;
  GtkWidget *profile_list;
  GtkWidget *label28;
  GtkWidget *vbuttonbox1;
  GtkWidget *button4;
  GtkWidget *button5;
  GtkWidget *button6;
  GtkWidget *vbox2;
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
  GtkWidget *frame2;
  GtkWidget *vbox10;
  GSList *vbox10_group = NULL;
  GtkWidget *radiobutton3;
  GtkWidget *radiobutton4;
  GtkWidget *label1;
  GtkWidget *vbox4;
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

  scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "scrolledwindow2", scrolledwindow2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow2);
  gtk_box_pack_start (GTK_BOX (hbox2), scrolledwindow2, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  profile_list = gtk_clist_new (1);
  gtk_widget_ref (profile_list);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "profile_list", profile_list,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_list);
  gtk_container_add (GTK_CONTAINER (scrolledwindow2), profile_list);
  gtk_clist_set_column_width (GTK_CLIST (profile_list), 0, 80);
  gtk_clist_column_titles_show (GTK_CLIST (profile_list));

  label28 = gtk_label_new (_("Profiles"));
  gtk_widget_ref (label28);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label28", label28,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label28);
  gtk_clist_set_column_widget (GTK_CLIST (profile_list), 0, label28);

  vbuttonbox1 = gtk_vbutton_box_new ();
  gtk_widget_ref (vbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "vbuttonbox1", vbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbuttonbox1);
  gtk_box_pack_start (GTK_BOX (hbox2), vbuttonbox1, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (vbuttonbox1), GTK_BUTTONBOX_START);

  button4 = gtk_button_new_with_label (_("Add"));
  gtk_widget_ref (button4);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "button4", button4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button4);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), button4);
  GTK_WIDGET_SET_FLAGS (button4, GTK_CAN_DEFAULT);

  button5 = gtk_button_new_with_label (_("Modify"));
  gtk_widget_ref (button5);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "button5", button5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button5);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), button5);
  GTK_WIDGET_SET_FLAGS (button5, GTK_CAN_DEFAULT);

  button6 = gtk_button_new_with_label (_("Delete"));
  gtk_widget_ref (button6);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "button6", button6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button6);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), button6);
  GTK_WIDGET_SET_FLAGS (button6, GTK_CAN_DEFAULT);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "vbox2", vbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (hbox2), vbox2, TRUE, TRUE, 5);
  gtk_widget_set_usize (vbox2, 162, -2);

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
  gtk_box_pack_start (GTK_BOX (vbox3), hbox4, FALSE, TRUE, 0);

  label7 = gtk_label_new (_("Profile Name:  "));
  gtk_widget_ref (label7);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label7", label7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label7);
  gtk_box_pack_start (GTK_BOX (hbox4), label7, FALSE, FALSE, 0);
  gtk_widget_set_usize (label7, 95, -2);
  gtk_label_set_justify (GTK_LABEL (label7), GTK_JUSTIFY_LEFT);
  gtk_label_set_line_wrap (GTK_LABEL (label7), TRUE);

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
  gtk_box_pack_start (GTK_BOX (vbox3), hbox5, FALSE, TRUE, 5);

  label8 = gtk_label_new (_("Server:  "));
  gtk_widget_ref (label8);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label8", label8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label8);
  gtk_box_pack_start (GTK_BOX (hbox5), label8, FALSE, FALSE, 0);
  gtk_widget_set_usize (label8, 95, -2);
  gtk_label_set_justify (GTK_LABEL (label8), GTK_JUSTIFY_LEFT);
  gtk_label_set_line_wrap (GTK_LABEL (label8), TRUE);

  pro_server = gtk_entry_new ();
  gtk_widget_ref (pro_server);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "pro_server", pro_server,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pro_server);
  gtk_box_pack_start (GTK_BOX (hbox5), pro_server, TRUE, TRUE, 0);
  gtk_widget_set_usize (pro_server, 221, -2);

  label9 = gtk_label_new (_(" Port:  "));
  gtk_widget_ref (label9);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label9", label9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label9);
  gtk_box_pack_start (GTK_BOX (hbox5), label9, FALSE, FALSE, 0);
  gtk_widget_set_usize (label9, 45, -2);

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
  gtk_box_pack_start (GTK_BOX (vbox3), hbox6, FALSE, TRUE, 0);

  label10 = gtk_label_new (_("Username:  "));
  gtk_widget_ref (label10);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label10", label10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label10);
  gtk_box_pack_start (GTK_BOX (hbox6), label10, FALSE, FALSE, 0);
  gtk_widget_set_usize (label10, 95, -2);
  gtk_label_set_justify (GTK_LABEL (label10), GTK_JUSTIFY_LEFT);
  gtk_label_set_line_wrap (GTK_LABEL (label10), TRUE);

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
  gtk_box_pack_start (GTK_BOX (vbox3), hbox7, FALSE, TRUE, 5);

  label11 = gtk_label_new (_("Password:  "));
  gtk_widget_ref (label11);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "label11", label11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label11);
  gtk_box_pack_start (GTK_BOX (hbox7), label11, FALSE, FALSE, 0);
  gtk_widget_set_usize (label11, 95, -2);
  gtk_label_set_justify (GTK_LABEL (label11), GTK_JUSTIFY_LEFT);
  gtk_label_set_line_wrap (GTK_LABEL (label11), TRUE);

  pro_password = gtk_entry_new ();
  gtk_widget_ref (pro_password);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "pro_password", pro_password,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pro_password);
  gtk_box_pack_start (GTK_BOX (hbox7), pro_password, TRUE, TRUE, 0);

  frame2 = gtk_frame_new (_("Login Type"));
  gtk_widget_ref (frame2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "frame2", frame2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox3), frame2, FALSE, TRUE, 0);

  vbox10 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox10);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "vbox10", vbox10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox10);
  gtk_container_add (GTK_CONTAINER (frame2), vbox10);
  gtk_container_set_border_width (GTK_CONTAINER (vbox10), 5);

  radiobutton3 = gtk_radio_button_new_with_label (vbox10_group, _("Normal"));
  vbox10_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton3));
  gtk_widget_ref (radiobutton3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "radiobutton3", radiobutton3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton3);
  gtk_box_pack_start (GTK_BOX (vbox10), radiobutton3, FALSE, FALSE, 0);

  radiobutton4 = gtk_radio_button_new_with_label (vbox10_group, _("Guest"));
  vbox10_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton4));
  gtk_widget_ref (radiobutton4);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "radiobutton4", radiobutton4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton4);
  gtk_box_pack_start (GTK_BOX (vbox10), radiobutton4, FALSE, FALSE, 0);

  label1 = gtk_label_new (_("Servers"));
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
  gtk_box_pack_start (GTK_BOX (vbox4), chat_table, FALSE, FALSE, 4);

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
  gtk_box_pack_start (GTK_BOX (vbox4), frame1, TRUE, TRUE, 0);

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
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);

  vbox6 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox6);
  gtk_object_set_data_full (GTK_OBJECT (dlg_props), "vbox6", vbox6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox6);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox6);

  label16 = gtk_label_new (_("All of the following information is optional."));
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
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label3);

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
  gtk_signal_connect (GTK_OBJECT (profile_list), "select_row",
                      GTK_SIGNAL_FUNC (on_profile_list_select_row),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (button4), "clicked",
                      GTK_SIGNAL_FUNC (props_add_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (button5), "clicked",
                      GTK_SIGNAL_FUNC (props_modify_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (button6), "clicked",
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

GtkWidget*
create_fontselect (void)
{
  GtkWidget *fontselect;
  GtkWidget *ok_button1;
  GtkWidget *cancel_button1;
  GtkWidget *apply_button1;

  fontselect = gtk_font_selection_dialog_new (_("Select Font"));
  gtk_object_set_data (GTK_OBJECT (fontselect), "fontselect", fontselect);
  gtk_container_set_border_width (GTK_CONTAINER (fontselect), 4);
  gtk_window_set_policy (GTK_WINDOW (fontselect), FALSE, TRUE, TRUE);

  ok_button1 = GTK_FONT_SELECTION_DIALOG (fontselect)->ok_button;
  gtk_object_set_data (GTK_OBJECT (fontselect), "ok_button1", ok_button1);
  gtk_widget_show (ok_button1);
  GTK_WIDGET_SET_FLAGS (ok_button1, GTK_CAN_DEFAULT);

  cancel_button1 = GTK_FONT_SELECTION_DIALOG (fontselect)->cancel_button;
  gtk_object_set_data (GTK_OBJECT (fontselect), "cancel_button1", cancel_button1);
  gtk_widget_show (cancel_button1);
  GTK_WIDGET_SET_FLAGS (cancel_button1, GTK_CAN_DEFAULT);

  apply_button1 = GTK_FONT_SELECTION_DIALOG (fontselect)->apply_button;
  gtk_object_set_data (GTK_OBJECT (fontselect), "apply_button1", apply_button1);
  gtk_widget_show (apply_button1);
  GTK_WIDGET_SET_FLAGS (apply_button1, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (ok_button1), "clicked",
                      GTK_SIGNAL_FUNC (props_fok_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cancel_button1), "clicked",
                      GTK_SIGNAL_FUNC (props_fcancel_button_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (apply_button1), "clicked",
                      GTK_SIGNAL_FUNC (props_fapply_button_clicked),
                      NULL);

  return fontselect;
}

