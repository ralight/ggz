#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "client.h"
#include "connect.h"
/*#include <gtk_dlg_options.h>*/
#include "dlg_error.h"
#include "protocols.h"
#include "options.h"
#include "game.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "err_func.h"


GtkWidget* detail_window = NULL;
extern Options opt;

void
InputOptions                           (GtkButton       *button,
                                        gpointer         user_data)
{
  
  GtkWidget* tmp;

  tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(user_data),"name_entry"));
  opt.user_name = g_strdup(gtk_entry_get_text( GTK_ENTRY(tmp)));

  tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(user_data),"pass_entry"));
  opt.password = g_strdup(gtk_entry_get_text( GTK_ENTRY(tmp)));

  tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(user_data),"host_entry"));
  opt.server = g_strdup( gtk_entry_get_text( GTK_ENTRY(tmp) ) );

  tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(user_data),"port_entry"));
  opt.port = atoi( gtk_entry_get_text( GTK_ENTRY(tmp) ) );
  
  tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(user_data),"normal_radio"));
  if( GTK_TOGGLE_BUTTON(tmp)->active )
    opt.login_type = 0;

  tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(user_data),"anon_radio"));
  if( GTK_TOGGLE_BUTTON(tmp)->active ) 
    opt.login_type = 1;

  tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(user_data),"first_radio"));
  if( GTK_TOGGLE_BUTTON(tmp)->active ) 
    opt.login_type = 2;
  
  
}

void
StartSession                           (GtkButton       *button,
                                        gpointer         user_data)
{

	if (opt.connected) {
		DisplayWarning("Already Connected.");
		return;
	}
	
	/* FIXME: Initialize for new game session */
	
	if (connect_to_server() < 0) {
		DisplayError("Could not connect");
		return;
	}

	/* Close connect dialog if we were successful*/
	opt.connected = 1;
	gtk_widget_destroy(GTK_WIDGET(user_data));
	/*FIXME: Other session starting things ? */
}


void
show_details                           (GtkButton       *button,
                                        gpointer         user_data)
{
  detail_window = create_dlg_details();
  gtk_widget_show(detail_window);
}


void
cancel_details                         (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_destroy(detail_window);
  detail_window = NULL;
}



void
join_game                              (GtkButton       *button,
                                        gpointer         user_data) 
{
	dbg_msg("joining game");
	CheckWriteInt(opt.sock, REQ_JOIN_GAME);
	CheckWriteInt(opt.sock, 0);
}


void
get_game_options                       (GtkButton       *button,
                                        gpointer         user_data) 
{
	int type = 0;	/* FIXME: Input type of game to launch */
	
	if (!opt.connected) 
		DisplayWarning("Not connected!");
	else {
		launch_game(type);
	}
}


void
logout                                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	dbg_msg("Logging out");
	CheckWriteInt(opt.sock, REQ_LOGOUT);
}


void
get_types                              (GtkMenuItem     *menuitem,
                                        gpointer         user_data) 
{
  char verbose = 1;
  
  CheckWriteInt(opt.sock, REQ_GAME_TYPES);
  write(opt.sock, &verbose, 1);
}

void
get_players                            (GtkMenuItem     *menuitem,
                                        gpointer         user_data) 
{
  CheckWriteInt(opt.sock, REQ_USER_LIST);
}

void
get_tables                             (GtkMenuItem     *menuitem,
                                        gpointer         user_data) 
{
  CheckWriteInt(opt.sock, REQ_TABLE_LIST);
  CheckWriteInt(opt.sock, -1);
}




void
fill_defaults                          (GtkWidget       *win,
                                        gpointer         user_data) 
{
	GtkWidget* tmp;
	char port[5];
	
	if (opt.user_name) {
		tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(win), 
						     "name_entry"));
		gtk_entry_set_text(GTK_ENTRY(tmp), opt.user_name);
	}
	
	if (opt.server) {
		tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(win), 
						     "host_entry"));
		gtk_entry_set_text(GTK_ENTRY(tmp), opt.server);
	}
	
	tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(win), "port_entry"));
	snprintf(port, 5, "%d", opt.port);
	gtk_entry_set_text(GTK_ENTRY(tmp), port );
}

