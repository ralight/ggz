/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
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

#include "callbacks.h"
#include "main_win.h"
#include "board.h"
#include "support.h"

GtkWidget*
create_main_win (void)
{
  GtkWidget *main_win;
  GtkWidget *vbox1;
  GtkWidget *menubar;
  GtkWidget *file;
  GtkWidget *file_menu;
  GtkAccelGroup *file_menu_accels;
  GtkWidget *exit;
  GtkWidget *game;
  GtkWidget *game_menu;
  GtkAccelGroup *game_menu_accels;
  GtkWidget *request_draw;
  GtkWidget *call_flag;
  GtkWidget *request_update;
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

  menubar = gtk_menu_bar_new ();
  gtk_widget_ref (menubar);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "menubar", menubar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (menubar);
  gtk_box_pack_start (GTK_BOX (vbox1), menubar, FALSE, FALSE, 0);

  file = gtk_menu_item_new_with_label (_("File"));
  gtk_widget_ref (file);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "file", file,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (file);
  gtk_container_add (GTK_CONTAINER (menubar), file);

  file_menu = gtk_menu_new ();
  gtk_widget_ref (file_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "file_menu", file_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (file), file_menu);
  file_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (file_menu));

  exit = gtk_menu_item_new_with_label (_("Exit"));
  gtk_widget_ref (exit);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "exit", exit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (exit);
  gtk_container_add (GTK_CONTAINER (file_menu), exit);

  game = gtk_menu_item_new_with_label (_("Game"));
  gtk_widget_ref (game);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "game", game,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (game);
  gtk_container_add (GTK_CONTAINER (menubar), game);

  game_menu = gtk_menu_new ();
  gtk_widget_ref (game_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "game_menu", game_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (game), game_menu);
  game_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (game_menu));

  request_draw = gtk_menu_item_new_with_label (_("Request draw"));
  gtk_widget_ref (request_draw);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "request_draw", request_draw,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (request_draw);
  gtk_container_add (GTK_CONTAINER (game_menu), request_draw);
  gtk_tooltips_set_tip (tooltips, request_draw, _("Ask the other player for a draw"), NULL);

  call_flag = gtk_menu_item_new_with_label (_("Call flag"));
  gtk_widget_ref (call_flag);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "call_flag", call_flag,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (call_flag);
  gtk_container_add (GTK_CONTAINER (game_menu), call_flag);
  gtk_tooltips_set_tip (tooltips, call_flag, _("If your opponent has run out of time, ask the server to end the game"), NULL);

  request_update = gtk_menu_item_new_with_label (_("Request update"));
  gtk_widget_ref (request_update);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "request_update", request_update,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (request_update);
  gtk_container_add (GTK_CONTAINER (game_menu), request_update);
  gtk_tooltips_set_tip (tooltips, request_update, _("Ask the server to update his time structures"), NULL);

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

  last_moves = gtk_text_new (NULL, NULL);
  gtk_widget_ref (last_moves);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "last_moves", last_moves,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (last_moves);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), last_moves);
  gtk_text_insert (GTK_TEXT (last_moves), NULL, NULL, NULL,
                   _("Last moves:\n"), 12);

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
                      GTK_SIGNAL_FUNC (gtk_main_quit),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (exit), "activate",
                      GTK_SIGNAL_FUNC (on_exit_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (request_draw), "activate",
                      GTK_SIGNAL_FUNC (board_request_draw),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (call_flag), "activate",
                      GTK_SIGNAL_FUNC (board_call_flag),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (request_update), "activate",
                      GTK_SIGNAL_FUNC (board_request_update),
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

