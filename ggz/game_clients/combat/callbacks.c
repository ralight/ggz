/*
 * File: callbacks.c
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Desc: Combat client GTK callback functions
 * $Id: callbacks.c 3709 2002-03-28 08:22:02Z jdorje $
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

#include "dlg_exit.h"

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "combat.h"
#include "game.h"
#include "dlg_options.h"
#include "dlg_about.h"

extern GdkPixmap *cbt_buf;
extern combat_game cbt_game;
struct game_info_t cbt_info;
extern GtkWidget *main_win;

static void try_to_quit(void)
{
  /* FIXME: we don't need a dialog if we're not actively playing
     a game. */
  ggz_show_exit_dialog(1);
}

gboolean
on_main_window_configure_event         (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
  return FALSE;
}


void
on_main_window_draw                    (GtkWidget       *widget,
                                        GdkRectangle    *area,
                                        gpointer         user_data)
{

}


gboolean
on_mainarea_expose_event               (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{

	if (cbt_buf)
		gdk_draw_pixmap( widget->window, 
				widget->style->fg_gc[GTK_WIDGET_STATE(widget)], cbt_buf, 
				event->area.x, event->area.y, event->area.x, event->area.y,
				event->area.width, event->area.height);

  return FALSE;
}


gboolean
main_window_exit                       (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  try_to_quit();

  return TRUE;
}



void
on_exit_menu_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	try_to_quit();
}


gboolean
on_mainarea_configure_event            (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
	if (!cbt_buf)
		cbt_buf = gdk_pixmap_new( widget->window, cbt_game.width * (PIXSIZE+1)+1,
														  cbt_game.height * (PIXSIZE+1)+1,
															-1);
	game_draw_bg();
	game_draw_board();

  return FALSE;
}


gboolean
on_mainarea_button_press_event         (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	int x = event->x/(PIXSIZE+1) + 1;
	int y = event->y/(PIXSIZE+1) + 1;


	switch (cbt_game.state) {
		case CBT_STATE_WAIT:
			if (!cbt_game.map)
				break;
		case CBT_STATE_SETUP:
			game_handle_setup(CART(x,y,cbt_game.width));
			break;
		case CBT_STATE_PLAYING:
			game_handle_move(CART(x,y,cbt_game.width));
			break;
	}

  return FALSE;
}

void callback_widget_set_enabled(char *name, int mode) {
 	GtkWidget *widget;
	widget = lookup_widget(main_win, name);
	if (widget == NULL)
		return;
	gtk_widget_set_sensitive (widget, (mode%2));
	if (mode == FALSE)
		gtk_widget_hide(widget);
	else if (mode == TRUE)
		gtk_widget_show(widget);
}

void
on_send_setup_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	game_send_setup();

}


void
on_request_sync_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	game_request_sync();

}

void change_show_enemy(GtkWidget *button, gpointer user_data) {
  GtkWidget *checkmenuitem = gtk_object_get_data(GTK_OBJECT(button), "checkmenu");
  gtk_object_set_data(GTK_OBJECT(checkmenuitem), "dirty", GINT_TO_POINTER(TRUE));
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(checkmenuitem), GPOINTER_TO_INT(user_data));
  cbt_info.show_enemy = GPOINTER_TO_INT(user_data);
}



void
on_save_map_menu_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  if (cbt_game.army && cbt_game.map)
    game_ask_save_map();
}

void
on_show_game_options_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  if (cbt_game.army && cbt_game.map)
    game_message(combat_options_describe(&cbt_game, 0));
}

void
on_remember_enemy_units_toggled        (GtkCheckMenuItem *checkmenuitem,
                                        gpointer         user_data)
{
  GtkWidget *dlg;
  GtkWidget *yes;
  gboolean dirty = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(checkmenuitem), "dirty"));
  if (dirty) {
    gtk_object_set_data(GTK_OBJECT(checkmenuitem), "dirty", GINT_TO_POINTER(FALSE));
    return;
  }
  // Let's see the current state
  if (checkmenuitem->active) {
    // Let's see if it's possible
    if (cbt_game.options & OPT_SHOW_ENEMY_UNITS) {
      // Ok, that's normal
      // Just do it without drama
      cbt_info.show_enemy = TRUE;
      return;
    }
    // Ok, the user is cheating.
    dlg = create_yes_no_dlg("You are cheating. This is really bad!\nPlease take a moment to think in your opponent.\nDoes he know what you are doing?\n\n\nSo, are you sure you want to do it?", GTK_SIGNAL_FUNC(change_show_enemy), GINT_TO_POINTER(TRUE));
    yes = lookup_widget(dlg, "yes");
    gtk_object_set_data(GTK_OBJECT(yes), "checkmenu", checkmenuitem);
    gtk_widget_show_all(dlg);
    // Let's keep it not active until the user selects
    gtk_check_menu_item_set_active(checkmenuitem, FALSE);
  }
  else {
    // It is being deactived
    // Let's just keep it
    cbt_info.show_enemy = FALSE;
  }
}


void
on_about_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *about;
  about = create_about_dlg();
  gtk_widget_show(about);
}
