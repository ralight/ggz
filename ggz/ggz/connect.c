/*
 * File: connect.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 1/23/99
 *
 * This fils contains functions for connecting with the server
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

#include <sys/types.h>
#include <sys/socket.h>
#include <gtk/gtk.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "connect.h"
#include "dlg_error.h"
#include "client.h"
#include "easysock.h"
#include "protocols.h"
#include "options.h"
#include "err_func.h"

/* Global state of game variable */
extern Options opt;
extern GtkWidget* detail_window;
extern GtkWidget* main_win;

/* Various local handles */
static guint sock_handle;
static void connect_msg( const char *, ... );
static void add_user_list( gchar* name, gint table);
static int anon_login( void );
static void handle_server_fd(gpointer, gint, GdkInputCondition);

GtkWidget* tmpWidget;

#ifdef DEBUG
char* user_msg[18] = { "MSG_SERVER_ID", 
		       "MSG_SERVER_FULL",
		       "RSP_NEW_LOGIN",
		       "RSP_LOGIN",
		       "RSP_ANON_LOGIN",
		       "RSP_MOTD",
		       "RSP_LOGOUT",
		       "RSP_USER_LIST",
		       "RSP_PREF_CHANGE",
		       "RSP_REMOVE_USER",
		       "RSP_GAME_TYPES",
		       "RSP_TABLE_LIST",
		       "RSP_TABLE_OPTIONS",
		       "RSP_LAUNCH_GAME",
		       "RSP_JOIN_GAME",
		       "RSP_USER_STAT",
		       "RSP_CHAT",
		       "RSP_ERROR" };
#endif


/**
 * Begin connection to server.  
 */
int connect_to_server(void) 
{
	if (FAIL(opt.sock = es_make_socket(ES_CLIENT, opt.port, opt.server)))
		return -1;
	
	sock_handle = gdk_input_add_full(opt.sock, GDK_INPUT_READ, 
					 handle_server_fd, NULL, NULL);
	
	return 0;
}


/*
 * Disconnect from server, close sockets, etc.
 */
void Disconnect( GtkWidget* widget, gpointer data ) 
{
	if (sock_handle)
		gdk_input_remove( sock_handle );
	sock_handle = 0;
	
	/* Close out network connection */
	NetClose();
}


/**
 * Read data from the Spades server 
 */
void handle_server_fd(gpointer data, gint source, GdkInputCondition cond) {

  char *message;
  int op, size, status;
  char byte;
  int checksum, ibyte;
  char buf[4096];
  int count, i;

  if (opt.playing) {
	  size = read(source, buf, 4096);
	  status = es_writen(opt.game_fd, buf, size);
	  if (status <= 0) { /* Game over */
		  dbg_msg("Game is over (msg from server)");
		  opt.playing = 0;
		  close(opt.game_fd);
	  }
	  return;
  }

  CheckReadInt(source, (int*)&op);
  
  switch (op) {
  case MSG_SERVER_ID:
	  CheckReadString( source, &message );
	  connect_msg("[MSG_SERVER_ID] %s\n", message);
	  switch (opt.login_type) {
	  case 0: /*Normal login */
	  case 2: /*First time login */
	  case 1: /*Anonymous login */
		  anon_login();
	  }
	  break;
	  
  case RSP_ANON_LOGIN:
	  read(source, &byte, 1);
	  connect_msg("[RSP_ANON_LOGIN] %d\n", byte);
	  if (byte < 0) {
		  Disconnect(NULL, NULL);
		  return;
	  }
	  CheckReadInt(source, &checksum );
	  connect_msg("[RSP_ANON_LOGIN] Checksum = %d\n", checksum);
	  tmpWidget = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(main_win),"player_list"));
	  gtk_clist_clear (GTK_CLIST (tmpWidget));
	  CheckWriteInt(opt.sock, REQ_USER_LIST);  
	  break;
	  
  case RSP_LAUNCH_GAME:
	  read(source, &byte, 1);
	  connect_msg("[RSP_LAUNCH_GAME] %d\n", byte);
	  if (byte < 0) {
		  Disconnect(NULL, NULL);
		  return;
	  }
	  opt.playing = 1;
	  break;
	  
  case RSP_JOIN_GAME:
	  read(source, &byte, 1);
	  connect_msg("[RSP_JOIN_GAME] %d\n", byte);
	  if (byte < 0) {
		  Disconnect(NULL, NULL);
		  return;
	  }
	  opt.playing = 1;
	  break;
	  
  case RSP_LOGOUT:
	  read(source, &byte, 1);
	  connect_msg("[RSP_LOGOUT] %d\n", byte);
	  Disconnect(NULL,NULL);
	  break;
	  
  case RSP_GAME_TYPES:
	  connect_msg("[RSP_GAME_TYPES] %d\n", byte);
	  break;
  case RSP_TABLE_LIST:
	  connect_msg("[RSP_TABLE_LIST] %d\n", byte);
	  break;
  case RSP_USER_LIST:
	  CheckReadInt(source, &ibyte );
	  count=ibyte;
	  connect_msg("[RSP_USER_LIST] User List Count %d\n", count);
	  for (i=1;i<=count;i++){
		  CheckReadString( source, &message );
		  connect_msg("[RSP_USER_LIST] User %s\n", message);
		  CheckReadInt(source, &ibyte );
		  connect_msg("[RSP_USER_LIST] Table %d\n", ibyte);
		  add_user_list(message, ibyte);
	  }
	  break;
	  
  case MSG_SERVER_FULL:
  case RSP_NEW_LOGIN:
  case RSP_LOGIN:
	  connect_msg("[RSP_LOGIN] %d\n", byte);
	  CheckWriteInt(opt.sock, REQ_USER_LIST);  
	  break;
  case RSP_MOTD:
  case RSP_PREF_CHANGE:
  case RSP_REMOVE_USER:
  case RSP_TABLE_OPTIONS:
  case RSP_USER_STAT:
  case RSP_ERROR:
  }
  
}



void connect_msg( const char* format, ... ) {

  va_list ap;
  char* message;
  
  if (detail_window == NULL)
    return;

  va_start( ap, format);
  message = g_strdup_vprintf(format, ap);
  va_end(ap);
  
  tmpWidget = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(detail_window),"text"));
  gtk_text_insert( GTK_TEXT(tmpWidget), NULL, NULL, NULL, message, -1 );
  
  g_free( message );

  
}


void add_user_list( gchar* name, gint table) {

  gchar *entry[2];
  if (main_win == NULL)
    return;

  tmpWidget = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(main_win),"player_list"));

  entry[0] = name;
  if (table == -1){
    entry[1] = "none";
  }else{
    entry[1] = g_strdup_printf("%d",table);
  }

  gtk_clist_append (GTK_CLIST (tmpWidget), entry);

  if (table == -1){
  }else{
    g_free (entry[1]);
  }
}


int anon_login( void ) {
  
  CheckWriteInt(opt.sock, REQ_ANON_LOGIN);
  CheckWriteString(opt.sock, opt.user_name);

  return 0;
}









