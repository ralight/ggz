#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "connect.h"
#include "datatypes.h"
#include "dlg_error.h"
#include "dlg_exit.h"
#include "dlg_launch.h"
#include "dlg_login.h"
#include "dlg_main.h"
#include "easysock.h"
#include "err_func.h"
#include "mnu_tables.h"
#include "mnu_players.h"
#include "protocols.h"


/* Globals neaded by this dialog */
extern GtkWidget *dlg_launch;
extern GtkWidget *dlg_login;
extern GtkWidget *mnu_players;
extern GtkWidget *mnu_tables;
extern gint selected_table;
extern gint selected_type;
extern struct ConnectInfo connection;
extern struct GameTypes game_types;

/* Global GtkWidget for this dialog */
GtkWidget *main_win;

/* Local callbacks which no other file will call */
void ggz_join_game(GtkButton * button, gpointer user_data);
void ggz_logout(GtkMenuItem * menuitem, gpointer user_data);
void ggz_input_chat_msg(GtkWidget * widget, gpointer user_data);
void ggz_table_select_row_callback(GtkWidget *widget, gint row, gint column,
				   GdkEventButton *event, gpointer data);
void ggz_get_game_options(GtkButton * button, gpointer user_data);
gint ggz_event_tables( GtkWidget *widget, GdkEvent *event );
gint ggz_event_players( GtkWidget *widget, GdkEvent *event );

GtkWidget*
create_main_win (void)
{
  GtkWidget *main_win;
  GtkWidget *main_box;
  GtkWidget *menu_bar;
  guint tmp_key;
  GtkWidget *ggz1;
  GtkWidget *ggz1_menu;
  GtkAccelGroup *ggz1_menu_accels;
  GtkWidget *connect2;
  GtkWidget *disconnect1;
  GtkWidget *separator4;
  GtkWidget *login1;
  GtkWidget *logout1;
  GtkWidget *separator3;
  GtkWidget *exit1;
  GtkWidget *game_menu;
  GtkWidget *game_menu_menu;
  GtkAccelGroup *game_menu_menu_accels;
  GtkWidget *new_game2;
  GtkWidget *join_game1;
  GtkWidget *edit1;
  GtkWidget *edit1_menu;
  GtkAccelGroup *edit1_menu_accels;
  GtkWidget *properties1;
  GtkWidget *view_menu;
  GtkWidget *view_menu_menu;
  GtkAccelGroup *view_menu_menu_accels;
  GtkWidget *game_types1;
  GtkWidget *motd1;
  GtkWidget *help1;
  GtkWidget *help1_menu;
  GtkAccelGroup *help1_menu_accels;
  GtkWidget *about1;
  GtkWidget *toolbar;
  GtkWidget *launch_button;
  GtkWidget *join_button;
  GtkWidget *prefs_button;
  GtkWidget *stat_button;
  GtkWidget *logout_button;
  GtkWidget *exit_button;
  GtkWidget *v_pane;
  GtkWidget *table_box;
  GtkWidget *table_label;
  GtkWidget *table_scroll;
  GtkWidget *table_tree;
  GtkWidget *label47;
  GtkWidget *label48;
  GtkWidget *label49;
  GtkWidget *label50;
  GtkWidget *label51;
  GtkWidget *label52;
  GtkWidget *label53;
  GtkWidget *h_pane;
  GtkWidget *player_box;
  GtkWidget *player_label;
  GtkWidget *player_scroll;
  GtkWidget *player_list;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *chat_box;
  GtkWidget *hbox16;
  GtkWidget *label54;
  GtkWidget *game_room;
  GtkWidget *combo_entry2;
  GtkWidget *chat_scroll;
  GtkWidget *chat_text;
  GtkWidget *chat_separator;
  GtkWidget *msg_box;
  GtkWidget *msg_label;
  GtkWidget *msg_entry;
  GtkWidget *msg_button;
  GtkWidget *statusbar;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  main_win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (main_win), "main_win", main_win);
  gtk_widget_set_usize (main_win, 580, -2);
  gtk_window_set_title (GTK_WINDOW (main_win), "Gnu Gaming Zone");
  gtk_window_set_default_size (GTK_WINDOW (main_win), 500, 450);

  main_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (main_box);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "main_box", main_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (main_box);
  gtk_container_add (GTK_CONTAINER (main_win), main_box);

  menu_bar = gtk_menu_bar_new ();
  gtk_widget_ref (menu_bar);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "menu_bar", menu_bar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (menu_bar);
  gtk_box_pack_start (GTK_BOX (main_box), menu_bar, FALSE, FALSE, 0);

  ggz1 = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (ggz1)->child),
                                   "_GGZ");
  gtk_widget_add_accelerator (ggz1, "activate_item", accel_group,
                              tmp_key, GDK_MOD1_MASK, 0);
  gtk_widget_ref (ggz1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "ggz1", ggz1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ggz1);
  gtk_container_add (GTK_CONTAINER (menu_bar), ggz1);

  ggz1_menu = gtk_menu_new ();
  gtk_widget_ref (ggz1_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "ggz1_menu", ggz1_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (ggz1), ggz1_menu);
  ggz1_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (ggz1_menu));

  connect2 = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (connect2)->child),
                                   "_Connect");
  gtk_widget_add_accelerator (connect2, "activate_item", ggz1_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (connect2);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "connect2", connect2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (connect2);
  gtk_container_add (GTK_CONTAINER (ggz1_menu), connect2);
  gtk_widget_add_accelerator (connect2, "activate", accel_group,
                              GDK_C, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  disconnect1 = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (disconnect1)->child),
                                   "_Disconnect");
  gtk_widget_add_accelerator (disconnect1, "activate_item", ggz1_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (disconnect1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "disconnect1", disconnect1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (disconnect1);
  gtk_container_add (GTK_CONTAINER (ggz1_menu), disconnect1);
  gtk_widget_add_accelerator (disconnect1, "activate", accel_group,
                              GDK_D, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  separator4 = gtk_menu_item_new ();
  gtk_widget_ref (separator4);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "separator4", separator4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator4);
  gtk_container_add (GTK_CONTAINER (ggz1_menu), separator4);
  gtk_widget_set_sensitive (separator4, FALSE);

  login1 = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (login1)->child),
                                   "_Login");
  gtk_widget_add_accelerator (login1, "activate_item", ggz1_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (login1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "login1", login1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (login1);
  gtk_container_add (GTK_CONTAINER (ggz1_menu), login1);

  logout1 = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (logout1)->child),
                                   "L_ogout");
  gtk_widget_add_accelerator (logout1, "activate_item", ggz1_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (logout1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "logout1", logout1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (logout1);
  gtk_container_add (GTK_CONTAINER (ggz1_menu), logout1);

  separator3 = gtk_menu_item_new ();
  gtk_widget_ref (separator3);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "separator3", separator3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator3);
  gtk_container_add (GTK_CONTAINER (ggz1_menu), separator3);
  gtk_widget_set_sensitive (separator3, FALSE);

  exit1 = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (exit1)->child),
                                   "E_xit");
  gtk_widget_add_accelerator (exit1, "activate_item", ggz1_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (exit1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "exit1", exit1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (exit1);
  gtk_container_add (GTK_CONTAINER (ggz1_menu), exit1);
  gtk_widget_add_accelerator (exit1, "activate", accel_group,
                              GDK_X, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  game_menu = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (game_menu)->child),
                                   "G_ame");
  gtk_widget_add_accelerator (game_menu, "activate_item", accel_group,
                              tmp_key, GDK_MOD1_MASK, 0);
  gtk_widget_ref (game_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "game_menu", game_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (game_menu);
  gtk_container_add (GTK_CONTAINER (menu_bar), game_menu);

  game_menu_menu = gtk_menu_new ();
  gtk_widget_ref (game_menu_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "game_menu_menu", game_menu_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (game_menu), game_menu_menu);
  game_menu_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (game_menu_menu));

  new_game2 = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (new_game2)->child),
                                   "_Launch Game");
  gtk_widget_add_accelerator (new_game2, "activate_item", game_menu_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (new_game2);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "new_game2", new_game2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (new_game2);
  gtk_container_add (GTK_CONTAINER (game_menu_menu), new_game2);
  gtk_widget_add_accelerator (new_game2, "activate", accel_group,
                              GDK_L, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  join_game1 = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (join_game1)->child),
                                   "_Join Game");
  gtk_widget_add_accelerator (join_game1, "activate_item", game_menu_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (join_game1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "join_game1", join_game1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (join_game1);
  gtk_container_add (GTK_CONTAINER (game_menu_menu), join_game1);

  edit1 = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (edit1)->child),
                                   "_Edit");
  gtk_widget_add_accelerator (edit1, "activate_item", accel_group,
                              tmp_key, GDK_MOD1_MASK, 0);
  gtk_widget_ref (edit1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "edit1", edit1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (edit1);
  gtk_container_add (GTK_CONTAINER (menu_bar), edit1);

  edit1_menu = gtk_menu_new ();
  gtk_widget_ref (edit1_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "edit1_menu", edit1_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (edit1), edit1_menu);
  edit1_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (edit1_menu));

  properties1 = gtk_menu_item_new_with_label ("Properties");
  gtk_widget_ref (properties1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "properties1", properties1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (properties1);
  gtk_container_add (GTK_CONTAINER (edit1_menu), properties1);

  view_menu = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (view_menu)->child),
                                   "_View");
  gtk_widget_add_accelerator (view_menu, "activate_item", accel_group,
                              tmp_key, GDK_MOD1_MASK, 0);
  gtk_widget_ref (view_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "view_menu", view_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (view_menu);
  gtk_container_add (GTK_CONTAINER (menu_bar), view_menu);

  view_menu_menu = gtk_menu_new ();
  gtk_widget_ref (view_menu_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "view_menu_menu", view_menu_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (view_menu), view_menu_menu);
  view_menu_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (view_menu_menu));

  game_types1 = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (game_types1)->child),
                                   "Game _Types");
  gtk_widget_add_accelerator (game_types1, "activate_item", view_menu_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (game_types1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "game_types1", game_types1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (game_types1);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), game_types1);

  motd1 = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (motd1)->child),
                                   "_MOTD");
  gtk_widget_add_accelerator (motd1, "activate_item", view_menu_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (motd1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "motd1", motd1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (motd1);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), motd1);

  help1 = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (help1)->child),
                                   "_Help");
  gtk_widget_add_accelerator (help1, "activate_item", accel_group,
                              tmp_key, GDK_MOD1_MASK, 0);
  gtk_widget_ref (help1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "help1", help1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (help1);
  gtk_container_add (GTK_CONTAINER (menu_bar), help1);

  help1_menu = gtk_menu_new ();
  gtk_widget_ref (help1_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "help1_menu", help1_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (help1), help1_menu);
  help1_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (help1_menu));

  about1 = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (about1)->child),
                                   "_About");
  gtk_widget_add_accelerator (about1, "activate_item", help1_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (about1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "about1", about1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (about1);
  gtk_container_add (GTK_CONTAINER (help1_menu), about1);

  toolbar = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_TEXT);
  gtk_widget_ref (toolbar);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "toolbar", toolbar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar);
  gtk_box_pack_start (GTK_BOX (main_box), toolbar, FALSE, FALSE, 0);

  launch_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Launch Game",
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (launch_button);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "launch_button", launch_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (launch_button);

  join_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Join Game",
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (join_button);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "join_button", join_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (join_button);

  prefs_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Change Prefs",
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (prefs_button);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "prefs_button", prefs_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (prefs_button);

  stat_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Stats",
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (stat_button);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "stat_button", stat_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (stat_button);

  logout_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Logout",
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (logout_button);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "logout_button", logout_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (logout_button);

  exit_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Exit",
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (exit_button);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "exit_button", exit_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (exit_button);

  v_pane = gtk_vpaned_new ();
  gtk_widget_ref (v_pane);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "v_pane", v_pane,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (v_pane);
  gtk_box_pack_start (GTK_BOX (main_box), v_pane, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (v_pane), 10);
  gtk_paned_set_gutter_size (GTK_PANED (v_pane), 15);
  gtk_paned_set_position (GTK_PANED (v_pane), 160);

  table_box = gtk_vbox_new (FALSE, 5);
  gtk_widget_ref (table_box);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "table_box", table_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table_box);
  gtk_paned_pack1 (GTK_PANED (v_pane), table_box, FALSE, TRUE);

  table_label = gtk_label_new ("Game Tables");
  gtk_widget_ref (table_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "table_label", table_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table_label);
  gtk_box_pack_start (GTK_BOX (table_box), table_label, FALSE, FALSE, 0);

  table_scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (table_scroll);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "table_scroll", table_scroll,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table_scroll);
  gtk_box_pack_start (GTK_BOX (table_box), table_scroll, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (table_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  table_tree = gtk_clist_new (7);
  gtk_widget_ref (table_tree);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "table_tree", table_tree,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table_tree);
  gtk_container_add (GTK_CONTAINER (table_scroll), table_tree);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 0, 12);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 1, 64);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 2, 80);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 3, 52);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 4, 72);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 5, 60);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 6, 162);
  gtk_clist_column_titles_show (GTK_CLIST (table_tree));

  label47 = gtk_label_new (" ");
  gtk_widget_ref (label47);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label47", label47,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label47);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 0, label47);

  label48 = gtk_label_new ("Table No.");
  gtk_widget_ref (label48);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label48", label48,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label48);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 1, label48);

  label49 = gtk_label_new ("Game Type");
  gtk_widget_ref (label49);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label49", label49,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label49);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 2, label49);

  label50 = gtk_label_new ("Seats");
  gtk_widget_ref (label50);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label50", label50,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label50);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 3, label50);

  label51 = gtk_label_new ("Open Seats");
  gtk_widget_ref (label51);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label51", label51,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label51);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 4, label51);

  label52 = gtk_label_new ("Humans");
  gtk_widget_ref (label52);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label52", label52,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label52);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 5, label52);

  label53 = gtk_label_new ("Description");
  gtk_widget_ref (label53);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label53", label53,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label53);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 6, label53);

  h_pane = gtk_hpaned_new ();
  gtk_widget_ref (h_pane);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "h_pane", h_pane,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (h_pane);
  gtk_paned_pack2 (GTK_PANED (v_pane), h_pane, TRUE, TRUE);
  gtk_container_set_border_width (GTK_CONTAINER (h_pane), 5);
  gtk_paned_set_gutter_size (GTK_PANED (h_pane), 10);
  gtk_paned_set_position (GTK_PANED (h_pane), 160);

  player_box = gtk_vbox_new (FALSE, 5);
  gtk_widget_ref (player_box);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "player_box", player_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_box);
  gtk_paned_pack1 (GTK_PANED (h_pane), player_box, FALSE, TRUE);

  player_label = gtk_label_new ("Players");
  gtk_widget_ref (player_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "player_label", player_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_label);
  gtk_box_pack_start (GTK_BOX (player_box), player_label, FALSE, FALSE, 0);

  player_scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (player_scroll);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "player_scroll", player_scroll,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_scroll);
  gtk_box_pack_start (GTK_BOX (player_box), player_scroll, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (player_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  player_list = gtk_clist_new (2);
  gtk_widget_ref (player_list);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "player_list", player_list,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_list);
  gtk_container_add (GTK_CONTAINER (player_scroll), player_list);
  gtk_clist_set_column_width (GTK_CLIST (player_list), 0, 80);
  gtk_clist_set_column_width (GTK_CLIST (player_list), 1, 48);
  gtk_clist_column_titles_show (GTK_CLIST (player_list));

  label6 = gtk_label_new ("Name");
  gtk_widget_ref (label6);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label6", label6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label6);
  gtk_clist_set_column_widget (GTK_CLIST (player_list), 0, label6);

  label7 = gtk_label_new ("Table");
  gtk_widget_ref (label7);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label7", label7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label7);
  gtk_clist_set_column_widget (GTK_CLIST (player_list), 1, label7);

  chat_box = gtk_vbox_new (FALSE, 5);
  gtk_widget_ref (chat_box);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "chat_box", chat_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_box);
  gtk_paned_pack2 (GTK_PANED (h_pane), chat_box, TRUE, FALSE);
  gtk_widget_set_usize (chat_box, -2, 280);
  gtk_container_set_border_width (GTK_CONTAINER (chat_box), 5);

  hbox16 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox16);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "hbox16", hbox16,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox16);
  gtk_box_pack_start (GTK_BOX (chat_box), hbox16, FALSE, FALSE, 0);

  label54 = gtk_label_new ("Current Room  ");
  gtk_widget_ref (label54);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label54", label54,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label54);
  gtk_box_pack_start (GTK_BOX (hbox16), label54, FALSE, FALSE, 0);

  game_room = gtk_combo_new ();
  gtk_widget_ref (game_room);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "game_room", game_room,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (game_room);
  gtk_box_pack_start (GTK_BOX (hbox16), game_room, TRUE, TRUE, 0);

  combo_entry2 = GTK_COMBO (game_room)->entry;
  gtk_widget_ref (combo_entry2);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "combo_entry2", combo_entry2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo_entry2);

  chat_scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (chat_scroll);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "chat_scroll", chat_scroll,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_scroll);
  gtk_box_pack_start (GTK_BOX (chat_box), chat_scroll, TRUE, TRUE, 0);
  gtk_widget_set_usize (chat_scroll, 100, -2);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (chat_scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  chat_text = gtk_text_new (NULL, NULL);
  gtk_widget_ref (chat_text);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "chat_text", chat_text,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_text);
  gtk_container_add (GTK_CONTAINER (chat_scroll), chat_text);
  gtk_widget_set_usize (chat_text, 100, 132);

  chat_separator = gtk_hseparator_new ();
  gtk_widget_ref (chat_separator);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "chat_separator", chat_separator,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_separator);
  gtk_box_pack_start (GTK_BOX (chat_box), chat_separator, FALSE, FALSE, 5);

  msg_box = gtk_hbox_new (FALSE, 5);
  gtk_widget_ref (msg_box);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "msg_box", msg_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (msg_box);
  gtk_box_pack_start (GTK_BOX (chat_box), msg_box, FALSE, FALSE, 0);

  msg_label = gtk_label_new ("Message: ");
  gtk_widget_ref (msg_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "msg_label", msg_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (msg_label);
  gtk_box_pack_start (GTK_BOX (msg_box), msg_label, FALSE, FALSE, 0);

  msg_entry = gtk_entry_new ();
  gtk_widget_ref (msg_entry);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "msg_entry", msg_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (msg_entry);
  gtk_box_pack_start (GTK_BOX (msg_box), msg_entry, TRUE, TRUE, 0);

  msg_button = gtk_button_new_with_label ("Send");
  gtk_widget_ref (msg_button);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "msg_button", msg_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (msg_button);
  gtk_box_pack_start (GTK_BOX (msg_box), msg_button, FALSE, FALSE, 0);

  statusbar = gtk_statusbar_new ();
  gtk_widget_ref (statusbar);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "statusbar", statusbar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (statusbar);
  gtk_box_pack_end (GTK_BOX (main_box), statusbar, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (statusbar), 5);

  gtk_signal_connect(GTK_OBJECT (table_tree), "select_row",
                        GTK_SIGNAL_FUNC(ggz_table_select_row_callback),
                        NULL);
  gtk_signal_connect (GTK_OBJECT (main_win), "delete_event",
                      GTK_SIGNAL_FUNC (exit_dlg),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (main_win), "destroy",
                      GTK_SIGNAL_FUNC (gtk_main_quit),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (new_game2), "activate",
                      GTK_SIGNAL_FUNC (ggz_get_game_options),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (join_game1), "activate",
                      GTK_SIGNAL_FUNC (ggz_join_game),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (logout1), "activate",
                      GTK_SIGNAL_FUNC (ggz_logout),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (login1), "activate",
                      GTK_SIGNAL_FUNC (ggz_login),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (exit1), "activate",
                      GTK_SIGNAL_FUNC (exit_dlg),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (game_types1), "activate",
                      GTK_SIGNAL_FUNC (ggz_get_types),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (launch_button), "clicked",
                      GTK_SIGNAL_FUNC (ggz_get_game_options),
                      main_win);
  gtk_signal_connect (GTK_OBJECT (join_button), "clicked",
                      GTK_SIGNAL_FUNC (ggz_join_game),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (logout_button), "clicked",
                      GTK_SIGNAL_FUNC (ggz_logout),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (exit_button), "clicked",
                      GTK_SIGNAL_FUNC (exit_dlg),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (msg_entry), "activate",
                      GTK_SIGNAL_FUNC (ggz_input_chat_msg),
                      msg_entry);
  gtk_signal_connect (GTK_OBJECT (msg_button), "clicked",
                      GTK_SIGNAL_FUNC (ggz_input_chat_msg),
                      msg_entry);
  gtk_signal_connect_object (GTK_OBJECT (player_list), "event",
  		             GTK_SIGNAL_FUNC (ggz_event_players), GTK_OBJECT (mnu_players));
  gtk_signal_connect_object (GTK_OBJECT (table_tree), "event", GTK_SIGNAL_FUNC (ggz_event_tables),
  			     GTK_OBJECT (mnu_tables));

  gtk_window_add_accel_group (GTK_WINDOW (main_win), accel_group);
  gtk_text_set_word_wrap( GTK_TEXT(chat_text), TRUE);
  return main_win;
}
 

/*                              *
 *           Callbacks          *
 *                              */

void ggz_join_game(GtkButton * button, gpointer user_data)
{
        dbg_msg("joining game");
        es_write_int(connection.sock, REQ_TABLE_JOIN);
        es_write_int(connection.sock, selected_table);
}
                        
void ggz_logout(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *tmp;

        dbg_msg("Logging out");
        es_write_int(connection.sock, REQ_LOGOUT);

	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "table_tree");
        gtk_clist_clear(GTK_CLIST(tmp));
	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "player_list");
        gtk_clist_clear(GTK_CLIST(tmp));
	display_chat("< <  > >","Disconnected from server.");
}
        
                        
void ggz_get_types(GtkMenuItem * menuitem, gpointer user_data)
{
        gchar verbose = 1;
                 
        es_write_int(connection.sock, REQ_LIST_TYPES);
        write(connection.sock, &verbose, 1);
}
                                
 
void ggz_get_players(GtkMenuItem * menuitem, gpointer user_data)
{
        es_write_int(connection.sock, REQ_LIST_PLAYERS);
}

        
void ggz_get_tables(GtkMenuItem * menuitem, gpointer user_data)
{       
        es_write_int(connection.sock, REQ_LIST_TABLES);
        es_write_int(connection.sock, -1);
}

void ggz_input_chat_msg(GtkWidget * widget, gpointer user_data)
{
        if (!connection.connected) {
                err_dlg("Not Connected");
                return;
        }
                        
        if (strcmp(gtk_entry_get_text(GTK_ENTRY(user_data)), "") != 0
            && es_write_int(connection.sock, REQ_CHAT) == 0)
                        
                es_write_string(connection.sock,
                                gtk_entry_get_text(GTK_ENTRY(user_data)));
        
        gtk_entry_set_text(GTK_ENTRY(user_data), "");
}
                        
void ggz_table_select_row_callback(GtkWidget *widget, gint row, gint column,
                               GdkEventButton *event, gpointer data)
{
        gchar *text;
        gint i;
        
        gtk_clist_get_text(GTK_CLIST(widget), row, 1, &text);
        selected_table = atoi(text);
        gtk_clist_get_text(GTK_CLIST(widget), row, 2, &text);
        for(i=0;i<game_types.count;i++)
        {
                if(!strcmp(text,game_types.info[i].name))
                        selected_type=i;
        }
}


void ggz_get_game_options(GtkButton * button, gpointer user_data)
{
        if (!connection.connected)
                warn_dlg("Not connected!");
        else {
                dlg_launch = create_dlgLaunch();
                gtk_widget_show(dlg_launch);
        }
}

gint ggz_event_tables( GtkWidget *widget, GdkEvent *event )
{
	GtkWidget *tmp;
	gint row, col;

	if (event->type == GDK_BUTTON_PRESS && event->button.button == 3)
	{
		tmp = gtk_object_get_data(GTK_OBJECT(mnu_tables), "launch1");
		gtk_widget_show(GTK_WIDGET(tmp));
		tmp = gtk_object_get_data(GTK_OBJECT(mnu_tables), "join1");
		gtk_widget_hide(GTK_WIDGET(tmp));

		tmp = gtk_object_get_data(GTK_OBJECT(main_win), "table_tree");
		if (gtk_clist_get_selection_info(GTK_CLIST(tmp), event->button.x, event->button.y, &row, &col) > 0)
		{
			tmp = gtk_object_get_data(GTK_OBJECT(mnu_tables), "launch1");
			gtk_widget_hide(GTK_WIDGET(tmp));
			tmp = gtk_object_get_data(GTK_OBJECT(mnu_tables), "join1");
			gtk_widget_show(GTK_WIDGET(tmp));

			tmp = gtk_object_get_data(GTK_OBJECT(main_win), "table_tree");
			gtk_clist_unselect_all (GTK_CLIST (tmp));
			gtk_clist_select_row (GTK_CLIST (tmp), row, 0);
		}

		gtk_menu_popup (GTK_MENU (mnu_tables), NULL, NULL, NULL, NULL,
			event->button.button, event->button.time);
        }
	if (event->type == GDK_2BUTTON_PRESS)
	{
        	es_write_int(connection.sock, REQ_TABLE_JOIN);
	        es_write_int(connection.sock, selected_table);
	}

	return 0;
}

gint ggz_event_players( GtkWidget *widget, GdkEvent *event )
{
	if (event->type == GDK_BUTTON_PRESS && event->button.button == 3)
	{
		GdkEventButton *bevent = (GdkEventButton *) event;
		gtk_menu_popup (GTK_MENU (mnu_players), NULL, NULL, NULL, NULL,
			bevent->button, bevent->time);
        }

	return 0;
}

void ggz_login()
{
	dlg_login = create_dlg_login();
	login_bad_name();
	gtk_widget_show(dlg_login);
}
