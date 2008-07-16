/*
 * File: callbacks.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Desc: GTK Callback functions
 * $Id: callbacks.c 10227 2008-07-08 18:34:30Z jdorje $
 *
 * Copyright (C) 2000-2002 Ismael Orenstein.
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <stdio.h>

#include "dlg_exit.h"

#include "callbacks.h"
#include "chess.h"
#include "board.h"
#include "game.h"
#include "libcgc/cgc.h"
#include "main_win.h"
#include "support.h"
#include "popup.h"

extern game_t *game;

extern struct chess_info game_info;

extern GtkWidget *main_win;

extern GdkPixmap *board_buf;

extern GtkTargetEntry *target;


static void try_to_quit(void)
{
	if (game_info.state == CHESS_STATE_PLAYING)
		ggz_show_exit_dialog(FALSE);
	else
		gtk_main_quit();
}

gboolean
ExitDialog(GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
	try_to_quit();
	return TRUE;
}

void on_exit_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	try_to_quit();
}


gboolean on_board_configure_event(GtkWidget * widget,
				  GdkEventConfigure * event,
				  gpointer user_data)
{
	board_resized(event->width, event->height);

	if (board_buf) {
		g_object_unref(board_buf);
	}
	board_buf =
	    gdk_pixmap_new(widget->window, PIXSIZE * 8, PIXSIZE * 8, -1);

	board_draw();


	return FALSE;
}


gboolean on_board_expose_event(GtkWidget * widget, GdkEventExpose * event,
			       gpointer user_data)
{
	gdk_draw_drawable(widget->window,
			  widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
			  board_buf,
			  event->area.x, event->area.y,
			  event->area.x, event->area.y,
			  event->area.width, event->area.height);

	return FALSE;
}


gboolean
on_board_button_press_event(GtkWidget * widget,
			    GdkEventButton * event, gpointer user_data)
{
	int x, y;
	char output[80];
	cgc_getboard(output, game->board);
	x = event->x / PIXSIZE;
	y = event->y / PIXSIZE;
	/* Set the last pressed data */
	g_object_set_data(G_OBJECT(widget), "from_x", GINT_TO_POINTER(x));
	g_object_set_data(G_OBJECT(widget), "from_y", GINT_TO_POINTER(y));

	if (output[x + (y * 9)] == '-'
	    || (game_info.seat == 1 && output[x + (y * 9)] < 'Z')
	    || (game_info.seat == 0 && !(output[x + (y * 9)] < 'Z')))
		gtk_drag_source_unset(widget);
	else
		gtk_drag_source_set(widget, GDK_BUTTON1_MASK, target, 1,
				    GDK_ACTION_MOVE);


	return FALSE;
}

void
on_board_drag_begin(GtkWidget * widget,
		    GdkDragContext * drag_context, gpointer user_data)
{
	int x =
	    GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "from_x"));
	int y =
	    GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "from_y"));

	board_dnd_highlight(x, y, drag_context);
}

gboolean
on_board_drag_motion(GtkWidget * widget,
		     GdkDragContext * drag_context,
		     gint x, gint y, guint timer, gpointer user_data)
{
	int f_x =
	    GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "from_x"));
	int f_y =
	    GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "from_y"));
	int t_x = x / PIXSIZE;
	int t_y = y / PIXSIZE;
	int retval = cgc_valid_move(game, f_x, 7 - f_y, t_x, 7 - t_y, 0);

	if (retval == VALID) {
		game_info.dest_x = t_x;
		game_info.dest_y = t_y;
	} else {
		game_info.dest_x = -1;
		game_info.dest_y = -1;
	}
	board_draw();

	return FALSE;
}


gboolean
on_board_drag_drop(GtkWidget * widget,
		   GdkDragContext * drag_context,
		   gint x, gint y, guint timer, gpointer user_data)
{
	int arg[4];
	char move[6];
	char output[80];
	int promote = 0;
	cgc_getboard(output, game->board);

	arg[0] =
	    GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "from_x"));
	arg[1] =
	    GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "from_y"));

	arg[2] = x / PIXSIZE;
	arg[3] = y / PIXSIZE;

	/* Should we promote ? */
	if (game_info.seat == 0) {
		/* We are white ! So we must go from 6 to 7 */
		if (arg[1] == 6 && arg[3] == 7
		    && output[arg[0] + (arg[1] * 10)] == 'P')
			promote = 1;
	} else {
		/* We are black ! So we must go from 1 to 0 */
		if (arg[1] == 1 && arg[3] == 0
		    && output[arg[0] * (arg[1] * 10)] == 'p')
			promote = 1;
	}

	move[0] = 65 + arg[0];
	move[1] = 49 + (7 - arg[1]);
	move[2] = 65 + arg[2];
	move[3] = 49 + (7 - arg[3]);
	move[4] = promote;
	move[5] = 0;

	if (!promote
	    && cgc_valid_move(game, arg[0], 7 - arg[1], arg[2], 7 - arg[3],
			      0) == VALID) {
		game_update(CHESS_EVENT_MOVE_END, move);
	} else if (promote) {
		g_object_set_data(G_OBJECT(main_win), "promote",
				  g_strdup(move));
		gtk_widget_show(create_promote_dialog());
	}

	gtk_drag_source_unset(widget);

	game_info.src_x = -1;
	game_info.src_y = -1;
	game_info.dest_x = -1;
	game_info.dest_y = -1;

	board_draw();


	return FALSE;
}

void on_auto_call_flag_activate(GtkMenuItem * menuitem, gpointer user_data)
{
}
