/*
 * File: main_win.c
 * Author: Ismael Orenstein
 * Project: GGZ Reversi game module
 * Date: 09/17/2000
 * Desc: Functions to deal with the graphics stuff
 *
 * Copyright (C) 2000 Ismael Orenstein.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <game.h>
#include <support.h>
#include <black.xpm>
#include <dot.xpm>
#include <white.xpm>


/* Pixmaps */
#define PIXSIZE 48
GdkPixmap* pix[3];
GdkBitmap* pix_mask[3];
GdkGC* pix_gc;
GdkPixmap* rvr_buf;

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
	
	printf(message);
	printf("\n");
	
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
	int piece = -1;
	GtkStyle* style;
	GtkWidget* white_label;
	GtkWidget* black_label;
	char score[29];

	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "drawingarea");
	style = gtk_widget_get_style(main_win);

	draw_bg(main_win);

	for (i = 0; i < 64; i++) {
		if (game.board[i] == BLACK) {
			piece = PLAYER2SEAT(BLACK);
		}
		else if (game.board[i] == WHITE) {
			piece = PLAYER2SEAT(WHITE);
		}
		else if (game_check_move(game.turn, i)) {
			piece = 2;
		}
		else
			continue;
		
		x = (X(i)-1)*PIXSIZE;
		y = (Y(i)-1)*PIXSIZE;
		
		gdk_gc_set_tile(pix_gc, pix[piece]);
		gdk_gc_set_ts_origin(pix_gc, x, y);
		gdk_gc_set_clip_origin(pix_gc, x, y);
		gdk_gc_set_clip_mask(pix_gc, pix_mask[piece]);
		// if last move, mark it (draw using a different function)
		if (i == game.last_move)
			gdk_draw_rectangle(rvr_buf, style->light_gc[3], TRUE, x+1, y+1, PIXSIZE-2, PIXSIZE-2);
		gdk_draw_rectangle(rvr_buf, pix_gc, TRUE, x, y, PIXSIZE, PIXSIZE);
	}

	if (game.state == RVR_STATE_PLAYING && game.turn == game.num)
		game_status("It's your turn!");
	else if (game.state == RVR_STATE_PLAYING && game.turn == -game.num)
		game_status("Wait for your oponnents turn");

	// Update the scores
	white_label = gtk_object_get_data( GTK_OBJECT(main_win), "white_label");
	black_label = gtk_object_get_data( GTK_OBJECT(main_win), "black_label");
	sprintf(score, "White(%s): %d", game.names[PLAYER2SEAT(WHITE)], game.white);
	gtk_label_set_text(GTK_LABEL(white_label), score);
	sprintf(score, "Black(%s): %d", game.names[PLAYER2SEAT(BLACK)], game.black);
	gtk_label_set_text(GTK_LABEL(black_label), score);
	
	
	gtk_widget_draw(tmp, NULL);
}


void on_main_win_realize(GtkWidget* widget, gpointer user_data)
{
	GtkStyle* style;
	
	// Get the current style
	style = gtk_widget_get_style(main_win);

	// Create the pix gc
	pix_gc = gdk_gc_new(main_win->window);
	gdk_gc_ref(pix_gc);
	gdk_gc_set_fill(pix_gc, GDK_TILED);

	// Create the black pix
	pix[PLAYER2SEAT(BLACK)] = gdk_pixmap_create_from_xpm_d( main_win->window, &pix_mask[PLAYER2SEAT(BLACK)],
					      &style->black, 
					      (gchar**)black_xpm );
	gdk_pixmap_ref(pix[PLAYER2SEAT(BLACK)]);
	gdk_bitmap_ref(pix_mask[PLAYER2SEAT(BLACK)]);

	// Sets up the white pix
	pix[PLAYER2SEAT(WHITE)] = gdk_pixmap_create_from_xpm_d( main_win->window, &pix_mask[PLAYER2SEAT(WHITE)],
					      &style->white, 
					      (gchar**)white_xpm );
	gdk_pixmap_ref(pix[PLAYER2SEAT(WHITE)]);
	gdk_bitmap_ref(pix_mask[PLAYER2SEAT(WHITE)]);

	// Create the dot pix
	pix[2] = gdk_pixmap_create_from_xpm_d( main_win->window, &pix_mask[2], NULL, (gchar**)dot_xpm);
	gdk_pixmap_ref(pix[2]);
	gdk_bitmap_ref(pix_mask[2]);
}


gboolean main_exit(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	// FIXME: should call an "are you sure dialog"
	gtk_main_quit();
	
	return FALSE;
}


void game_resync(GtkMenuItem *menuitem, gpointer user_data)
{
	// Request sync from the server
	request_sync();
}


void game_exit(GtkMenuItem *menuitem, gpointer user_data)
{
	// FIXME: should call an "are you sure dialog"
	gtk_main_quit();
}


void game_about(GtkMenuItem *menuitem, gpointer user_data)
{
/*

*/
}


gboolean configure_handle(GtkWidget *widget, GdkEventConfigure *event, 
			  gpointer user_data)
{
	if (rvr_buf)
		gdk_pixmap_unref(rvr_buf);
	else {
		rvr_buf = gdk_pixmap_new( widget->window,
					  widget->allocation.width,
					  widget->allocation.height,
					  -1);
	}
	draw_bg(widget);

	return TRUE;
}

void draw_bg(GtkWidget *widget) {
	int i;
	gdk_draw_rectangle( rvr_buf,
					widget->style->mid_gc[3],
					TRUE,
					0, 0,
					widget->allocation.width,
					widget->allocation.height);

	for (i = 1; i < 8; i++) {
		gdk_draw_line(rvr_buf, 
					widget->style->black_gc,
					i*PIXSIZE, 0,
					i*PIXSIZE, widget->allocation.height);

		gdk_draw_line(rvr_buf, 
					widget->style->black_gc,
					0, i*PIXSIZE,
					widget->allocation.width, i*PIXSIZE);
	}
	
	return;
}


gboolean expose_handle(GtkWidget *widget, GdkEventExpose  *event, 
		       gpointer user_data)
{
	gdk_draw_pixmap( widget->window,
			 widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
			 rvr_buf,
			 event->area.x, event->area.y,
			 event->area.x, event->area.y,
			 event->area.width, event->area.height);
	
	return FALSE;
}


gboolean handle_move(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	int x = (int)(event->x);
	int y = (int)(event->y);
	int move, status = 0;
	x/=PIXSIZE;
	y/=PIXSIZE;
	x++;
	y++;
	move = CART(x,y);

	// Check if it's right time
	if (game.state != RVR_STATE_PLAYING) {
		game_status("Game hasn't started yet");
		return FALSE;
	}

	// Check if it's his turn
	if (game.num != game.turn) {
		game_status("This is not your turn yet!");
		return FALSE;
	}

	// Check if out of bounds
	if (x < 1 || x > 8 || y < 1 || y > 8) {
		game_status("Move out of bounds!");
		return FALSE;
	}

	// Check if duplicated
	if (game.board[move] != EMPTY) {
		game_status("Invalid move!");
		return FALSE;
	}

	/* CHECK IF THE MOVE IS VALID */

	// Check if it's valid up
	status += game_check_move(game.turn, CART(x,y));
	
	if (status <= 0) {
		game_status("Invalid move!");
		return FALSE;
	}

	if (event->button == 1 && rvr_buf != NULL) {
		game_status("Moving on %d, %d", x, y);
		send_my_move(CART(x,y));
	}

	return TRUE;
}


GtkWidget*
create_main_win (void)
{
  GtkWidget *main_win;
  GtkWidget *main_box;
  GtkWidget *menubar;
	GtkWidget *black_label;
	GtkWidget *black_label_frame;
	GtkWidget *white_label;
	GtkWidget *white_label_frame;
	GtkWidget *label_box;
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
  gtk_window_set_title (GTK_WINDOW (main_win), _("Reversi"));

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
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (game_menuhead)->child), _("_Game"));
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

	// Create labels frames
	white_label_frame = gtk_frame_new(NULL);
	gtk_widget_ref(white_label_frame);
	gtk_object_set_data_full (GTK_OBJECT(main_win), "white_label_frame", white_label_frame, (GtkDestroyNotify)gtk_widget_unref);
	gtk_widget_show(white_label_frame);
	
	black_label_frame = gtk_frame_new(NULL);
	gtk_widget_ref(black_label_frame);
	gtk_object_set_data_full (GTK_OBJECT(main_win), "black_label_frame", black_label_frame, (GtkDestroyNotify)gtk_widget_unref);
	gtk_widget_show(black_label_frame);

	// Create labels
	white_label = gtk_label_new("White: 0");
	// Why is that? Really don't know, but all the other scripts have it!
  gtk_widget_ref (white_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "white_label", white_label,
                            (GtkDestroyNotify) gtk_widget_unref);

	black_label = gtk_label_new("Black: 0");
  gtk_widget_ref (black_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "black_label", black_label,
                            (GtkDestroyNotify) gtk_widget_unref);

	gtk_container_add( GTK_CONTAINER(white_label_frame), white_label);
	gtk_container_add( GTK_CONTAINER(black_label_frame), black_label);
	gtk_widget_show(white_label);
	gtk_widget_show(black_label);

	// Label box
	label_box = gtk_hbox_new(TRUE, 5);
  gtk_widget_ref (label_box);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label_box", label_box,
                            (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_set_usize(label_box, PIXSIZE*8, 20);
	gtk_box_pack_start( GTK_BOX(label_box), black_label_frame, FALSE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(label_box), white_label_frame, FALSE, TRUE, 0 );
	gtk_widget_show(label_box);
	gtk_box_pack_start( GTK_BOX(main_box), label_box, FALSE, TRUE, 0);

  drawingarea = gtk_drawing_area_new ();
  gtk_widget_ref (drawingarea);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "drawingarea", drawingarea,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (drawingarea);
  gtk_box_pack_start (GTK_BOX (main_box), drawingarea, TRUE, TRUE, 0);
  gtk_widget_set_usize (drawingarea, PIXSIZE*8, PIXSIZE*8);
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
                      GTK_SIGNAL_FUNC (handle_move),
                      NULL);

  gtk_window_add_accel_group (GTK_WINDOW (main_win), accel_group);

  return main_win;
}

int game_check_move(int player, int move) {
	int x = X(move);
	int y = Y(move);
	int status = 0;

	status += game_check_direction(player, 0,-1, x, y);
	status += game_check_direction(player, 1,-1, x, y);
	status += game_check_direction(player, 1, 0, x, y);
	status += game_check_direction(player, 1, 1, x, y);
	status += game_check_direction(player, 0, 1, x, y);
	status += game_check_direction(player,-1, 1, x, y);
	status += game_check_direction(player,-1, 0, x, y);
	status += game_check_direction(player,-1,-1, x, y);

	return status;

}
