/*
 * File: main_win.c
 * Author: GGZ Development Team
 * Project: GGZ Chess game module
 * Desc: Chess client main window creation
 * $Id: main_win.c 6240 2004-11-03 19:24:53Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "dlg_about.h"
#include "dlg_players.h"

#include "callbacks.h"
#include "main_win.h"
#include "support.h"

static void game_about(GtkMenuItem *menuitem, gpointer user_data)
{
	create_or_raise_dlg_about();
}

static GtkWidget *create_menus(GtkWidget *window)
{
	GtkAccelGroup *accel_group;
	GtkItemFactory *menu;
	GtkWidget *menu_item;
	/* FIXME: the menu items used to have tooltips, and some of them
	   need them!  But I don't know how to do this with the item
	   factory.  --JDS */
	GtkItemFactoryEntry items[] = {
	  {_("/_Table"), NULL, NULL, 0, "<Branch>"},
	  {_("/Table/Player _list"), "<ctrl>L",
	   create_or_raise_dlg_players, 0, NULL},
	  {_("/Table/_Sync with server"), "<ctrl>S",
	   board_request_update, 0, NULL},
	  {_("/Table/E_xit"), "<ctrl>X", on_exit_activate, 0, NULL},
	  {_("/_Game"), NULL, NULL, 0, "<Branch>"},
	  {_("/Game/Request _draw"), "<ctrl>D", board_request_draw, 0, NULL},
	  {_("/Game/_Call flag"), "<ctrl>C", board_call_flag, 0, NULL},
	  {_("/_Options"), NULL, NULL, 0, "<Branch>"},
	  {_("/Options/_Auto call flag"), NULL,
	   on_auto_call_flag_activate, 1, "<ToggleItem>"},
	  {_("/_Help"), NULL, NULL, 0, "<LastBranch>"},
	  {_("/Help/_About"), "<ctrl>A", game_about, 0, NULL}
	};
	const int num = sizeof(items) / sizeof(items[0]);

	accel_group = gtk_accel_group_new();

	menu = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>", accel_group);
	gtk_item_factory_create_items(menu, num, items, NULL);
	gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

	menu_item = gtk_item_factory_get_widget(menu,
						_("<main>/Options/"
						  "Auto call flag"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu_item),
				       TRUE);
	gtk_object_set_data(GTK_OBJECT(window), "auto_call_flag", menu_item);

	return gtk_item_factory_get_widget(menu, "<main>");
}

GtkWidget *create_main_win(void)
{
  GtkWidget *main_win;
  GtkWidget *vbox1;
  GtkWidget *menubar;
  GtkWidget *hbox1;
  GtkWidget *hpaned1;
  GtkWidget *board;
  GtkWidget *vpaned1;
  GtkWidget *vbox2;
  GtkWidget *hbox2;
  GtkWidget *white_arrow;
  GtkWidget *white_time;
  GtkWidget *hbox3;
  GtkWidget *black_arrow;
  GtkWidget *black_time;
  GtkWidget *vbox3;
  GtkWidget *last_moves_label;
  GtkWidget *scrolledwindow1;
  GtkWidget *last_moves;
  GtkWidget *statusbar;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  main_win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (main_win), "main_win", main_win);
  gtk_window_set_title (GTK_WINDOW (main_win), _("GGZ Chess"));

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (main_win), vbox1);

  menubar = create_menus(main_win);
  gtk_widget_ref (menubar);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "menubar", menubar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (menubar);
  gtk_box_pack_start (GTK_BOX (vbox1), menubar, FALSE, FALSE, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "hbox1", hbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  hpaned1 = gtk_hpaned_new ();
  gtk_widget_ref (hpaned1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "hpaned1", hpaned1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hpaned1);
  gtk_box_pack_start (GTK_BOX (hbox1), hpaned1, TRUE, TRUE, 0);
  gtk_paned_set_position (GTK_PANED (hpaned1), 0);

  board = gtk_drawing_area_new ();
  gtk_widget_ref (board);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "board", board,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (board);
  gtk_paned_pack1 (GTK_PANED (hpaned1), board, FALSE, FALSE);
  gtk_widget_set_usize (board, 512, 512);
  gtk_widget_set_events (board, GDK_BUTTON_PRESS_MASK);

  vpaned1 = gtk_vpaned_new ();
  gtk_widget_ref (vpaned1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "vpaned1", vpaned1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vpaned1);
  gtk_paned_pack2 (GTK_PANED (hpaned1), vpaned1, TRUE, TRUE);
  gtk_paned_set_position (GTK_PANED (vpaned1), 0);

  vbox2 = gtk_vbox_new (TRUE, 15);
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "vbox2", vbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_paned_pack1 (GTK_PANED (vpaned1), vbox2, FALSE, FALSE);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox2);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "hbox2", hbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox2, FALSE, FALSE, 0);

  white_arrow = gtk_arrow_new (GTK_ARROW_RIGHT, GTK_SHADOW_IN);
  gtk_widget_ref (white_arrow);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "white_arrow", white_arrow,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (white_arrow);
  gtk_box_pack_start (GTK_BOX (hbox2), white_arrow, FALSE, FALSE, 0);

  white_time = gtk_label_new (_("White -> 99:99"));
  gtk_widget_ref (white_time);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "white_time", white_time,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (white_time);
  gtk_box_pack_start (GTK_BOX (hbox2), white_time, TRUE, TRUE, 0);
  gtk_label_set_justify (GTK_LABEL (white_time), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_alignment (GTK_MISC (white_time), 1, 0.5);
  gtk_misc_set_padding (GTK_MISC (white_time), 10, 0);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox3);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "hbox3", hbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox3, TRUE, TRUE, 0);

  black_arrow = gtk_arrow_new (GTK_ARROW_RIGHT, GTK_SHADOW_IN);
  gtk_widget_ref (black_arrow);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "black_arrow", black_arrow,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (black_arrow);
  gtk_box_pack_start (GTK_BOX (hbox3), black_arrow, FALSE, FALSE, 0);

  black_time = gtk_label_new (_("Black -> 99:99"));
  gtk_widget_ref (black_time);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "black_time", black_time,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (black_time);
  gtk_box_pack_start (GTK_BOX (hbox3), black_time, TRUE, TRUE, 0);
  gtk_label_set_justify (GTK_LABEL (black_time), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_alignment (GTK_MISC (black_time), 1, 0.5);
  gtk_misc_set_padding (GTK_MISC (black_time), 10, 0);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox3);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "vbox3", vbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox3);
  gtk_paned_pack2 (GTK_PANED (vpaned1), vbox3, FALSE, TRUE);

  last_moves_label = gtk_label_new (_("Last moves"));
  gtk_widget_ref (last_moves_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "last_moves_label", last_moves_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (last_moves_label);
  gtk_box_pack_start (GTK_BOX (vbox3), last_moves_label, FALSE, FALSE, 0);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "scrolledwindow1", scrolledwindow1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (vbox3), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  last_moves = gtk_text_view_new_with_buffer(gtk_text_buffer_new(NULL));
  gtk_text_view_set_editable(GTK_TEXT_VIEW(last_moves), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(last_moves), FALSE);
  gtk_widget_ref (last_moves);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "last_moves", last_moves,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (last_moves);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), last_moves);
  gtk_text_buffer_insert_at_cursor(gtk_text_view_get_buffer
				   (GTK_TEXT_VIEW(last_moves)),
				   _("Last moves: \n"),
				   -1);

  statusbar = gtk_statusbar_new ();
  gtk_widget_ref (statusbar);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "statusbar", statusbar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (statusbar);
  gtk_box_pack_start (GTK_BOX (vbox1), statusbar, FALSE, FALSE, 0);

  gtk_signal_connect (GTK_OBJECT (main_win), "destroy_event",
                      GTK_SIGNAL_FUNC (gtk_main_quit),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (main_win), "delete_event",
                      GTK_SIGNAL_FUNC (ExitDialog),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (board), "configure_event",
                      GTK_SIGNAL_FUNC (on_board_configure_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (board), "expose_event",
                      GTK_SIGNAL_FUNC (on_board_expose_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (board), "button_press_event",
                      GTK_SIGNAL_FUNC (on_board_button_press_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (board), "drag_begin",
                      GTK_SIGNAL_FUNC (on_board_drag_begin),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (board), "drag_motion",
                      GTK_SIGNAL_FUNC (on_board_drag_motion),
                      NULL);
  gtk_signal_connect_after (GTK_OBJECT (board), "drag_drop",
                            GTK_SIGNAL_FUNC (on_board_drag_drop),
                            NULL);

  gtk_object_set_data (GTK_OBJECT (main_win), "tooltips", tooltips);

  return main_win;
}

