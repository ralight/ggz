/*
 * File: game.c
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 2/18/00
 *
 * This fils contains functions for handling individual game client
 * programs
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

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <easysock.h>

#include "game.h"
#include "dlg_error.h"
#include "dlg_launch.h"
#include "datatypes.h"
#include "protocols.h"
#include "err_func.h"
#include "seats.h"
#include "ggzrc.h"

/* Global data structures */
extern GtkWidget *dlg_launch;
extern struct ConnectInfo client;
extern struct Game game;
extern struct GameTypes game_types;
extern GtkWidget *main_win;
extern GtkWidget *mnu_tables;
extern struct Rooms room_info;

/* Local data */
static guint game_handle;

static void run_game(gint type, gchar flag, gchar* path);
static void handle_game(gpointer data, gint source, GdkInputCondition cond);


void launch_game(gint type, gchar launch)
{
	pid_t pid;
	gint sock, fd;
	gchar* game_name;
	gchar* path;
	gchar* full_path;

	if (launch)
		dbg_msg("Launching game");
	else
		dbg_msg("Joining game");
	
	game_name = g_strdup(game_types.info[type].name);
	if ( (path = ggzrc_read_string(game_name, "Path", NULL)) == NULL)
		return;
	
	dbg_msg("path from config file is : %s", path);
	
	if (g_path_is_absolute(path))
		full_path = path;
	else {
		full_path = g_strdup_printf("%s/%s", GAMEDIR, path);
		g_free(path);
	}
	dbg_msg("full game module path is : %s", full_path);
	
	/* Fork table process */
	if ( (pid = fork()) < 0) {
		err_dlg("fork failed");
		return;
	} else if (pid == 0) {
		run_game(type, launch, full_path);
		err_dlg("exec failed");
		return;
	} else {
		/* Create Unix doamin socket for communication*/
		game.fd_name = g_strdup_printf("/tmp/%s.%d", game_name, pid);
		sock = es_make_unix_socket(ES_SERVER, game.fd_name);
		/* FIXME: need to check validity of fd */

		if (listen(sock, 1) < 0)
			err_sys_exit("listen falied");

		if ( (fd = accept(sock, NULL, NULL)) < 0)
			err_sys_exit("accpet failed");
		
		/* Key info about game */
		game.type = type;
		game.pid = pid;
		game.fd = fd;

		game_handle = gdk_input_add(fd, GDK_INPUT_READ, handle_game, 
					    NULL);
					    
	}

	g_free(game_name);
	g_free(full_path);
}


static void run_game(gint type, gchar flag, gchar* path)
{
	dbg_msg("Process forked.  Game running");

	/* FIXME: Maybe pass over sock, rather than cmd-line? */
	if (flag)
		execl(path, g_basename(path), "-o", NULL);
	else
		execl(path, g_basename(path), NULL);
}


static void handle_game(gpointer data, gint source, GdkInputCondition cond)
{
	gint size, status;
	gchar buf[4096];

	dbg_msg("Got game msg from game client");

	size = read(source, buf, 4096);
	dbg_msg("Client sent %d bytes", size);
	es_write_int(client.sock, REQ_GAME);
	es_write_int(client.sock, size);
	status = es_writen(client.sock, buf, size);

	if (status <= 0) {	/* Game over */
		game_over();
	}
}


int game_over(void)
{
	GtkWidget *tmp;

	dbg_msg("Game is over (msg from client)");
	client.playing = FALSE;
	close(game.fd);
	if (game_handle) {
		gdk_input_remove(game_handle);
		game_handle = 0;
	}
	if (game.pid)
		kill(game.pid, SIGINT);
	unlink(game.fd_name);
			
	/* FIXME: should be in a function based on state somewhere */
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "launch");
        gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "join");
        gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "launch_button");
        gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "join_button");
        gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(mnu_tables), "launch");
        gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(mnu_tables), "join1");
        gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "room_combo");
        gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);

	return 0;
}


RETSIGTYPE game_dead(int sig)
{
  	int  pid, status;
  
	pid = waitpid(WAIT_ANY, &status, WNOHANG);
	if( pid < 0 ) {
		err_sys("waitpid");
	}
	else if( pid == 0 ) {
		err_sys("No dead child");
	}
	else if( pid == game.pid ) {
		game.pid = 0;
		dbg_msg("Game module is dead");
	}
}

