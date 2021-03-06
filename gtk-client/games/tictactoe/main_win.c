/*
 * File: main_win.c
 * Author: Brent Hendricks
 * Project: GGZ Tic-Tac-Toe game module
 * Date: 3/31/00
 * Desc: Main window creation and callbacks
 * $Id: main_win.c 10239 2008-07-08 20:04:55Z jdorje $
 *
 * Copyright (C) 2000 Brent Hendricks.
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

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "dlg_about.h"
#include "dlg_players.h"
#include "dlg_exit.h"
#include "game.h"
#include "menus.h"
#include "main_win.h"
#include "net.h"
#include "support.h"


/* Pixmaps */
static int PIXSIZE, GRIDSIZE, BORDERSIZE;

GdkPixbuf *x_pix, *o_pix;
GdkPixmap *ttt_buf;

GtkWidget *main_win;

/* Global game variables */
extern struct game_state_t game;


void game_status(const char *format, ...)
{
	int id;
	va_list ap;
	char *message;
	gpointer tmp;

	va_start(ap, format);
	message = g_strdup_vprintf(format, ap);
	va_end(ap);

	tmp = g_object_get_data(G_OBJECT(main_win), "statusbar");

	id = gtk_statusbar_get_context_id(GTK_STATUSBAR(tmp), "Main");

	gtk_statusbar_pop(GTK_STATUSBAR(tmp), id);
	gtk_statusbar_push(GTK_STATUSBAR(tmp), id, message);

	g_free(message);

}


void display_board(void)
{
	int i, x, y;
	GdkPixbuf *piece;
	GtkStyle *style;
	GtkWidget *tmp = g_object_get_data(G_OBJECT(main_win),
					   "drawingarea");
	int w = tmp->allocation.width, h = tmp->allocation.height;

	ggz_debug(DBG_BOARD,
		  "  %c | %c | %c  ", game.board[0], game.board[1],
		  game.board[2]);
	ggz_debug(DBG_BOARD, "-------------");
	ggz_debug(DBG_BOARD,
		  "  %c | %c | %c  ", game.board[3], game.board[4],
		  game.board[5]);
	ggz_debug(DBG_BOARD, "-------------");
	ggz_debug(DBG_BOARD,
		  "  %c | %c | %c  ", game.board[6], game.board[7],
		  game.board[8]);

	tmp = g_object_get_data(G_OBJECT(main_win), "drawingarea");
	style = gtk_widget_get_style(main_win);

	gdk_draw_rectangle(ttt_buf,
			   tmp->style->black_gc, TRUE, 0, 0, w, h);

	gdk_draw_line(ttt_buf,
		      tmp->style->white_gc,
		      BORDERSIZE + GRIDSIZE, BORDERSIZE,
		      BORDERSIZE + GRIDSIZE, BORDERSIZE + 3 * GRIDSIZE);

	gdk_draw_line(ttt_buf,
		      tmp->style->white_gc,
		      BORDERSIZE + 2 * GRIDSIZE, BORDERSIZE,
		      BORDERSIZE + 2 * GRIDSIZE,
		      BORDERSIZE + 3 * GRIDSIZE);

	gdk_draw_line(ttt_buf,
		      tmp->style->white_gc,
		      BORDERSIZE, BORDERSIZE + GRIDSIZE,
		      BORDERSIZE + 3 * GRIDSIZE, BORDERSIZE + GRIDSIZE);

	gdk_draw_line(ttt_buf,
		      tmp->style->white_gc,
		      BORDERSIZE, BORDERSIZE + 2 * GRIDSIZE,
		      BORDERSIZE + 3 * GRIDSIZE,
		      BORDERSIZE + 2 * GRIDSIZE);

	for (i = 0; i < 9; i++) {
		if (game.board[i] == 'x')
			piece = x_pix;
		else if (game.board[i] == 'o')
			piece = o_pix;
		else
			continue;

		x = (i % 3) * GRIDSIZE + BORDERSIZE + (GRIDSIZE -
						       PIXSIZE) / 2;
		y = (i / 3) * GRIDSIZE + BORDERSIZE + (GRIDSIZE -
						       PIXSIZE) / 2;

		gdk_draw_pixbuf(ttt_buf,
				style->
				fg_gc[GTK_WIDGET_STATE(tmp)],
				piece,
				0, 0, x, y, PIXSIZE, PIXSIZE,
				GDK_RGB_DITHER_NONE, 0, 0);
	}

	gtk_widget_queue_draw(tmp);
}


static GdkPixbuf *load_pixmap(const char *name)
{
	char *fullpath = g_strdup_printf("%s/tictactoe/pixmaps/%s.svg",
					 GGZDATADIR, name);
	GdkPixbuf *image;
	GError *error = NULL;

	image = gdk_pixbuf_new_from_file_at_size(fullpath, PIXSIZE, PIXSIZE,
						 &error);
	if (image == NULL)
		ggz_error_msg_exit("Can't load pixmap %s", fullpath);
	g_free(fullpath);

	return image;
}

static void window_resized(void)
{
	GtkWidget *widget = g_object_get_data(G_OBJECT(main_win),
					      "drawingarea");
	int w = widget->allocation.width, h = widget->allocation.height;

	if (ttt_buf) {
		g_object_unref(ttt_buf);
		g_object_unref(x_pix);
		g_object_unref(o_pix);
	}

	gtk_widget_realize(widget);	/* HACK: must be realized before used. */
	ttt_buf = gdk_pixmap_new(widget->window, w, h, -1);
	gdk_draw_rectangle(ttt_buf,
			   widget->style->black_gc, TRUE, 0, 0, w, h);


	BORDERSIZE = MIN(w, h) / 20;
	GRIDSIZE = (MIN(w, h) - 2 * BORDERSIZE) / 3;
	PIXSIZE = GRIDSIZE / 2;

	x_pix = load_pixmap("x");
	o_pix = load_pixmap("o");

	display_board();
}

static void on_main_win_realize(GtkWidget * widget, gpointer user_data)
{
	window_resized();
}


static gboolean main_exit(GtkWidget * widget, GdkEvent * event,
			  gpointer user_data)
{
	/* FIXME: should call an "are you sure dialog" */
	gtk_main_quit();

	return FALSE;
}


void game_resync(void)
{
	ggzcomm_reqsync();
}


void game_exit(void)
{
	ggz_show_exit_dialog(TRUE);
}

static gboolean configure_handle(GtkWidget * widget,
				 GdkEventConfigure * event,
				 gpointer user_data)
{
	window_resized();
	return TRUE;
}


static gboolean expose_handle(GtkWidget * widget, GdkEventExpose * event,
			      gpointer user_data)
{
	gdk_draw_drawable(widget->window,
			  widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
			  ttt_buf,
			  event->area.x, event->area.y,
			  event->area.x, event->area.y,
			  event->area.width, event->area.height);

	return FALSE;
}


static gboolean get_move(GtkWidget * widget, GdkEventButton * event,
			 gpointer user_data)
{
	int x = (int)(event->x);
	int y = (int)(event->y);
	int col = (x - BORDERSIZE) / GRIDSIZE;
	int row = (y - BORDERSIZE) / GRIDSIZE;

	int is_spectator, seat_num;

	ggzmod_get_player(game.ggzmod, &is_spectator, &seat_num);

	if (game.state != STATE_MOVE) {
		if (!is_spectator)
			/* FIXME 1: why do we track state when ggzmod can do it? */
			/* FIXME 2: notwithstanding the PLAY_AGAIN feature, a TTT_ERR_OVER message could be useful */
			if(game.state == STATE_DONE)
				game_status(_("The game is over."));
			else
				game_status(_("It's not your move yet."));
		else
			game_status(_("You're just watching."));
		return TRUE;
	}

	if (event->button == 1 && ttt_buf != NULL) {
		if (col == 3)
			col = 2;
		if (row == 3)
			row = 2;

		game.move = row * 3 + col;
		send_my_move();
	}

	return TRUE;
}


static GtkWidget *create_menus(GtkWidget * window)
{
	GtkWidget *menubar;
	GtkItemFactoryEntry items[] = {
		TABLE_MENU,
		HELP_MENU
	};

	menubar = ggz_create_menus(window,
				   items,
				   sizeof(items) / sizeof(items[0]));

	set_menu_sensitive(TABLE_SYNC, TRUE);
	set_menu_sensitive(TABLE_CHAT_WINDOW, TRUE);

	return menubar;
}


GtkWidget *create_main_win(void)
{
	GtkWidget *win_main;
	GtkWidget *main_box;
	GtkWidget *menubar;
	GtkWidget *drawingarea;
	GtkWidget *statusbar;
	GtkWidget *chat;
	GtkAccelGroup *accel_group;

	accel_group = gtk_accel_group_new();

	win_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_object_set_data(G_OBJECT(win_main), "main_win", win_main);
	gtk_window_set_title(GTK_WINDOW(win_main), _("Tic-Tac-Toe"));

	main_box = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(main_box);
	g_object_set_data_full(G_OBJECT(win_main), "main_box", main_box,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(main_box);
	gtk_container_add(GTK_CONTAINER(win_main), main_box);

	menubar = create_menus(win_main);
	gtk_widget_ref(menubar);
	g_object_set_data_full(G_OBJECT(win_main), "menubar", menubar,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(menubar);
	gtk_box_pack_start(GTK_BOX(main_box), menubar, FALSE, FALSE, 0);

	drawingarea = gtk_drawing_area_new();
	gtk_widget_ref(drawingarea);
	g_object_set_data_full(G_OBJECT(win_main), "drawingarea",
			       drawingarea,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(drawingarea);
	gtk_box_pack_start(GTK_BOX(main_box), drawingarea, TRUE, TRUE, 0);
	gtk_widget_set_size_request(drawingarea, 200, 200);
	gtk_widget_set_events(drawingarea,
			      GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);

	statusbar = gtk_statusbar_new();
	gtk_widget_ref(statusbar);
	g_object_set_data_full(G_OBJECT(win_main), "statusbar", statusbar,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(statusbar);
	gtk_box_pack_start(GTK_BOX(main_box), statusbar, FALSE, FALSE, 0);

	chat = create_chat_widget();
	gtk_widget_ref(chat);
	g_object_set_data_full(G_OBJECT(win_main), "chat", chat,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(chat);
	gtk_box_pack_start(GTK_BOX(main_box), chat, FALSE, FALSE, 0);

	g_signal_connect(GTK_OBJECT(win_main), "delete_event",
			 GTK_SIGNAL_FUNC(main_exit), NULL);
	g_signal_connect(GTK_OBJECT(win_main), "realize",
			 GTK_SIGNAL_FUNC(on_main_win_realize), NULL);
	g_signal_connect(GTK_OBJECT(drawingarea), "configure_event",
			 GTK_SIGNAL_FUNC(configure_handle), NULL);
	g_signal_connect(GTK_OBJECT(drawingarea), "expose_event",
			 GTK_SIGNAL_FUNC(expose_handle), NULL);
	g_signal_connect(GTK_OBJECT(drawingarea), "button_press_event",
			 GTK_SIGNAL_FUNC(get_move), NULL);

	gtk_window_add_accel_group(GTK_WINDOW(win_main), accel_group);

	return win_main;
}
