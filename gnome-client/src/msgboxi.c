/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gnome.h>

#include "msgboxc.h"
#include "msgboxi.h"
#include "support.h"
#include "login.h"

void msgbox_error(gchar *msg)
{
	GtkWidget *msgbox;
	GtkWidget *tmp;
	
	msgbox = create_msgbox();
	gtk_widget_realize(msgbox);
	tmp = lookup_widget(msgbox, "lblText");

	if (!strcmp(msg, "Name taken"))
	{
		gtk_label_set_text (GTK_LABEL (tmp), 
		"The username you have chosen is either "
		"registered to someone else, or already "
		"logged in to the zone. Please choose a "
		"different username and try again.");
		login_failed();
	} else if(!strcmp(msg, "Error connecting to server: No such file or directory")) {
		gtk_label_set_text (GTK_LABEL (tmp), 
		"The GGZ Gaming Zone server you are "
		"tryinh to connect to is not responding. "
		" Please check to make sure the server "
		"address you have entered is correct.");
		login_connect_failed();
	} else if(!strcmp(msg, "Already logged in")) {
		gtk_label_set_text (GTK_LABEL (tmp), 
		"The username you are trying to use is "
		"invalid. Please make sure your username "
		"does not contain any of the following "
		"characters:\n"
		"\" \", \"'\"");
		login_failed();
	} else if(!strcmp(msg, "Error connecting to server: Connection refused")) {
		gtk_label_set_text (GTK_LABEL (tmp), 
		"The computer you are trying to connect to "
		"does not apear to be running a server. "
		"Please verify that the address and port "
		"are correct and try again.");
		login_connect_failed();
	} else if(!strcmp(msg, "Server error: Bad XML from server")) {
		gtk_label_set_text (GTK_LABEL (tmp), 
		"A network error occured. The server "
		"has disconnected you. ");
	} else if(!strcmp(msg, "Server error: Server disconnected")) {
		gtk_label_set_text (GTK_LABEL (tmp), 
		"The server has just disconnected you../");
	} else {	
		g_print("%s\n", msg);
	}

	gtk_widget_show(msgbox);
}


GtkWidget*
create_msgbox (void)
{
  GtkWidget *window;
  GtkWidget *fixMain;
  GtkWidget *pmBackground;
  GtkWidget *lblText;
  GtkWidget *btnOK;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_name (window, "window");
  gtk_object_set_data (GTK_OBJECT (window), "window", window);
  gtk_widget_set_size_request (window, 450, 150);
  gtk_window_set_title (GTK_WINDOW (window), _("GGZ Gaming Zone"));
  gtk_window_set_policy (GTK_WINDOW (window), FALSE, FALSE, TRUE);

  fixMain = gtk_fixed_new ();
  gtk_widget_set_name (fixMain, "fixMain");
  gtk_widget_ref (fixMain);
  gtk_object_set_data_full (GTK_OBJECT (window), "fixMain", fixMain,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (fixMain);
  gtk_container_add (GTK_CONTAINER (window), fixMain);
  gtk_widget_set_size_request (fixMain, 450, 150);

  pmBackground = create_pixmap (window, "msg.xpm");
  gtk_widget_set_name (pmBackground, "pmBackground");
  gtk_widget_ref (pmBackground);
  gtk_object_set_data_full (GTK_OBJECT (window), "pmBackground", pmBackground,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pmBackground);
  gtk_fixed_put (GTK_FIXED (fixMain), pmBackground, 0, 0);
  gtk_widget_set_uposition (pmBackground, 0, 0);
  gtk_widget_set_size_request (pmBackground, 450, 150);

  lblText = gtk_label_new (NULL);
  gtk_label_set_justify (GTK_LABEL (lblText), GTK_JUSTIFY_LEFT);
  gtk_label_set_line_wrap (GTK_LABEL (lblText), TRUE);
  gtk_widget_set_name (lblText, "lblText");
  gtk_widget_ref (lblText);
  gtk_object_set_data_full (GTK_OBJECT (window), "lblText", lblText,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(lblText);
  gtk_fixed_put (GTK_FIXED (fixMain), lblText, 125, 12);
  gtk_widget_set_uposition (lblText, 125, 12);
  gtk_widget_set_size_request (lblText, 320, 75);

  btnOK = gtk_button_new_with_label (_("OK"));
  gtk_widget_set_name (btnOK, "btnOK");
  gtk_widget_ref (btnOK);
  gtk_object_set_data_full (GTK_OBJECT (window), "btnOK", btnOK,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnOK);
  gtk_fixed_put (GTK_FIXED (fixMain), btnOK, 190, 120);
  gtk_widget_set_uposition (btnOK, 190, 120);
  gtk_widget_set_size_request (btnOK, 70, 22);

  gtk_window_set_modal (GTK_WINDOW (window), TRUE);

  gtk_signal_connect (GTK_OBJECT (window), "destroy_event",
                      GTK_SIGNAL_FUNC (on_msgbox_destroy_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnOK), "clicked",
                      GTK_SIGNAL_FUNC (on_btnOK_clicked),
                      window);

  return window;
}

