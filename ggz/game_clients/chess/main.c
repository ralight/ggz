/*
 * File: main.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 09/17/2000
 * Desc: Chess client main game loop
 *
 * Copyright (C) 2001 Ismael Orenstein.
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


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <easysock.h>
#include <gtk/gtk.h>
#include "main_win.h"

/* main window widget */
GtkWidget *main_win;

int main(int argc, char *argv[]) {
	
	gtk_init(&argc, &argv);
  add_pixmap_directory(".");

	main_win = create_main_win();
	gtk_widget_show(main_win);

	//ggz_connect("Chess");

	//gdk_input_add(game.fd, GDK_INPUT_READ, game_handle_io, NULL);

	gtk_main();
	
	return 0;
}

int ggz_connect(char *name) {
	char fd_name[30];
  int fd;
	struct sockaddr_un addr;
						 
	/* Connect to Unix domain socket */
	sprintf(fd_name, "/tmp/%s.%d", name, getpid());
		 
	if ( (fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0)
		exit(-1);
		 
	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, fd_name);
					 
	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		exit(-1);

  return fd;
}    
