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
#include "interface.h"
#include "support.h"

GtkWidget*
create_dlg_login (void)
{
  GtkWidget *dlg_login;
  GtkWidget *main_box;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *name_frame;
  GtkWidget *info_box;
  GtkWidget *name_box;
  GtkWidget *name_label;
  GtkWidget *name_entry;
  GtkWidget *pass_box;
  GtkWidget *pass_label;
  GtkWidget *pass_entry;
  GtkWidget *opt_frame;
  GtkWidget *radio_box;
  GSList *login_type_group = NULL;
  GtkWidget *normal_radio;
  GtkWidget *anon_radio;
  GtkWidget *first_radio;
  GtkWidget *server_frame;
  GtkWidget *server_box;
  GtkWidget *host_label;
  GtkWidget *host_entry;
  GtkWidget *port_entry;
  GtkWidget *port_label;
  GtkWidget *hseparator;
  GtkWidget *button_box;
  GtkWidget *connect_button;
  GtkWidget *cancel_button;
  GtkWidget *details_button;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  dlg_login = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_object_set_data (GTK_OBJECT (dlg_login), "dlg_login", dlg_login);
  gtk_window_set_title (GTK_WINDOW (dlg_login), "Login");
  gtk_window_set_position (GTK_WINDOW (dlg_login), GTK_WIN_POS_MOUSE);
  gtk_window_set_policy (GTK_WINDOW (dlg_login), FALSE, FALSE, FALSE);

  main_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (main_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "main_box", main_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (main_box);
  gtk_container_add (GTK_CONTAINER (dlg_login), main_box);
  gtk_container_set_border_width (GTK_CONTAINER (main_box), 5);

  vbox = gtk_vbox_new (FALSE, 5);
  gtk_widget_ref (vbox);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "vbox", vbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox);
  gtk_box_pack_start (GTK_BOX (main_box), vbox, FALSE, FALSE, 0);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "hbox", hbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 5);

  name_frame = gtk_frame_new (NULL);
  gtk_widget_ref (name_frame);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "name_frame", name_frame,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (name_frame);
  gtk_box_pack_start (GTK_BOX (hbox), name_frame, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (name_frame), 5);

  info_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (info_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "info_box", info_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (info_box);
  gtk_container_add (GTK_CONTAINER (name_frame), info_box);
  gtk_container_set_border_width (GTK_CONTAINER (info_box), 10);

  name_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (name_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "name_box", name_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (name_box);
  gtk_box_pack_start (GTK_BOX (info_box), name_box, TRUE, TRUE, 0);

  name_label = gtk_label_new ("Username : ");
  gtk_widget_ref (name_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "name_label", name_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (name_label);
  gtk_box_pack_start (GTK_BOX (name_box), name_label, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (name_label), GTK_JUSTIFY_RIGHT);

  name_entry = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (name_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "name_entry", name_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (name_entry);
  gtk_box_pack_start (GTK_BOX (name_box), name_entry, TRUE, TRUE, 0);
  gtk_widget_set_usize (name_entry, 96, -2);
  gtk_tooltips_set_tip (tooltips, name_entry, "Enter your login name here", NULL);

  pass_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (pass_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "pass_box", pass_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pass_box);
  gtk_box_pack_start (GTK_BOX (info_box), pass_box, TRUE, TRUE, 0);

  pass_label = gtk_label_new ("Password : ");
  gtk_widget_ref (pass_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "pass_label", pass_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pass_label);
  gtk_box_pack_start (GTK_BOX (pass_box), pass_label, FALSE, FALSE, 0);

  pass_entry = gtk_entry_new ();
  gtk_widget_ref (pass_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "pass_entry", pass_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pass_entry);
  gtk_box_pack_start (GTK_BOX (pass_box), pass_entry, TRUE, TRUE, 0);
  gtk_widget_set_usize (pass_entry, 96, -2);
  gtk_tooltips_set_tip (tooltips, pass_entry, "Enter your password here", NULL);
  gtk_entry_set_visibility (GTK_ENTRY (pass_entry), FALSE);

  opt_frame = gtk_frame_new (NULL);
  gtk_widget_ref (opt_frame);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "opt_frame", opt_frame,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_frame);
  gtk_box_pack_start (GTK_BOX (hbox), opt_frame, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (opt_frame), 5);

  radio_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (radio_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "radio_box", radio_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radio_box);
  gtk_container_add (GTK_CONTAINER (opt_frame), radio_box);
  gtk_container_set_border_width (GTK_CONTAINER (radio_box), 5);

  normal_radio = gtk_radio_button_new_with_label (login_type_group, "Normal Login");
  login_type_group = gtk_radio_button_group (GTK_RADIO_BUTTON (normal_radio));
  gtk_widget_ref (normal_radio);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "normal_radio", normal_radio,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (normal_radio);
  gtk_box_pack_start (GTK_BOX (radio_box), normal_radio, FALSE, FALSE, 0);

  anon_radio = gtk_radio_button_new_with_label (login_type_group, "Anonymous Login");
  login_type_group = gtk_radio_button_group (GTK_RADIO_BUTTON (anon_radio));
  gtk_widget_ref (anon_radio);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "anon_radio", anon_radio,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (anon_radio);
  gtk_box_pack_start (GTK_BOX (radio_box), anon_radio, FALSE, FALSE, 0);

  first_radio = gtk_radio_button_new_with_label (login_type_group, "First-time Login");
  login_type_group = gtk_radio_button_group (GTK_RADIO_BUTTON (first_radio));
  gtk_widget_ref (first_radio);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "first_radio", first_radio,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (first_radio);
  gtk_box_pack_start (GTK_BOX (radio_box), first_radio, FALSE, FALSE, 0);

  server_frame = gtk_frame_new (NULL);
  gtk_widget_ref (server_frame);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "server_frame", server_frame,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (server_frame);
  gtk_box_pack_start (GTK_BOX (vbox), server_frame, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (server_frame), 5);

  server_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (server_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "server_box", server_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (server_box);
  gtk_container_add (GTK_CONTAINER (server_frame), server_box);
  gtk_container_set_border_width (GTK_CONTAINER (server_box), 10);

  host_label = gtk_label_new ("Server:");
  gtk_widget_ref (host_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "host_label", host_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (host_label);
  gtk_box_pack_start (GTK_BOX (server_box), host_label, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (host_label), GTK_JUSTIFY_RIGHT);

  host_entry = gtk_entry_new_with_max_length (256);
  gtk_widget_ref (host_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "host_entry", host_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (host_entry);
  gtk_box_pack_start (GTK_BOX (server_box), host_entry, FALSE, FALSE, 5);

  port_entry = gtk_entry_new ();
  gtk_widget_ref (port_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "port_entry", port_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (port_entry);
  gtk_box_pack_end (GTK_BOX (server_box), port_entry, FALSE, FALSE, 5);
  gtk_widget_set_usize (port_entry, 50, -2);

  port_label = gtk_label_new ("Port:");
  gtk_widget_ref (port_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "port_label", port_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (port_label);
  gtk_box_pack_end (GTK_BOX (server_box), port_label, FALSE, FALSE, 0);

  hseparator = gtk_hseparator_new ();
  gtk_widget_ref (hseparator);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "hseparator", hseparator,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hseparator);
  gtk_box_pack_start (GTK_BOX (vbox), hseparator, TRUE, TRUE, 0);

  button_box = gtk_hbutton_box_new ();
  gtk_widget_ref (button_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "button_box", button_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button_box);
  gtk_box_pack_start (GTK_BOX (vbox), button_box, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (button_box), 5);

  connect_button = gtk_button_new_with_label ("Connect");
  gtk_widget_ref (connect_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "connect_button", connect_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (connect_button);
  gtk_container_add (GTK_CONTAINER (button_box), connect_button);
  GTK_WIDGET_SET_FLAGS (connect_button, GTK_CAN_DEFAULT);

  cancel_button = gtk_button_new_with_label ("Cancel");
  gtk_widget_ref (cancel_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "cancel_button", cancel_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cancel_button);
  gtk_container_add (GTK_CONTAINER (button_box), cancel_button);
  GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);

  details_button = gtk_button_new_with_label ("Details >>");
  gtk_widget_ref (details_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "details_button", details_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (details_button);
  gtk_container_add (GTK_CONTAINER (button_box), details_button);
  gtk_widget_set_usize (details_button, 93, -2);
  GTK_WIDGET_SET_FLAGS (details_button, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (dlg_login), "delete_event",
                      GTK_SIGNAL_FUNC (gtk_widget_destroy),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (dlg_login), "destroy_event",
                      GTK_SIGNAL_FUNC (gtk_widget_destroy),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (dlg_login), "realize",
                      GTK_SIGNAL_FUNC (fill_defaults),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (anon_radio), "toggled",
                      GTK_SIGNAL_FUNC (anon_toggled),
                      dlg_login);
  gtk_signal_connect (GTK_OBJECT (connect_button), "clicked",
                      GTK_SIGNAL_FUNC (input_options),
                      dlg_login);
  gtk_signal_connect (GTK_OBJECT (connect_button), "clicked",
                      GTK_SIGNAL_FUNC (start_session),
                      dlg_login);
  gtk_signal_connect_object (GTK_OBJECT (cancel_button), "clicked",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy),
                             GTK_OBJECT (dlg_login));
  gtk_signal_connect (GTK_OBJECT (details_button), "clicked",
                      GTK_SIGNAL_FUNC (show_details),
                      dlg_login);

  gtk_widget_grab_default (connect_button);
  gtk_object_set_data (GTK_OBJECT (dlg_login), "tooltips", tooltips);

  return dlg_login;
}

GtkWidget*
create_dlg_details (void)
{
  GtkWidget *dlg_details;
  GtkWidget *dialog_vbox1;
  GtkWidget *scrolled_window;
  GtkWidget *text;
  GtkWidget *dialog_action_area1;
  GtkWidget *hbuttonbox1;
  GtkWidget *cancel_button;

  dlg_details = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (dlg_details), "dlg_details", dlg_details);
  gtk_window_set_title (GTK_WINDOW (dlg_details), "Connection Messages");
  gtk_window_set_position (GTK_WINDOW (dlg_details), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size (GTK_WINDOW (dlg_details), 400, 300);
  gtk_window_set_policy (GTK_WINDOW (dlg_details), TRUE, TRUE, FALSE);

  dialog_vbox1 = GTK_DIALOG (dlg_details)->vbox;
  gtk_object_set_data (GTK_OBJECT (dlg_details), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);

  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolled_window);
  gtk_object_set_data_full (GTK_OBJECT (dlg_details), "scrolled_window", scrolled_window,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolled_window);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), scrolled_window, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 10);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  text = gtk_text_new (NULL, NULL);
  gtk_widget_ref (text);
  gtk_object_set_data_full (GTK_OBJECT (dlg_details), "text", text,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (text);
  gtk_container_add (GTK_CONTAINER (scrolled_window), text);

  dialog_action_area1 = GTK_DIALOG (dlg_details)->action_area;
  gtk_object_set_data (GTK_OBJECT (dlg_details), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_details), "hbuttonbox1", hbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), hbuttonbox1, TRUE, TRUE, 0);

  cancel_button = gtk_button_new_with_label ("Cancel");
  gtk_widget_ref (cancel_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_details), "cancel_button", cancel_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cancel_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), cancel_button);
  GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (cancel_button), "clicked",
                      GTK_SIGNAL_FUNC (cancel_details),
                      NULL);

  return dlg_details;
}

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

  table_tree = gtk_ctree_new (10, 0);
  gtk_widget_ref (table_tree);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "table_tree", table_tree,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table_tree);
  gtk_container_add (GTK_CONTAINER (table_scroll), table_tree);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 0, 80);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 1, 80);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 2, 80);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 3, 80);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 4, 80);
  gtk_clist_set_column_width (GTK_CLIST (table_tree), 5, 80);
  gtk_clist_column_titles_show (GTK_CLIST (table_tree));

  label8 = gtk_label_new ("Table No.");
  gtk_widget_ref (label8);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label8", label8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label8);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 0, label8);

  label9 = gtk_label_new ("Game Type");
  gtk_widget_ref (label9);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label9", label9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label9);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 1, label9);

  label10 = gtk_label_new ("Seats");
  gtk_widget_ref (label10);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label10", label10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label10);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 2, label10);

  label11 = gtk_label_new ("Open Seats");
  gtk_widget_ref (label11);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label11", label11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label11);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 3, label11);

  label12 = gtk_label_new ("Humans");
  gtk_widget_ref (label12);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label12", label12,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label12);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 4, label12);

  label13 = gtk_label_new (" ");
  gtk_widget_ref (label13);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "label13", label13,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label13);
  gtk_clist_set_column_widget (GTK_CLIST (table_tree), 5, label13);

  h_pane = gtk_hpaned_new ();
  gtk_widget_ref (h_pane);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "h_pane", h_pane,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (h_pane);
  gtk_container_add (GTK_CONTAINER (v_pane), h_pane);
  gtk_container_set_border_width (GTK_CONTAINER (h_pane), 5);
  gtk_paned_set_gutter_size (GTK_PANED (h_pane), 10);
  gtk_paned_set_position (GTK_PANED (h_pane), 200);

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
  gtk_clist_set_column_width (GTK_CLIST (player_list), 1, 80);
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
                      GTK_SIGNAL_FUNC (get_game_options),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (join_game1), "activate",
                      GTK_SIGNAL_FUNC (join_game),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (logout1), "activate",
                      GTK_SIGNAL_FUNC (logout),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (exit1), "activate",
                      GTK_SIGNAL_FUNC (exit_dlg),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (game_types1), "activate",
                      GTK_SIGNAL_FUNC (get_types),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (players1), "activate",
                      GTK_SIGNAL_FUNC (get_players),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (tables1), "activate",
                      GTK_SIGNAL_FUNC (get_tables),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (launch_button), "clicked",
                      GTK_SIGNAL_FUNC (get_game_options),
                      main_win);
  gtk_signal_connect (GTK_OBJECT (join_button), "clicked",
                      GTK_SIGNAL_FUNC (join_game),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (logout_button), "clicked",
                      GTK_SIGNAL_FUNC (logout),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (exit_button), "clicked",
                      GTK_SIGNAL_FUNC (exit_dlg),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (msg_entry), "activate",
                      GTK_SIGNAL_FUNC (input_chat_msg),
                      msg_entry);
  gtk_signal_connect (GTK_OBJECT (msg_button), "clicked",
                      GTK_SIGNAL_FUNC (input_chat_msg),
                      msg_entry);

  gtk_window_add_accel_group (GTK_WINDOW (main_win), accel_group);

  return main_win;
}

