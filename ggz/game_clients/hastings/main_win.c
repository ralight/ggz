/*
 * File: main_win.c
 * Author: Josef Spillner
 * Original Author: Brent Hendricks
 * Project: GGZ Hastings1066 game module
 * Date: 09/13/00
 * Desc: Main window creation and callbacks
 * $Id: main_win.c 6245 2004-11-03 22:00:55Z jdorje $
 *
 * Copyright (C) 2000 - 2002 Josef Spillner
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

/* Configuration file */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

/* System includes */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

/* Gtk+ files */
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

/* Gtk-games files */
#include "dlg_exit.h"
#include "dlg_players.h"
#include "menus.h"

/* Hastings files */
#include "main_win.h"
#include "game.h"
#include "support.h"
#include "dlg_about.h"
#include "dlg_yesno.h"
#include "ggzintl.h"

/* GGZ includes */
#include <ggz.h>	/* libggz */

/* #define USE_SHADOW */

/* Unit pictures */
/* Red is 0, blue is 1, green is 2, yellow is 3.
   This corresponds to player_colors[] in main.c! */
GdkPixbuf *man_pix[4];
GdkPixbuf* frame_ll_pix;
GdkPixbuf* frame_lr_pix;
GdkPixbuf* frame_ul_pix;
GdkPixbuf* frame_ur_pix;
GdkPixbuf* frame_black_pix;
GdkPixbuf* shadow_pix;

/* Map picture */
GdkPixbuf* map_pix;

/* Buffer for the map */
GdkPixmap* hastings_buf;

/* Main window */
GtkWidget* main_win;

/* Global game variables */
extern struct game_state_t game;

const char* player_colors[] = {N_("red"), N_("blue"),
                               N_("green"), N_("yellow")};
const char* team_colors[] = {N_("white"), N_("black")};


/* Display the game's current status on status bar and console */
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

	id = gtk_statusbar_get_context_id(GTK_STATUSBAR(tmp), "Main");

	gtk_statusbar_pop(GTK_STATUSBAR(tmp), id);
	gtk_statusbar_push(GTK_STATUSBAR(tmp), id, message);

	ggz_debug("debug", "STATUS: %s", message);

	g_free(message);

}

static void draw(GdkPixbuf *image, int x, int y, int w, int h)
{
	GtkStyle *style = gtk_widget_get_style(main_win);
	GtkWidget *tmp = gtk_object_get_data(GTK_OBJECT(main_win), "drawingarea");
	GdkGC *gc = style->fg_gc[GTK_WIDGET_STATE(tmp)];

	gdk_pixbuf_render_to_drawable(image, hastings_buf, gc,
				      0, 0, x, y, w, h,
				      GDK_RGB_DITHER_NONE, 0, 0);
}

/* Draw a frame around a knight */
static void highlight(int col, int row, int widgetstate)
{
	int offsetx = 75 - row % 2 * 45 + col * 90;
	int offsety = 30 + row * 25;

	draw(frame_ul_pix, offsetx - 16, offsety - 16, 5, 5);
	draw(frame_ur_pix, offsetx + 11, offsety - 16, 5, 5);
	draw(frame_ll_pix, offsetx - 16, offsety + 11, 5, 5);
	draw(frame_lr_pix, offsetx + 11, offsety + 11, 5, 5);
}

#ifdef USE_SHADOW
/* Mark own knights with shadow */
static void shadow(int col, int row, int widgetstate)
{
	GtkStyle *style;
	int offsetx, offsety;

	style = gtk_widget_get_style(main_win);
	offsetx = 75 - row % 2 * 45 + col * 90;
	offsety = 30 + row * 25;

	gdk_draw_pixmap(hastings_buf, style->fg_gc[widgetstate], shadow_pix, 0, 0, offsetx - 16, offsety - 16, 32, 32);
}
#endif

/* Draw a single hexagon */
static void hexagon(GtkWidget *widget, int offsetx, int offsety)
{
	const int radius = 30;
	int radx;
	int rady;
	GdkColor colour;

	radx = (int)(3.0 * (float)radius / 6.0);
	rady = (int)(5.0 * (float)radius / 6.0);

	colour.pixel = 0;
	colour.red = 0xFFFF;
	colour.green = 0xFFFF;
	colour.blue = 0xFFFF;
	gdk_gc_set_foreground(widget->style->black_gc, &colour);
	/*gdk_gc_set_background(widget->style->black_gc, &colour);*/

	gdk_draw_line(hastings_buf, widget->style->white_gc,
		offsetx - radx, offsety - rady, offsetx + radx, offsety - rady);
	gdk_draw_line(hastings_buf, widget->style->white_gc,
		offsetx - radx, offsety + rady, offsetx + radx, offsety + rady);
	gdk_draw_line(hastings_buf, widget->style->white_gc,
		offsetx - radx, offsety - rady, offsetx - radius, offsety);
	gdk_draw_line(hastings_buf, widget->style->white_gc,
		offsetx - radx, offsety + rady, offsetx - radius, offsety);
	gdk_draw_line(hastings_buf, widget->style->white_gc,
		offsetx + radx, offsety - rady, offsetx + radius, offsety);
	gdk_draw_line(hastings_buf, widget->style->white_gc,
		offsetx + radx, offsety + rady, offsetx + radius, offsety);
}

/* Do all display works */
void display_board(void)
{
	int i, j;
	GtkWidget* tmp = gtk_object_get_data(GTK_OBJECT(main_win), "drawingarea");
	int offsetx, offsety;

	/* map background */
	draw(map_pix, 0, 0, 510, 510);

	/* hexagons */
	for(j = 0; j < 19; j++)
		for(i = 0; i < 5 + j % 2; i++)
			if (game.boardmap[i][j] != 32) hexagon(tmp, 75 - j % 2 * 45 + i * 90, 30 + j * 25);

	/* knights */
	for(j = 0; j < 19; j++)
		for(i = 0; i < 5 + j % 2; i++)
		{
			offsetx = 75 - j % 2 * 45 + i * 90;
			offsety = 30 + j * 25;

			if(game.board[i][j] > -1)
			{
				/* draw mask */
				draw(man_pix[game.board[i][j] / 2],
				     offsetx - 16, offsety - 16, 32, 32);

				/* which nation is the knight of? */

				/* Black team */
				if (game.board[i][j] % 2) {
					draw(frame_black_pix,
					     offsetx + 10, offsety + 10,
					     5, 5);
				}

				/* Draw a shadow for own knights */
				if(game.board[i][j] == game.self) {
					draw(shadow_pix,
					     offsetx - 16, offsety - 16,
					     32, 32);
#ifdef USE_SHADOW
					shadow(i, j, GTK_WIDGET_STATE(tmp));
#endif
				}
			}
		}

	/* selected a knight? */
	if (game.state == STATE_MOVE)
		highlight(game.move_src_x, game.move_src_y, GTK_WIDGET_STATE(tmp));

	/*gdk_draw_pixmap(main_win->window, tmp->style->black_gc, hastings_buf, 0, 0, 0, 0, 510, 510);*/

	/* finally, draw it */
	gtk_widget_draw(tmp, NULL);
}

static GdkPixbuf *load_pixmap(const char *name)
{
	char *fullpath;
	GdkPixbuf *image;
	GError *error = NULL;

	fullpath = g_strdup_printf("%s/hastings/pixmaps/%s.png",
				   GGZDATADIR, name);
	image = gdk_pixbuf_new_from_file(fullpath, &error);
	if(image == NULL)
		ggz_error_msg_exit("Can't load pixmap %s", fullpath);
	g_free(fullpath);

	return image;
}

/* Load pixmap files */
static void on_main_win_realize(GtkWidget* widget, gpointer user_data)
{
	GtkStyle* style;

	/* now for the pixmap from gdk */
	style = gtk_widget_get_style(main_win);

	man_pix[0] = load_pixmap("newmanred");
	man_pix[1] = load_pixmap("newmanblue");
	man_pix[2] = load_pixmap("newmangreen");
	man_pix[3] = load_pixmap("newmanyellow");

	shadow_pix = load_pixmap("shadow");

	frame_ul_pix = load_pixmap("frame_ul");
	frame_ur_pix = load_pixmap("frame_ur");
 	frame_ll_pix = load_pixmap("frame_ll");
	frame_lr_pix = load_pixmap("frame_lr");

	frame_black_pix = load_pixmap("frame_black");

	map_pix = load_pixmap("map");
}

/* Quit the game */
static gboolean main_exit(GtkWidget *widget, GdkEvent *event,
			  gpointer user_data)
{
	game_exit();
	return TRUE;
}

/* Resyncing... */
void game_resync(void)
{
	request_sync();
}

/* Leave the game */
void game_exit(void)
{
	ggz_show_exit_dialog(1, main_win);
}

/* Draw the screen? Unsure */
static gboolean configure_handle(GtkWidget *widget, GdkEventConfigure *event,
				 gpointer user_data)
{
	if (hastings_buf) gdk_pixmap_unref(hastings_buf);
	else
	{
		hastings_buf = gdk_pixmap_new(widget->window,
			widget->allocation.width, widget->allocation.height, -1);
		gdk_draw_rectangle(hastings_buf, widget->style->black_gc,
			TRUE, 0, 0, widget->allocation.width, widget->allocation.height);
	}
	return TRUE;
}

/* Unsure */
static gboolean expose_handle(GtkWidget *widget, GdkEventExpose  *event,
			      gpointer user_data)
{
	gdk_draw_pixmap(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		hastings_buf, event->area.x, event->area.y,	event->area.x, event->area.y,
		event->area.width, event->area.height);

	return FALSE;
}

/* Evaluate user mouse click on the field */
static gboolean get_move(GtkWidget *widget, GdkEventButton *event,
			 gpointer user_data)
{
	int x = (int)(event->x);
	int y = (int)(event->y);
	int col;
	int row;

	/* Determine whether player is allowed to play */
	if ((game.state != STATE_SELECT) && (game.state != STATE_MOVE))
	{
		game_status(_("Not my turn yet"));
		return TRUE;
	}

	/* determine hex-field from coordinates */
 	col = x / 15;
	row = ((y - 5) / 50) * 2;
	if(col % 3 != 0)
	{
		col = col / 3;
		if(col % 2 == 0) row = ((y + 20) / 50) * 2 - 1;
		col = col / 2;
	}

	/* Left mouse button pressed on a valid context */
	if (event->button == 1 && hastings_buf != NULL)
	{
		/* Step 1: Click on a guy to select him ... */
		if(game.state == STATE_SELECT)
		{
			game.move_src_x = col;
 			game.move_src_y = row;
			if((game.boardmap[col][row] == 32) || (col < 0) || (row < 0) || (col > 5) || (row > 18))
			{
				game_status(_("Looking for submarines? They aren't invented yet (it's 1066)."));
				return FALSE;
			}
			if(game.board[col][row] == -1)
			{
				game_status(_("I can't see anyone here. Can you?"));
				return FALSE;
			}
			if(game.board[col][row] != game.self)
			{
				game_status(_("Don't cheat! That's not yours!"
				              " You are %s on the %s team."),
				            _(player_colors[game.self / 2]),
				            _(team_colors[game.self % 2]));
				return FALSE;
			}
			game.state = STATE_MOVE;
			display_board();
			game_status(_("Where shall I go?"));
		}
		/* Step 2: ... an send him to heaven or hell. */
		else
		{
 			game.move_dst_x = col;
	 		game.move_dst_y = row;
			send_my_move();
		}
	}
	return TRUE;
}

static GtkWidget *create_menus(GtkWidget *window)
{
	GtkAccelGroup *accel_group;
	GtkItemFactory *menu;
	GtkItemFactoryEntry items[] = {
	  {_("/_Table"), NULL, NULL, 0, "<Branch>"},
	  {_("/Table/Player _list"), "<ctrl>L",
	   create_or_raise_dlg_players, 0, NULL},
	  {_("/Table/_Sync with server"), "<ctrl>S", game_resync, 0, NULL},
	  {_("/Table/E_xit"), "<ctrl>X", game_exit, 0, NULL},
	  {_("/_Help"), NULL, NULL, 0, "<LastBranch>"},
	  {_("/Help/_About"), "<ctrl>A", create_or_raise_dlg_about, 0, NULL}
	};
	const int num = sizeof(items) / sizeof(items[0]);

	accel_group = gtk_accel_group_new();

	menu = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>", accel_group);
	gtk_item_factory_create_items(menu, num, items, NULL);
	gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

	return gtk_item_factory_get_widget(menu, "<main>");
}

/* Gtk+ stuff: set up the window */
GtkWidget* create_main_win (void)
{
  GtkWidget *main_win;
  GtkWidget *main_box;
  GtkWidget *menubar;
  GtkWidget *drawingarea;
  GtkWidget *statusbar;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  main_win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (main_win), "main_win", main_win);
  gtk_window_set_title (GTK_WINDOW (main_win), _("Hastings1066"));
  gtk_window_set_policy (GTK_WINDOW (main_win), FALSE, FALSE, FALSE);

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
  gtk_widget_set_usize (drawingarea, 510, 510);
  gtk_widget_set_events (drawingarea, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);

  statusbar = gtk_statusbar_new ();
  gtk_widget_ref (statusbar);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "statusbar", statusbar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (statusbar);
  gtk_box_pack_start (GTK_BOX (main_box), statusbar, FALSE, FALSE, 0);

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

