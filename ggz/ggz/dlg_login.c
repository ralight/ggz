#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "connect.h"
#include "dlg_details.h"
#include "dlg_error.h"
#include "dlg_login.h"
#include "datatypes.h"


/* Globals neaded by this dialog */
extern struct ConnectInfo connection;
extern GtkWidget *detail_window;

/* Global widget for this dialog */
GtkWidget *dlg_login;
GtkWidget *main_win;

/* Local callbacks which no other file will call */
void login_anon_toggled(GtkWidget* button, gpointer window);
void login_fill_defaults(GtkWidget * win, gpointer user_data);
void login_input_options(GtkButton * button, gpointer window);
void login_start_session(GtkButton * button, gpointer window);
void login_show_details(GtkButton * button, gpointer user_data);


GtkWidget*
create_dlg_login (void)
{
  GtkWidget *dlg_login;
  GtkWidget *main_box;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *name_frame;
  GtkWidget *info_box;
  GtkWidget *name_box;
  GtkWidget *name_label;
  GtkWidget *name_entry;
  GtkWidget *pass_box;
  GtkWidget *pass_label;
  GtkWidget *pass_entry;
  GtkWidget *opt_frame;
  GtkWidget *radio_box;
  GSList *login_type_group = NULL;
  GtkWidget *normal_radio;
  GtkWidget *anon_radio;
  GtkWidget *first_radio;
  GtkWidget *server_frame;
  GtkWidget *server_box;
  GtkWidget *host_label;
  GtkWidget *host_entry;
  GtkWidget *port_entry;
  GtkWidget *port_label;
  GtkWidget *hseparator;
  GtkWidget *button_box;
  GtkWidget *connect_button;
  GtkWidget *cancel_button;
  GtkWidget *details_button;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  dlg_login = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_object_set_data (GTK_OBJECT (dlg_login), "dlg_login", dlg_login);
  gtk_window_set_title (GTK_WINDOW (dlg_login), "Login");
  gtk_window_set_position (GTK_WINDOW (dlg_login), GTK_WIN_POS_MOUSE);
  gtk_window_set_policy (GTK_WINDOW (dlg_login), FALSE, FALSE, FALSE);

  main_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (main_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "main_box", main_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (main_box);
  gtk_container_add (GTK_CONTAINER (dlg_login), main_box);
  gtk_container_set_border_width (GTK_CONTAINER (main_box), 5);

  vbox = gtk_vbox_new (FALSE, 5);
  gtk_widget_ref (vbox);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "vbox", vbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox);
  gtk_box_pack_start (GTK_BOX (main_box), vbox, FALSE, FALSE, 0);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "hbox", hbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 5);

  name_frame = gtk_frame_new (NULL);
  gtk_widget_ref (name_frame);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "name_frame", name_frame,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (name_frame);
  gtk_box_pack_start (GTK_BOX (hbox), name_frame, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (name_frame), 5);

  info_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (info_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "info_box", info_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (info_box);
  gtk_container_add (GTK_CONTAINER (name_frame), info_box);
  gtk_container_set_border_width (GTK_CONTAINER (info_box), 10);

  name_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (name_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "name_box", name_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (name_box);
  gtk_box_pack_start (GTK_BOX (info_box), name_box, TRUE, TRUE, 0);

  name_label = gtk_label_new ("Username : ");
  gtk_widget_ref (name_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "name_label", name_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (name_label);
  gtk_box_pack_start (GTK_BOX (name_box), name_label, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (name_label), GTK_JUSTIFY_RIGHT);

  name_entry = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (name_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "name_entry", name_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (name_entry);
  gtk_box_pack_start (GTK_BOX (name_box), name_entry, TRUE, TRUE, 0);
  gtk_widget_set_usize (name_entry, 96, -2);
  gtk_tooltips_set_tip (tooltips, name_entry, "Enter your login name here", NULL);

  pass_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (pass_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "pass_box", pass_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pass_box);
  gtk_box_pack_start (GTK_BOX (info_box), pass_box, TRUE, TRUE, 0);

  pass_label = gtk_label_new ("Password : ");
  gtk_widget_ref (pass_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "pass_label", pass_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pass_label);
  gtk_box_pack_start (GTK_BOX (pass_box), pass_label, FALSE, FALSE, 0);

  pass_entry = gtk_entry_new ();
  gtk_widget_ref (pass_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "pass_entry", pass_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pass_entry);
  gtk_box_pack_start (GTK_BOX (pass_box), pass_entry, TRUE, TRUE, 0);
  gtk_widget_set_usize (pass_entry, 96, -2);
  gtk_tooltips_set_tip (tooltips, pass_entry, "Enter your password here", NULL);
  gtk_entry_set_visibility (GTK_ENTRY (pass_entry), FALSE);

  opt_frame = gtk_frame_new (NULL);
  gtk_widget_ref (opt_frame);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "opt_frame", opt_frame,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_frame);
  gtk_box_pack_start (GTK_BOX (hbox), opt_frame, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (opt_frame), 5);

  radio_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (radio_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "radio_box", radio_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radio_box);
  gtk_container_add (GTK_CONTAINER (opt_frame), radio_box);
  gtk_container_set_border_width (GTK_CONTAINER (radio_box), 5);

  normal_radio = gtk_radio_button_new_with_label (login_type_group, "Normal Login");
  login_type_group = gtk_radio_button_group (GTK_RADIO_BUTTON (normal_radio));
  gtk_widget_ref (normal_radio);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "normal_radio", normal_radio,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (normal_radio);
  gtk_box_pack_start (GTK_BOX (radio_box), normal_radio, FALSE, FALSE, 0);

  anon_radio = gtk_radio_button_new_with_label (login_type_group, "Anonymous Login");
  login_type_group = gtk_radio_button_group (GTK_RADIO_BUTTON (anon_radio));
  gtk_widget_ref (anon_radio);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "anon_radio", anon_radio,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (anon_radio);
  gtk_box_pack_start (GTK_BOX (radio_box), anon_radio, FALSE, FALSE, 0);

  first_radio = gtk_radio_button_new_with_label (login_type_group, "First-time Login");
  login_type_group = gtk_radio_button_group (GTK_RADIO_BUTTON (first_radio));
  gtk_widget_ref (first_radio);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "first_radio", first_radio,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (first_radio);
  gtk_box_pack_start (GTK_BOX (radio_box), first_radio, FALSE, FALSE, 0);

  server_frame = gtk_frame_new (NULL);
  gtk_widget_ref (server_frame);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "server_frame", server_frame,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (server_frame);
  gtk_box_pack_start (GTK_BOX (vbox), server_frame, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (server_frame), 5);

  server_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (server_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "server_box", server_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (server_box);
  gtk_container_add (GTK_CONTAINER (server_frame), server_box);
  gtk_container_set_border_width (GTK_CONTAINER (server_box), 10);

  host_label = gtk_label_new ("Server:");
  gtk_widget_ref (host_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "host_label", host_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (host_label);
  gtk_box_pack_start (GTK_BOX (server_box), host_label, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (host_label), GTK_JUSTIFY_RIGHT);

  host_entry = gtk_entry_new_with_max_length (256);
  gtk_widget_ref (host_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "host_entry", host_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (host_entry);
  gtk_box_pack_start (GTK_BOX (server_box), host_entry, FALSE, FALSE, 5);

  port_entry = gtk_entry_new ();
  gtk_widget_ref (port_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "port_entry", port_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (port_entry);
  gtk_box_pack_end (GTK_BOX (server_box), port_entry, FALSE, FALSE, 5);
  gtk_widget_set_usize (port_entry, 50, -2);

  port_label = gtk_label_new ("Port:");
  gtk_widget_ref (port_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "port_label", port_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (port_label);
  gtk_box_pack_end (GTK_BOX (server_box), port_label, FALSE, FALSE, 0);

  hseparator = gtk_hseparator_new ();
  gtk_widget_ref (hseparator);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "hseparator", hseparator,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hseparator);
  gtk_box_pack_start (GTK_BOX (vbox), hseparator, TRUE, TRUE, 0);

  button_box = gtk_hbutton_box_new ();
  gtk_widget_ref (button_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "button_box", button_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button_box);
  gtk_box_pack_start (GTK_BOX (vbox), button_box, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (button_box), 5);

  connect_button = gtk_button_new_with_label ("Connect");
  gtk_widget_ref (connect_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "connect_button", connect_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (connect_button);
  gtk_container_add (GTK_CONTAINER (button_box), connect_button);
  GTK_WIDGET_SET_FLAGS (connect_button, GTK_CAN_DEFAULT);

  cancel_button = gtk_button_new_with_label ("Cancel");
  gtk_widget_ref (cancel_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "cancel_button", cancel_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cancel_button);
  gtk_container_add (GTK_CONTAINER (button_box), cancel_button);
  GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);

  details_button = gtk_button_new_with_label ("Details >>");
  gtk_widget_ref (details_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "details_button", details_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (details_button);
  gtk_container_add (GTK_CONTAINER (button_box), details_button);
  gtk_widget_set_usize (details_button, 93, -2);
  GTK_WIDGET_SET_FLAGS (details_button, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (dlg_login), "delete_event",
                      GTK_SIGNAL_FUNC (gtk_widget_destroy),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (dlg_login), "destroy_event",
                      GTK_SIGNAL_FUNC (gtk_widget_destroy),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (dlg_login), "realize",
                      GTK_SIGNAL_FUNC (login_fill_defaults),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (anon_radio), "toggled",
                      GTK_SIGNAL_FUNC (login_anon_toggled),
                      dlg_login);
  gtk_signal_connect (GTK_OBJECT (connect_button), "clicked",
                      GTK_SIGNAL_FUNC (login_input_options),
                      dlg_login);
  gtk_signal_connect (GTK_OBJECT (connect_button), "clicked",
                      GTK_SIGNAL_FUNC (login_start_session),
                      dlg_login);
  gtk_signal_connect_object (GTK_OBJECT (cancel_button), "clicked",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy),
                             GTK_OBJECT (dlg_login));
  gtk_signal_connect (GTK_OBJECT (details_button), "clicked",
                      GTK_SIGNAL_FUNC (login_show_details),
                      dlg_login);

  gtk_widget_grab_default (connect_button);
  gtk_object_set_data (GTK_OBJECT (dlg_login), "tooltips", tooltips);

  return dlg_login;
}


/*                              *
 *           Callbacks          *
 *                              */

void login_anon_toggled(GtkWidget* button, gpointer window)
{ 
        gpointer tmp;
        tmp = gtk_object_get_data(GTK_OBJECT(window), "pass_box");
 
        if (GTK_TOGGLE_BUTTON(button)->active) 
                gtk_widget_hide(GTK_WIDGET(tmp));
        else
                gtk_widget_show(GTK_WIDGET(tmp));
}


void login_fill_defaults(GtkWidget * win, gpointer user_data)
{
        gpointer *tmp;
        gchar port[5];

        tmp = gtk_object_get_data(GTK_OBJECT(win), "name_entry");
        if (connection.username)
                gtk_entry_set_text(GTK_ENTRY(tmp), connection.username);
        else
                gtk_entry_set_text(GTK_ENTRY(tmp), getenv("LOGNAME"));


        tmp = gtk_object_get_data(GTK_OBJECT(win), "host_entry");
        if (connection.server)
                gtk_entry_set_text(GTK_ENTRY(tmp), connection.server);
        else
                gtk_entry_set_text(GTK_ENTRY(tmp), "localhost");

        tmp = gtk_object_get_data(GTK_OBJECT(win), "port_entry");
        if (connection.port) {
                snprintf(port, 5, "%d", connection.port);
                gtk_entry_set_text(GTK_ENTRY(tmp), port);
        }
        else
                gtk_entry_set_text(GTK_ENTRY(tmp), "7626");

        tmp = gtk_object_get_data(GTK_OBJECT(win), "anon_radio");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
}

void login_input_options(GtkButton * button, gpointer window)
{
        gpointer tmp;

        tmp = gtk_object_get_data(GTK_OBJECT(window), "name_entry");
        connection.username = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));

        tmp = gtk_object_get_data(GTK_OBJECT(window), "pass_entry");
        connection.password = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));

        tmp = gtk_object_get_data(GTK_OBJECT(window), "host_entry");
        connection.server = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));

        tmp = gtk_object_get_data(GTK_OBJECT(window), "port_entry");
        connection.port = atoi(gtk_entry_get_text(GTK_ENTRY(tmp)));

        tmp = gtk_object_get_data(GTK_OBJECT(window), "normal_radio");
        if (GTK_TOGGLE_BUTTON(tmp)->active)
                connection.login_type = GGZ_LOGIN;

        tmp = gtk_object_get_data(GTK_OBJECT(window), "anon_radio");
        if (GTK_TOGGLE_BUTTON(tmp)->active)
                connection.login_type = GGZ_LOGIN_ANON;

        tmp = gtk_object_get_data(GTK_OBJECT(window), "first_radio");
        if (GTK_TOGGLE_BUTTON(tmp)->active)
                connection.login_type = GGZ_LOGIN_NEW;
}

void login_start_session(GtkButton * button, gpointer window)
{
	GtkWidget *tmp;

	gtk_widget_set_sensitive(GTK_WIDGET(button),FALSE);

        if (connection.connected) {
        	tmp = gtk_object_get_data(GTK_OBJECT(window), "name_entry");
	        connection.username = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));
                
        	tmp = gtk_object_get_data(GTK_OBJECT(window), "pass_entry");
	        connection.password = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));

                switch (connection.login_type) {
                case 0: /*Normal login */  
                case 2: /*First time login */
                case 1: /*Anonymous login */
                        anon_login();
                }

                return;
        } else {

        	/* FIXME: Initialize for new game session */

	        if (connect_to_server() < 0) {
        	        err_dlg("Could not connect");
                	return;
	        }
	}

        connection.connected = TRUE;

        /*FIXME: Other session starting things ? */
}


void login_show_details(GtkButton * button, gpointer user_data)
{
        detail_window = create_dlg_details();
	gtk_widget_set_sensitive(GTK_WIDGET(button),FALSE);
        gtk_widget_show(detail_window);
	
}

void login_ok()
{
	if(dlg_login)
		gtk_widget_destroy(dlg_login);	
}

void login_bad_name()
{
        GtkWidget *tmp;

        tmp = gtk_object_get_data(GTK_OBJECT(dlg_login), "connect_button");
	gtk_label_set_text(GTK_LABEL(GTK_BIN(tmp)->child),"Login");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_login), "server_frame");
	gtk_widget_hide(tmp);
}

void login_disconnect()
{
        GtkWidget *tmp;

	/* Set sensitive FALSE */
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "disconnect");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);

        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "game_menu");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "edit");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "view_menu");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);

        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "msg_entry");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "msg_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "room_combo");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);

        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "launch_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "join_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "prefs_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "stats_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "disconnect_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);

        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "player_list");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "table_tree");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);

	/* Set sensitive TRUE */
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "connect");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "connect_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
}

void login_online()
{
        GtkWidget *tmp;

	/* Set sensitive FALSE */
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "connect");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "connect_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);

	/* Set sensitive TRUE */
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "disconnect");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "game_menu");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "edit");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "view_menu");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);

        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "msg_entry");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "msg_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "room_combo");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);

        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "launch_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "join_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "prefs_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "stats_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "disconnect_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);

        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "player_list");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "table_tree");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);

}


