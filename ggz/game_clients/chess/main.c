/*
 * File: main.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 09/17/2000
 * Desc: Chess client main game loop
 * $Id: main.c 2918 2001-12-17 10:11:39Z jdorje $
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


#include <gtk/gtk.h>

#include <easysock.h>
#include <ggzmod.h>

#include "main_win.h"
#include "support.h"
#include "chess.h"
#include "board.h"
#include "game.h"
#include "net.h"

/* main window widget */
GtkWidget *main_win;

/* Game info */
struct chess_info game_info;

int main(int argc, char *argv[]) {
	
	gtk_init(&argc, &argv);
  add_pixmap_directory(".");

	main_win = create_main_win();
	gtk_widget_show(main_win);

  game_info.state = CHESS_STATE_INIT;

  board_init();
  game_update(CHESS_EVENT_INIT, NULL);

	game_info.fd = ggzmod_connect();
	if (game_info.fd < 0)
		return -1;

	gdk_input_add(game_info.fd, GDK_INPUT_READ, net_handle_input, NULL);

	gtk_main();
	
	if (ggzmod_disconnect() < 0)
		return -2;

	return 0;
}
