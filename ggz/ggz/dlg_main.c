#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "datatypes.h"
#include "dlg_error.h"
#include "dlg_exit.h"
#include "dlg_launch.h"
#include "dlg_main.h"
#include "easysock.h"
#include "err_func.h"
#include "protocols.h"


/* Globals neaded by this dialog */
extern GtkWidget *dlg_launch;
extern int selected_table;
extern int selected_type;
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

GtkWidget*
create_main_win (void)
{
  GtkWidget *main_win;
  GtkWidget *main_box;
  GtkWidget *menu_bar;
  guint tmp_key;
  GtkWidget *game_menu;
  GtkWidget *game_menu_menu;
  GtkAccelGroup *game_menu_menu_accels;
  GtkWidget *new_game2;
  GtkWidget *join_game1;
  GtkWidget *separator1;
  GtkWidget *logout1;
  GtkWidget *separator2;
  GtkWidget *exit1;
  GtkWidget *view_menu;
  GtkWidget *view_menu_menu;
  GtkAccelGroup *view_menu_menu_accels;
  GtkWidget *game_types1;
  GtkWidget *players1;
  GtkWidget *tables1;
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
  GtkWidget *label5;
  GtkWidget *label8;
  GtkWidget *label9;
  GtkWidget *label10;
  GtkWidget *label11;
  GtkWidget *label12;
  GtkWidget *label13;
  GtkWidget *h_pane;
  GtkWidget *player_box;
  GtkWidget *player_label;
  GtkWidget *player_scroll;
  GtkWidget *player_list;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *chat_box;
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
  gtk_window_set_title (GTK_WINDOW (main_win), "Gnu Gaming Zone");
  gtk_window_set_default_size (GTK_WINDOW (main_win), 500, 450);
  gtk_window_set_policy (GTK_WINDOW (main_win), TRUE, TRUE, FALSE);

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

  game_menu = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (game_menu)->child),
                                   "_Game");
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

  separator1 = gtk_menu_item_new ();
  gtk_widget_ref (separator1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "separator1", separator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator1);
  gtk_container_add (GTK_CONTAINER (game_menu_menu), separator1);
  gtk_widget_set_sensitive (separator1, FALSE);

  logout1 = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (logout1)->child),
                                   "L_ogout");
  gtk_widget_add_accelerator (logout1, "activate_item", game_menu_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (logout1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "logout1", logout1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (logout1);
  gtk_container_add (GTK_CONTAINER (game_menu_menu), logout1);

  separator2 = gtk_menu_item_new ();
  gtk_widget_ref (separator2);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "separator2", separator2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator2);
  gtk_container_add (GTK_CONTAINER (game_menu_menu), separator2);
  gtk_widget_set_sensitive (separator2, FALSE);

  exit1 = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (exit1)->child),
                                   "E_xit");
  gtk_widget_add_accelerator (exit1, "activate_item", game_menu_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_ref (exit1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "exit1", exit1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (exit1);
  gtk_container_add (GTK_CONTAINER (game_menu_menu), exit1);

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

  game_types1 = gtk_menu_item_new_with_label ("Game Types");
  gtk_widget_ref (game_types1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "game_types1", game_types1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (game_types1);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), game_types1);

  players1 = gtk_menu_item_new_with_label ("Players");
  gtk_widget_ref (players1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "players1", players1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (players1);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), players1);

  tables1 = gtk_menu_item_new_with_label ("Tables");
  gtk_widget_ref (tables1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "tables1", tables1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (tables1);
  gtk_container_add (GTK_CONTAINER (view_menu_menu), tables1);

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
  gtk_container_add (GTK_CONTAINER (v_pane), table_box);

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

  table_tree = gtk_ctree_new (7,0);
  gtk_widget_ref (table_tree);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "table_tree", table_tree,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table_tree);
  gtk_container_add (GTK_CONTAINER (table_scroll), table_tree);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 0, 10);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 1, 70);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 2, 70);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 3, 70);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 4, 70);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 5, 70);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 6, 1);
  gtk_clist_column_titles_show (GTK_CLIST (table_tree));

  label5 = gtk_label_new (" ");
  gtk_widget_ref (label5);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label5", label5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label5);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 0, label5);

  label8 = gtk_label_new ("Table No.");
  gtk_widget_ref (label8);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label8", label8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label8);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 1, label8);

  label9 = gtk_label_new ("Game Type");
  gtk_widget_ref (label9);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label9", label9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label9);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 2, label9);

  label10 = gtk_label_new ("Seats");
  gtk_widget_ref (label10);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label10", label10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label10);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 3, label10);

  label11 = gtk_label_new ("Open Seats");
  gtk_widget_ref (label11);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label11", label11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label11);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 4, label11);

  label12 = gtk_label_new ("Humans");
  gtk_widget_ref (label12);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label12", label12,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label12);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 5, label12);

  label13 = gtk_label_new ("Description");
  gtk_widget_ref (label13);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label13", label13,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label13);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 6, label13);
  gtk_signal_connect(GTK_OBJECT (table_tree), "select_row", 
			GTK_SIGNAL_FUNC(ggz_table_select_row_callback),
			NULL);

  h_pane = gtk_hpaned_new ();
  gtk_widget_ref (h_pane);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "h_pane", h_pane,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (h_pane);
  gtk_container_add (GTK_CONTAINER (v_pane), h_pane);
  gtk_container_set_border_width (GTK_CONTAINER (h_pane), 5);
  gtk_paned_set_gutter_size (GTK_PANED (h_pane), 10);
  gtk_paned_set_position (GTK_PANED (h_pane), 160);

  player_box = gtk_vbox_new (FALSE, 5);
  gtk_widget_ref (player_box);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "player_box", player_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_box);
  gtk_container_add (GTK_CONTAINER (h_pane), player_box);

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
  gtk_clist_set_column_width (GTK_CLIST (player_list), 1, 50);
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
  gtk_container_add (GTK_CONTAINER (h_pane), chat_box);
  gtk_container_set_border_width (GTK_CONTAINER (chat_box), 5);

  chat_scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (chat_scroll);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "chat_scroll", chat_scroll,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_scroll);
  gtk_box_pack_start (GTK_BOX (chat_box), chat_scroll, TRUE, TRUE, 0);
  gtk_widget_set_usize (chat_scroll, 100, -2);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (chat_scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  chat_text = gtk_text_new (NULL, NULL);
  gtk_text_set_word_wrap( GTK_TEXT(chat_text), TRUE);
  gtk_widget_ref (chat_text);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "chat_text", chat_text,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (chat_text);
  gtk_container_add (GTK_CONTAINER (chat_scroll), chat_text);
  gtk_widget_set_usize (chat_text, 100, 146);

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
  gtk_signal_connect (GTK_OBJECT (exit1), "activate",
                      GTK_SIGNAL_FUNC (exit_dlg),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (game_types1), "activate",
                      GTK_SIGNAL_FUNC (ggz_get_types),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (players1), "activate",
                      GTK_SIGNAL_FUNC (ggz_get_players),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (tables1), "activate",
                      GTK_SIGNAL_FUNC (ggz_get_tables),
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

  gtk_window_add_accel_group (GTK_WINDOW (main_win), accel_group);

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
        dbg_msg("Logging out");
        es_write_int(connection.sock, REQ_LOGOUT);
}
        
                        
void ggz_get_types(GtkMenuItem * menuitem, gpointer user_data)
{
        char verbose = 1;
                 
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
        int i;
        
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

