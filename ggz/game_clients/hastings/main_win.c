/*
 * File: main_win.c
 * Author: Josef Spillner
 * Original Author: Brent Hendricks
 * Project: GGZ Hastings1066 game module
 * Date: 09/13/00
 * Desc: Main window creation and callbacks
 *
 * Copyright (C) 2000 Josef Spillner
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

/* Hastings files */
#include <main_win.h>
#include <game.h>
#include <support.h>
#include <dlg_about.h>

/* Pixmap files */
#include <newmanred.xpm>
#include <newmangreen.xpm>
#include <newmanblue.xpm>
#include <newmanyellow.xpm>
#include <frame_ul.xpm>
#include <frame_ur.xpm>
#include <frame_ll.xpm>
#include <frame_lr.xpm>
#include <frame_black.xpm>
#include <map.xpm>

/* Unit pictures */
/* Red is 0, green is 1, blue is 2, yellow is 3 */
GdkPixmap* man_pix[4];
GdkPixmap* frame_ll_pix;
GdkPixmap* frame_lr_pix;
GdkPixmap* frame_ul_pix;
GdkPixmap* frame_ur_pix;
GdkPixmap *frame_black_pix;

/* Transparency mask */
GdkGC* man_gc[4];
GdkBitmap* man_mask[4];

/* Map picture */
GdkPixmap* map_pix;

/* Buffer for the map */
GdkPixmap* hastings_buf;

/* Main window */
GtkWidget *main_win;

/* Global game variables */
extern struct game_state_t game;


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

	printf("STATUS: %s\n", message);

	g_free(message);

}

/* Draw a frame around a knight */
void highlight(int col, int row, int widgetstate)
{
	GtkStyle* style;
	int offsetx, offsety;

	style = gtk_widget_get_style(main_win);
	offsetx = 75 - row % 2 * 45 + col * 90;
	offsety = 30 + row * 25;

	gdk_draw_pixmap(hastings_buf, style->fg_gc[widgetstate], frame_ul_pix, 0, 0, offsetx - 16, offsety - 16, 5, 5);
	gdk_draw_pixmap(hastings_buf, style->fg_gc[widgetstate], frame_ur_pix, 0, 0, offsetx + 11, offsety - 16, 5, 5);
	gdk_draw_pixmap(hastings_buf, style->fg_gc[widgetstate], frame_ll_pix, 0, 0, offsetx - 16, offsety + 11, 5, 5);
	gdk_draw_pixmap(hastings_buf, style->fg_gc[widgetstate], frame_lr_pix, 0, 0, offsetx + 11, offsety + 11, 5, 5);
}

/* Draw a single hexagon */
void hexagon(GtkWidget *widget, int offsetx, int offsety)
{
	const int radius = 30;
	int radx;
	int rady;

	radx = (int)(3.0 * (float)radius / 6.0);
	rady = (int)(5.0 * (float)radius / 6.0);

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
	GtkWidget* tmp;
	GtkStyle* style;
	int offsetx, offsety;

	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "drawingarea");
	style = gtk_widget_get_style(main_win);

	/* map background */
	gdk_draw_pixmap(hastings_buf, tmp->style->black_gc, map_pix, 0, 0, 0, 0, 510, 510);

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
				gdk_gc_set_ts_origin(man_gc[game.board[i][j] % 4], offsetx - 16, offsety - 16);
				gdk_gc_set_clip_origin(man_gc[game.board[i][j] % 4], offsetx - 16, offsety - 16);
				gdk_gc_set_clip_mask(man_gc[game.board[i][j] % 4], man_mask[game.board[i][j] % 4]);
				gdk_draw_rectangle(hastings_buf, man_gc[game.board[i][j] % 4], TRUE, offsetx - 16, offsety - 16, 32, 32);

				/* which nation is the knight of? */
				/*gdk_draw_pixmap(hastings_buf, style->fg_gc[GTK_WIDGET_STATE(tmp)],
							man_pix[game.board[i][j] % 4], 0, 0, offsetx - 16, offsety - 16, 32, 32);*/

				/* Black team */
				if(game.board[i][j] % 4)
				{
					gdk_draw_pixmap(hastings_buf, style->fg_gc[GTK_WIDGET_STATE(tmp)],
						frame_black_pix, 0, 0, offsetx + 10, offsety + 10, 5, 5);
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

/* Load pixmap files */
void on_main_win_realize(GtkWidget* widget, gpointer user_data)
{
	GtkStyle* style;
	GdkBitmap* mask;
	int i;

	/* now for the pixmap from gdk */
	style = gtk_widget_get_style(main_win);

	man_pix[0] = gdk_pixmap_create_from_xpm_d(main_win->window, &man_mask[0], &style->bg[GTK_STATE_NORMAL], (gchar**)newmanred_xpm);
	man_pix[1] = gdk_pixmap_create_from_xpm_d(main_win->window, &man_mask[1], &style->bg[GTK_STATE_NORMAL], (gchar**)newmanblue_xpm);
	man_pix[2] = gdk_pixmap_create_from_xpm_d(main_win->window, &man_mask[2], &style->bg[GTK_STATE_NORMAL], (gchar**)newmangreen_xpm);
	man_pix[3] = gdk_pixmap_create_from_xpm_d(main_win->window, &man_mask[3], &style->bg[GTK_STATE_NORMAL], (gchar**)newmanyellow_xpm);

	for(i = 0; i < 4; i++)
	{
		man_gc[i] = gdk_gc_new(main_win->window);
		gdk_gc_set_fill(man_gc[i], GDK_TILED);
		gdk_gc_set_tile(man_gc[i], man_pix[i]);
	}

	frame_ul_pix = gdk_pixmap_create_from_xpm_d(main_win->window, &mask, &style->bg[GTK_STATE_NORMAL], (gchar**)frame_ul_xpm);
	frame_ur_pix = gdk_pixmap_create_from_xpm_d(main_win->window, &mask, &style->bg[GTK_STATE_NORMAL], (gchar**)frame_ur_xpm);
 	frame_ll_pix = gdk_pixmap_create_from_xpm_d(main_win->window, &mask, &style->bg[GTK_STATE_NORMAL], (gchar**)frame_ll_xpm);
	frame_lr_pix = gdk_pixmap_create_from_xpm_d(main_win->window, &mask, &style->bg[GTK_STATE_NORMAL], (gchar**)frame_lr_xpm);

	frame_black_pix = gdk_pixmap_create_from_xpm_d(main_win->window, &mask, &style->bg[GTK_STATE_NORMAL], (gchar**)frame_black_xpm);

	map_pix = gdk_pixmap_create_from_xpm_d(main_win->window, &mask, &style->bg[GTK_STATE_NORMAL], (gchar**)map_xpm);
}

/* Quit the game */
gboolean main_exit(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	/* FIXME: should call an "are you sure dialog" */
	gtk_main_quit();

	return FALSE;
}

/* Resyncing... */
void game_resync(GtkMenuItem *menuitem, gpointer user_data)
{
	request_sync();
}

/* Leave the game */
void game_exit(GtkMenuItem *menuitem, gpointer user_data)
{
	/* FIXME: should call an "are you sure dialog" */
	gtk_main_quit();
}

/* Display the about dialog, crediting me all over... */
void game_about(GtkMenuItem *menuitem, gpointer user_data)
{

	static GtkWidget *dlg_about = NULL;

	if(dlg_about != NULL)
	{
		gdk_window_show(dlg_about->window);
		gdk_window_raise(dlg_about->window);
	}
	else
	{
		dlg_about = create_dlg_about();
		gtk_signal_connect(GTK_OBJECT(dlg_about),
				   "destroy",
				   GTK_SIGNAL_FUNC(gtk_widget_destroyed),
				   &dlg_about);
		gtk_widget_show(dlg_about);
	}
}

/* Draw the screen? Unsure */
gboolean configure_handle(GtkWidget *widget, GdkEventConfigure *event,
			  gpointer user_data)
{
	if (hastings_buf) gdk_pixmap_unref(hastings_buf);
	else
	{
		hastings_buf = gdk_pixmap_new(widget->window,
					  widget->allocation.width,
					  widget->allocation.height,
					  -1);
		gdk_draw_rectangle(hastings_buf,
				    widget->style->black_gc,
				    TRUE,
				    0, 0,
				    widget->allocation.width,
				    widget->allocation.height);
	}
	return TRUE;
}

/* Unsure */
gboolean expose_handle(GtkWidget *widget, GdkEventExpose  *event,
		       gpointer user_data)
{
	gdk_draw_pixmap(widget->window,
			widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
			hastings_buf,
			event->area.x, event->area.y,
			event->area.x, event->area.y,
			event->area.width, event->area.height);

	return FALSE;
}

/* Evaluate user mouse click on the field */
gboolean get_move(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
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
				game_status(_("Don't cheat! That's not yours! You are number %i, this is %i."),
					game.self, game.board[col][row]);
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

/* Gtk+ stuff: set up the window */
GtkWidget*
create_main_win (void)
{
  GtkWidget *main_win;
  GtkWidget *main_box;
  GtkWidget *menubar;
  guint tmp_key;
  GtkWidget *game_menuhead;
  GtkWidget *game_menu;
  GtkAccelGroup *game_menu_accels;
  GtkWidget *resync;
  GtkWidget *exit;
  GtkWidget *help_menuhead;
  GtkWidget *help_menu;
  GtkAccelGroup *help_menu_accels;
  GtkWidget *about;
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

  menubar = gtk_menu_bar_new ();
  gtk_widget_ref (menubar);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "menubar", menubar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (menubar);
  gtk_box_pack_start (GTK_BOX (main_box), menubar, FALSE, FALSE, 0);

  game_menuhead = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (game_menuhead)->child),
                                   _("_Game"));
  gtk_widget_add_accelerator (game_menuhead, "activate_item", accel_group,
                              tmp_key, GDK_MOD1_MASK, 0);
  gtk_widget_ref (game_menuhead);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "game_menuhead", game_menuhead,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (game_menuhead);
  gtk_container_add (GTK_CONTAINER (menubar), game_menuhead);

  game_menu = gtk_menu_new ();
  gtk_widget_ref (game_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "game_menu", game_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (game_menuhead), game_menu);
  game_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (game_menu));

  resync = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (resync)->child),
                                   _("Re_sync"));
  gtk_widget_add_accelerator (resync, "activate_item", game_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (resync);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "resync", resync,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (resync);
  gtk_container_add (GTK_CONTAINER (game_menu), resync);

  exit = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (exit)->child),
                                   _("E_xit"));
  gtk_widget_add_accelerator (exit, "activate_item", game_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (exit);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "exit", exit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (exit);
  gtk_container_add (GTK_CONTAINER (game_menu), exit);

  help_menuhead = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (help_menuhead)->child),
                                   _("_Help"));
  gtk_widget_add_accelerator (help_menuhead, "activate_item", accel_group,
                              tmp_key, GDK_MOD1_MASK, 0);
  gtk_widget_ref (help_menuhead);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "help_menuhead", help_menuhead,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (help_menuhead);
  gtk_container_add (GTK_CONTAINER (menubar), help_menuhead);
  gtk_menu_item_right_justify (GTK_MENU_ITEM (help_menuhead));

  help_menu = gtk_menu_new ();
  gtk_widget_ref (help_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "help_menu", help_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (help_menuhead), help_menu);
  help_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (help_menu));

  about = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (about)->child),
                                   _("_About"));
  gtk_widget_add_accelerator (about, "activate_item", help_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (about);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "about", about,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (about);
  gtk_container_add (GTK_CONTAINER (help_menu), about);

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
  gtk_signal_connect (GTK_OBJECT (resync), "activate",
                      GTK_SIGNAL_FUNC (game_resync),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (exit), "activate",
                      GTK_SIGNAL_FUNC (game_exit),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (about), "activate",
                      GTK_SIGNAL_FUNC (game_about),
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
