/*
 * File: main_win.c
 * Author: Brent Hendricks
 * Project: GGZ Tic-Tac-Toe game module
 * Date: 3/31/00
 * Desc: Main window creation and callbacks
 * $Id: main_win.c 6248 2004-11-03 23:55:48Z jdorje $
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
#include <librsvg/rsvg.h>
#include <gtk/gtk.h>

#include "dlg_about.h"
#include "dlg_players.h"
#include "menus.h"
#include "main_win.h"
#include "game.h"
#include "support.h"


/* Pixmaps */
#define PIXSIZE 30
#define GRIDSIZE 60
#define BORDERSIZE 10

GdkPixbuf *x_pix, *o_pix;
GdkPixmap* ttt_buf;

GtkWidget *main_win;

/* Global game variables */
extern struct game_state_t game;


void game_status( const char* format, ... ) 
{
	int id;
	va_list ap;
	char* message;
	gpointer tmp;

	va_start( ap, format);
	message = g_strdup_vprintf(format, ap);
	va_end(ap);
	
	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "statusbar");
	
	id = gtk_statusbar_get_context_id( GTK_STATUSBAR(tmp), "Main" );
	
	gtk_statusbar_pop( GTK_STATUSBAR(tmp), id );
	gtk_statusbar_push( GTK_STATUSBAR(tmp), id, message );
	
	g_free( message );
	
}


void display_board(void)
{
	int i, x, y;
	GtkWidget* tmp;
	GdkPixbuf* piece;
	GtkStyle* style;

#if 0
	g_print("  %c | %c | %c  \n", game.board[0], game.board[1], game.board[2]);
	g_print("-------------\n");
	g_print("  %c | %c | %c  \n", game.board[3], game.board[4], game.board[5]);
	g_print("-------------\n");
	g_print("  %c | %c | %c  \n", game.board[6], game.board[7], game.board[8]);
	g_print("\n");
#endif

	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "drawingarea");
	style = gtk_widget_get_style(main_win);

	for (i = 0; i < 9; i++) {
		if (game.board[i] == 'x')
			piece = x_pix;
		else if (game.board[i] == 'o')
			piece = o_pix;
		else 
			continue;
		
		x = (i % 3) * GRIDSIZE + BORDERSIZE + (GRIDSIZE - PIXSIZE) / 2;
		y = (i / 3) * GRIDSIZE + BORDERSIZE + (GRIDSIZE - PIXSIZE) / 2;

		gdk_pixbuf_render_to_drawable(piece, ttt_buf,
					style->fg_gc[GTK_WIDGET_STATE(tmp)],
					      0, 0, x, y, PIXSIZE, PIXSIZE,
					      GDK_RGB_DITHER_NONE, 0, 0);
	}
	
	gtk_widget_draw(tmp, NULL);
}


static GdkPixbuf *load_pixmap(const char *name)
{
  char *fullpath = g_strdup_printf("%s/tictactoe/pixmaps/%s.svg",
				   GGZDATADIR, name);
  GdkPixbuf *image;
  GError *error = NULL;

  image = rsvg_pixbuf_from_file_at_size(fullpath, PIXSIZE, PIXSIZE, &error);
  if (image == NULL)
    ggz_error_msg_exit("Can't load pixmap %s", fullpath);
  g_free(fullpath);

  return image;
}

static void on_main_win_realize(GtkWidget* widget, gpointer user_data)
{
	GtkStyle* style;

	/* now for the pixmap from gdk */
	style = gtk_widget_get_style(main_win);

	x_pix = load_pixmap("x");
	o_pix = load_pixmap("o");
}


static gboolean main_exit(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	/* FIXME: should call an "are you sure dialog" */
	gtk_main_quit();
	
	return FALSE;
}


void game_resync(void)
{
	assert(FALSE);
}


void game_exit(void)
{
	/* FIXME: should call an "are you sure dialog" */
	gtk_main_quit();
}


static gboolean configure_handle(GtkWidget *widget, GdkEventConfigure *event, 
			         gpointer user_data)
{
	if (ttt_buf)
		gdk_pixmap_unref(ttt_buf);
	else {
		ttt_buf = gdk_pixmap_new( widget->window,
					  widget->allocation.width,
					  widget->allocation.height,
					  -1);
		gdk_draw_rectangle( ttt_buf,
				    widget->style->black_gc,
				    TRUE,
				    0, 0,
				    widget->allocation.width,
				    widget->allocation.height);
		
		gdk_draw_line(ttt_buf, 
			      widget->style->white_gc,
			      70, 10,
			      70, 190);

		gdk_draw_line(ttt_buf, 
			      widget->style->white_gc,
			      130, 10,
			      130, 190);

		gdk_draw_line(ttt_buf, 
			      widget->style->white_gc,
			      10, 70,
			      190, 70);

		gdk_draw_line(ttt_buf, 
			      widget->style->white_gc,
			      10, 130,
			      190, 130);

	}
	
	return TRUE;
}


static gboolean expose_handle(GtkWidget *widget, GdkEventExpose  *event, 
			      gpointer user_data)
{
	gdk_draw_pixmap( widget->window,
			 widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
			 ttt_buf,
			 event->area.x, event->area.y,
			 event->area.x, event->area.y,
			 event->area.width, event->area.height);
	
	return FALSE;
}


static gboolean get_move(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	int x = (int)(event->x);
	int y = (int)(event->y);
	int col = (x - BORDERSIZE) / GRIDSIZE;
	int row = (y - BORDERSIZE) / GRIDSIZE;

	if (game.state != STATE_MOVE) {
		if (game.num >= 0)
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


static GtkWidget *create_menus(GtkWidget *window)
{
	GtkWidget *menubar;
	GtkItemFactoryEntry items[] = {
		TABLE_MENU,
		HELP_MENU
	};

	menubar = ggz_create_menus(window,
				   items,
				   sizeof(items) / sizeof(items[0]));

	set_menu_sensitive(TABLE_SYNC, FALSE);
	set_menu_sensitive(TABLE_CHAT_WINDOW, TRUE);

	return menubar;
}


GtkWidget*
create_main_win (void)
{
  GtkWidget *main_win;
  GtkWidget *main_box;
  GtkWidget *menubar;
  GtkWidget *drawingarea;
  GtkWidget *statusbar;
  GtkWidget *chat;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  main_win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (main_win), "main_win", main_win);
  gtk_window_set_title (GTK_WINDOW (main_win), _("Tic-Tac-Toe"));

  /* TTT will crash if resized. */
  gtk_window_set_resizable(GTK_WINDOW(main_win), FALSE);

  main_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (main_box);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "main_box", main_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (main_box);
  gtk_container_add (GTK_CONTAINER (main_win), main_box);

  menubar = create_menus(main_win);
  gtk_widget_ref (menubar);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "menubar", menubar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (menubar);
  gtk_box_pack_start (GTK_BOX (main_box), menubar, FALSE, FALSE, 0);

  drawingarea = gtk_drawing_area_new ();
  gtk_widget_ref (drawingarea);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "drawingarea", drawingarea,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (drawingarea);
  gtk_box_pack_start (GTK_BOX (main_box), drawingarea, TRUE, TRUE, 0);
  gtk_widget_set_usize (drawingarea, 200, 200);
  gtk_widget_set_events (drawingarea, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);

  statusbar = gtk_statusbar_new ();
  gtk_widget_ref (statusbar);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "statusbar", statusbar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (statusbar);
  gtk_box_pack_start (GTK_BOX (main_box), statusbar, FALSE, FALSE, 0);

  chat = create_chat_widget();
  gtk_widget_ref(chat);
  gtk_object_set_data_full(GTK_OBJECT(main_win), "chat", chat,
			   (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(chat);
  gtk_box_pack_start(GTK_BOX(main_box), chat, FALSE, FALSE, 0);

  gtk_signal_connect (GTK_OBJECT (main_win), "delete_event",
                      GTK_SIGNAL_FUNC (main_exit),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (main_win), "realize",
                      GTK_SIGNAL_FUNC (on_main_win_realize),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (drawingarea), "configure_event",
                      GTK_SIGNAL_FUNC (configure_handle),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (drawingarea), "expose_event",
                      GTK_SIGNAL_FUNC (expose_handle),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (drawingarea), "button_press_event",
                      GTK_SIGNAL_FUNC (get_move),
                      NULL);

  gtk_window_add_accel_group (GTK_WINDOW (main_win), accel_group);

  return main_win;
}

