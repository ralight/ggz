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
#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <easysock.h>

#include "game.h"
#include "dlg_error.h"
#include "datatypes.h"
#include "protocols.h"
#include "err_func.h"


/* Global data structures */
extern struct ConnectInfo connection;
extern struct Game game;

/* Local data */
static guint game_handle;

static void run_game(int type, char flag, int fd);
static void handle_game(gpointer data, gint source, GdkInputCondition cond);
static void handle_options(gpointer data, gint source, GdkInputCondition cond);

void launch_game(int type, char launch)
{
	pid_t pid;
	int fd[2];
	GdkInputFunction callback;

	/* Create socketpair for communication */
	socketpair(PF_UNIX, SOCK_STREAM, 0, fd);

	if (launch)
		dbg_msg("Launching game");
	else
		dbg_msg("Joining game");
	
	/* Fork table process */
	if ((pid = fork()) < 0) {
		err_dlg("fork failed");
		return;
	} else if (pid == 0) {
		run_game(type, launch, fd[0]);
		err_dlg("exec failed");
		return;
	} else {
		/* Close the remote ends of the socket pairs */
		close(fd[0]);
		game.pid = pid;
		game.fd = fd[1];
		if (launch)
			callback = handle_options;
		else
			callback = handle_game;

		game_handle = gdk_input_add(fd[1], GDK_INPUT_READ, 
					    *callback, NULL);
	}
}


static void run_game(int type, char flag, int fd)
{
	dbg_msg("Process forked.  Game running");

	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	close(fd);

	/* FIXME: Don't hardcode client game */
	/* FIXME: Maybe pass through pipe, rather than cmd-line? */
	if (flag)
		execl("../game_clients/spades/gspades", "gspades", "-o",
		      NULL);
	else
		execl("../game_clients/spades/gspades", "gspades", NULL);
}


static void handle_options(gpointer data, gint source, GdkInputCondition cond)
{
	int size;
	char ai;
	void *options;

	dbg_msg("Getting options from game client");

	es_read_int(source, &size);
	options = malloc(size);
	/*FIXME: check for failed malloc */
	es_readn(source, options, size);
	es_read_char(source, &ai);

	/* Send launch game request to server */
	es_write_int(connection.sock, REQ_TABLE_LAUNCH);
	es_write_int(connection.sock, 0);	/* Game type index */
	es_write_int(connection.sock, 4);	/* Number of seats */
	es_write_char(connection.sock, ai);	/* AI players */
	es_write_int(connection.sock, 0);	/* Number of reservations */
	es_write_int(connection.sock, size);
	es_writen(connection.sock, options, size);
	free(options);

	gdk_input_remove(game_handle);
	game_handle = gdk_input_add(source, GDK_INPUT_READ, handle_game,
				    NULL);
}


static void handle_game(gpointer data, gint source, GdkInputCondition cond)
{
	int size, status;
	char buf[4096];

	dbg_msg("Got game msg from game client");

	size = read(source, buf, 4096);
	dbg_msg("Client sent %d bytes", size);
	es_write_int(connection.sock, REQ_GAME);
	es_write_int(connection.sock, size);
	status = es_writen(connection.sock, buf, size);

	if (status <= 0) {	/* Game over */
		dbg_msg("Game is over (msg from client)");
		connection.playing = FALSE;
		close(source);
		gdk_input_remove(game_handle);
		/* FIXME: also kill game process ?*/
	}
}
