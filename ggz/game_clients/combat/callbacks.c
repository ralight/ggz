/*
 * File: callbacks.c
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Desc: Combat client GTK callback functions
 * $Id: callbacks.c 6330 2004-11-11 16:30:21Z jdorje $
 *
 * Copyright (C) 2002 Ismael Orenstein.
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

#include "dlg_about.h"
#include "dlg_exit.h"
#include "menus.h"

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "combat.h"
#include "game.h"
#include "dlg_options.h"

extern GdkPixmap *cbt_buf;
extern combat_game cbt_game;
struct game_info_t cbt_info;
extern GtkWidget *main_win;

static void try_to_quit(void)
{
	/* FIXME: we don't need a dialog if we're not actively playing
	   a game. */
	ggz_show_exit_dialog(1, main_win);
}

gboolean
on_main_window_configure_event(GtkWidget * widget,
			       GdkEventConfigure * event,
			       gpointer user_data)
{
	return FALSE;
}


void
on_main_window_draw(GtkWidget * widget,
		    GdkRectangle * area, gpointer user_data)
{

}


gboolean
on_mainarea_expose_event(GtkWidget * widget,
			 GdkEventExpose * event, gpointer user_data)
{

	if (cbt_buf)
		gdk_draw_drawable(widget->window,
				  widget->style->
				  fg_gc[GTK_WIDGET_STATE(widget)], cbt_buf,
				  event->area.x, event->area.y,
				  event->area.x, event->area.y,
				  event->area.width, event->area.height);

	return FALSE;
}


gboolean
main_window_exit(GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
	try_to_quit();

	return TRUE;
}



void game_exit(void)
{
	try_to_quit();
}


gboolean
on_mainarea_configure_event(GtkWidget * widget,
			    GdkEventConfigure * event, gpointer user_data)
{
	if (!cbt_buf)
		cbt_buf =
		    gdk_pixmap_new(widget->window,
				   cbt_game.width * (PIXSIZE + 1) + 1,
				   cbt_game.height * (PIXSIZE + 1) + 1,
				   -1);
	game_draw_bg();
	game_draw_board();

	return FALSE;
}


gboolean
on_mainarea_button_press_event(GtkWidget * widget,
			       GdkEventButton * event, gpointer user_data)
{
	int x = event->x / (PIXSIZE + 1) + 1;
	int y = event->y / (PIXSIZE + 1) + 1;


	switch (cbt_game.state) {
	case CBT_STATE_WAIT:
		if (!cbt_game.map)
			break;
	case CBT_STATE_SETUP:
		game_handle_setup(CART(x, y, cbt_game.width));
		break;
	case CBT_STATE_PLAYING:
		game_handle_move(CART(x, y, cbt_game.width));
		break;
	}

	return FALSE;
}

/* 0 => insensitive and hidden
 * 1 => sensitive and shown
 * 2 => insensitive
 * 3 => sensitive
 * (Note TRUE == 1)
 */
void callback_widget_set_enabled(char *name, int mode)
{
	GtkWidget *widget;
	widget = lookup_widget(main_win, name);
	if (widget == NULL)
		return;
	gtk_widget_set_sensitive(widget, (mode % 2));
	if (mode == FALSE)
		gtk_widget_hide(widget);
	else if (mode == TRUE)
		gtk_widget_show(widget);
}

void on_send_setup_clicked(GtkButton * button, gpointer user_data)
{
	game_send_setup();

}


void change_show_enemy(GtkWidget * button, gpointer user_data)
{
	GtkWidget *checkmenuitem =
	    g_object_get_data(G_OBJECT(button), "checkmenu");
	g_object_set_data(G_OBJECT(checkmenuitem), "dirty",
			  GINT_TO_POINTER(TRUE));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(checkmenuitem),
				       GPOINTER_TO_INT(user_data));
	cbt_info.show_enemy = GPOINTER_TO_INT(user_data);
}



void on_save_map_menu_activate(void)
{
	if (cbt_game.army && cbt_game.map)
		game_ask_save_map();
}

void on_show_game_options_activate(void)
{
	if (cbt_game.army && cbt_game.map)
		game_message(combat_options_describe(&cbt_game, 0));
}

void on_remember_enemy_units_toggled(void)
{
	GtkWidget *dlg;
	GtkWidget *yes;
	GtkWidget *item =
	    get_menu_item(_("<main>/Game/Remember enemy units"));
	gboolean dirty =
	    GPOINTER_TO_INT(g_object_get_data(G_OBJECT(item), "dirty"));
	if (dirty) {
		g_object_set_data(G_OBJECT(item), "dirty",
				  GINT_TO_POINTER(FALSE));
		return;
	}
	// Let's see the current state
	if (GTK_CHECK_MENU_ITEM(item)->active) {
		// Let's see if it's possible
		if (cbt_game.options & OPT_SHOW_ENEMY_UNITS) {
			// Ok, that's normal
			// Just do it without drama
			cbt_info.show_enemy = TRUE;
			return;
		}
		// Ok, the user is cheating.
		dlg =
		    create_yes_no_dlg(_
				      ("You are cheating. This is really bad!\n"
				       "Please take a moment to think of your opponent.\n"
				       "Does he know what you are doing?\n\n\n"
				       "So, are you sure you want to do it?"),
				      GTK_SIGNAL_FUNC(change_show_enemy),
				      GINT_TO_POINTER(TRUE));
		yes = lookup_widget(dlg, "yes");
		g_object_set_data(G_OBJECT(yes), "checkmenu", item);
		gtk_widget_show_all(dlg);
		// Let's keep it not active until the user selects
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
					       FALSE);
	} else {
		// It is being deactived
		// Let's just keep it
		cbt_info.show_enemy = FALSE;
	}
}
