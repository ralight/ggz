#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <ggz.h>

#include "combat.h"
#include "support.h"
#include "dlg_options.h"
#include <stdio.h>
#include "game.h"
#include "map.h"

GdkPixmap *mini_buf;
GdkPixmap *preview_buf;
extern GdkColor lake_color;
extern GdkColor open_color;
extern GdkColor *player_colors;

extern int unitdefault[12];
extern char unitname[12][36];

static const char spin_name[12][8] = {"spin_F", "spin_B", "spin_1", "spin_2", "spin_3", "spin_4", "spin_5", "spin_6", "spin_7", "spin_8", "spin_9", "spin_10"};

GtkWidget*
create_dlg_options (int number)
{
  GtkWidget *dlg_options;
  GtkWidget *dialog_vbox2;
  GtkWidget *notebook1;
  GtkWidget *hbox3;
  GtkWidget *mini_board;
  GtkWidget *vbox1;
  GtkWidget *table1;
  GtkWidget *label7;
  GtkObject *width_adj;
  GtkWidget *width;
  GtkObject *height_adj;
  GtkWidget *height;
  GtkWidget *label6;
  GtkWidget *hseparator1;
  GSList *terrain_types_group = NULL;
  GtkWidget *open;
  GtkWidget *lake;
  GtkWidget *black;
  GtkWidget *player_1;
  GtkWidget *player_2;
  GtkWidget *label3;
  GtkWidget *army_hbox;
  GtkWidget *label4;
  GtkWidget *label5;
  GtkWidget *hbox5;
  GtkWidget *scrolledwindow1;
  GtkWidget *maps_list;
  GtkWidget *label10;
  GtkWidget *vbox2;
  GtkWidget *table2;
  GtkWidget *preview_board;
  GtkWidget *preview_label;
  GtkWidget *preview_options;
  GtkWidget *preview_options_scroll;
  GtkWidget *label11;
  GtkWidget *hbuttonbox3;
  GtkWidget *load;
  GtkWidget *save;
  GtkWidget *delete;
  GtkWidget *label9;
  GtkWidget *dialog_action_area2;
  GtkWidget *hbuttonbox2;
  GtkWidget *ok_button;
  GtkWidget *cancel_button;
	GtkWidget *unit_label[12];
	GtkWidget *unit_spin[12];
	GtkObject *unit_spin_adj[12];
	GtkWidget *unit_label_box;
	GtkWidget *unit_spin_box;
	GtkWidget *unit_stats_box;
  GtkWidget *army_total;
  GtkWidget *army_player_1;
  GtkWidget *army_player_2;
  // Options
  GtkWidget *options_table;
  GtkWidget *eventbox2;
  GtkWidget *eventbox3;
  GtkWidget *eventbox4;
  GtkWidget *eventbox5;
  GtkWidget *eventbox6;
  GtkWidget *eventbox7;
  GtkWidget *eventbox8;
  GtkWidget *eventbox9;
  GtkWidget *eventbox10;
  GtkWidget *eventbox11;
  GtkWidget *eventbox12;
  GtkWidget *eventbox13;
  GtkWidget *eventbox14;
  GtkWidget *eventbox15;
  GtkWidget *eventbox16;
  GtkWidget *opt_bin1[16];
  GtkWidget *opt_box_open_map;
  GtkTooltips *tooltips;
	int i, def;
  char **namelist;

  tooltips = gtk_tooltips_new ();

  dlg_options = gtk_dialog_new ();
  gtk_widget_set_name (dlg_options, "dlg_options");
  gtk_object_set_data (GTK_OBJECT (dlg_options), "dlg_options", dlg_options);
  gtk_window_set_title (GTK_WINDOW (dlg_options), _("Game options"));
  gtk_window_set_modal (GTK_WINDOW (dlg_options), TRUE);
  gtk_window_set_policy (GTK_WINDOW (dlg_options), TRUE, TRUE, FALSE);

  dialog_vbox2 = GTK_DIALOG (dlg_options)->vbox;
  gtk_widget_set_name (dialog_vbox2, "dialog_vbox2");
  gtk_object_set_data (GTK_OBJECT (dlg_options), "dialog_vbox2", dialog_vbox2);
  gtk_widget_show (dialog_vbox2);

  notebook1 = gtk_notebook_new ();
  gtk_widget_set_name (notebook1, "notebook1");
  gtk_widget_ref (notebook1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "notebook1", notebook1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (notebook1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox2), notebook1, TRUE, TRUE, 0);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_name (hbox3, "hbox3");
  gtk_widget_ref (hbox3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "hbox3", hbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox3);
  gtk_container_add (GTK_CONTAINER (notebook1), hbox3);

  mini_board = gtk_drawing_area_new ();
  gtk_widget_set_name (mini_board, "mini_board");
  gtk_widget_ref (mini_board);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "mini_board", mini_board,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mini_board);
  gtk_box_pack_start (GTK_BOX (hbox3), mini_board, TRUE, TRUE, 0);
  gtk_widget_set_usize (mini_board, 240, 240);
  gtk_widget_set_events (mini_board, GDK_BUTTON_PRESS_MASK);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_set_name (vbox1, "vbox1");
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (hbox3), vbox1, FALSE, FALSE, 5);

  table1 = gtk_table_new (2, 2, FALSE);
  gtk_widget_set_name (table1, "table1");
  gtk_widget_ref (table1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "table1", table1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, FALSE, TRUE, 0);

  label7 = gtk_label_new (_("Height: "));
  gtk_widget_set_name (label7, "label7");
  gtk_widget_ref (label7);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "label7", label7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label7);
  gtk_table_attach (GTK_TABLE (table1), label7, 0, 1, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  width_adj = gtk_adjustment_new (10, 1, 25, 1, 10, 10);
  width = gtk_spin_button_new (GTK_ADJUSTMENT (width_adj), 1, 0);
  gtk_widget_set_name (width, "width");
  gtk_widget_ref (width);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "width", width,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (width);
  gtk_table_attach (GTK_TABLE (table1), width, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  height_adj = gtk_adjustment_new (10, 1, 25, 1, 10, 10);
  height = gtk_spin_button_new (GTK_ADJUSTMENT (height_adj), 1, 0);
  gtk_widget_set_name (height, "height");
  gtk_widget_ref (height);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "height", height,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (height);
  gtk_table_attach (GTK_TABLE (table1), height, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label6 = gtk_label_new (_("Width: "));
  gtk_widget_set_name (label6, "label6");
  gtk_widget_ref (label6);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "label6", label6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table1), label6, 0, 1, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  hseparator1 = gtk_hseparator_new ();
  gtk_widget_set_name (hseparator1, "hseparator1");
  gtk_widget_ref (hseparator1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "hseparator1", hseparator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hseparator1);
  gtk_box_pack_start (GTK_BOX (vbox1), hseparator1, FALSE, TRUE, 7);

  open = gtk_radio_button_new_with_label (terrain_types_group, _("Open"));
  terrain_types_group = gtk_radio_button_group (GTK_RADIO_BUTTON (open));
  gtk_widget_set_name (open, "open");
	gtk_object_set_data (GTK_OBJECT(open), "type", GINT_TO_POINTER(OPEN));
  gtk_widget_ref (open);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "open", open,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (open);
  gtk_box_pack_start (GTK_BOX (vbox1), open, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (open), TRUE);

  lake = gtk_radio_button_new_with_label (terrain_types_group, _("Lake"));
  terrain_types_group = gtk_radio_button_group (GTK_RADIO_BUTTON (lake));
  gtk_widget_set_name (lake, "lake");
	gtk_object_set_data (GTK_OBJECT(lake), "type", GINT_TO_POINTER(LAKE));
  gtk_widget_ref (lake);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "lake", lake,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lake);
  gtk_box_pack_start (GTK_BOX (vbox1), lake, FALSE, FALSE, 0);

  black = gtk_radio_button_new_with_label (terrain_types_group, _("Black"));
  terrain_types_group = gtk_radio_button_group (GTK_RADIO_BUTTON (black));
  gtk_widget_set_name (black, "black");
	gtk_object_set_data (GTK_OBJECT(black), "type", GINT_TO_POINTER(BLACK));
  gtk_widget_ref (black);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "black", black,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (black);
  gtk_box_pack_start (GTK_BOX (vbox1), black, FALSE, FALSE, 0);

  player_1 = gtk_radio_button_new_with_label (terrain_types_group, _("Player 1"));
  terrain_types_group = gtk_radio_button_group (GTK_RADIO_BUTTON (player_1));
  gtk_widget_set_name (player_1, "player_1");
	gtk_object_set_data (GTK_OBJECT(player_1), "type", GINT_TO_POINTER(PLAYER_1));
  gtk_widget_ref (player_1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "player_1", player_1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_1);
  gtk_box_pack_start (GTK_BOX (vbox1), player_1, FALSE, FALSE, 0);

  player_2 = gtk_radio_button_new_with_label (terrain_types_group, _("Player 2"));
  terrain_types_group = gtk_radio_button_group (GTK_RADIO_BUTTON (player_2));
  gtk_widget_set_name (player_2, "player_2");
	gtk_object_set_data (GTK_OBJECT(player_2), "type", GINT_TO_POINTER(PLAYER_2));
  gtk_widget_ref (player_2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "player_2", player_2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_2);
  gtk_box_pack_start (GTK_BOX (vbox1), player_2, FALSE, FALSE, 0);

  label3 = gtk_label_new (_("Terrain"));
  gtk_widget_set_name (label3, "label3");
  gtk_widget_ref (label3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "label3", label3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label3);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label3);

  army_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_name (army_hbox, "army_hbox");
  gtk_widget_ref (army_hbox);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "army_hbox", army_hbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (army_hbox);
  gtk_container_add (GTK_CONTAINER (notebook1), army_hbox);

  label4 = gtk_label_new (_("Army"));
  gtk_widget_set_name (label4, "label4");
  gtk_widget_ref (label4);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "label4", label4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label4);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label4);

  /* Options ! */

  options_table = gtk_table_new (8, 2, FALSE);
  gtk_widget_set_name (options_table, "options_table");
  gtk_widget_ref (options_table);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "options_table", options_table,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (options_table);
  gtk_container_add (GTK_CONTAINER (notebook1), options_table);

  eventbox2 = gtk_event_box_new ();
  gtk_widget_set_name (eventbox2, "eventbox2");
  gtk_widget_ref (eventbox2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "eventbox2", eventbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (eventbox2);
  gtk_table_attach (GTK_TABLE (options_table), eventbox2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, eventbox2, _("If set, the bombs who explode will be removed from the game."), NULL);

  opt_bin1[1] = gtk_check_button_new_with_label (_("One Time Bomb"));
  gtk_widget_set_name (opt_bin1[1], "opt_bin1[1]");
  gtk_widget_ref (opt_bin1[1]);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "opt_bin1[1]", opt_bin1[1],
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_bin1[1]);
  gtk_container_add (GTK_CONTAINER (eventbox2), opt_bin1[1]);
  gtk_widget_set_sensitive (opt_bin1[1], TRUE);

  eventbox3 = gtk_event_box_new ();
  gtk_widget_set_name (eventbox3, "eventbox3");
  gtk_widget_ref (eventbox3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "eventbox3", eventbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (eventbox3);
  gtk_table_attach (GTK_TABLE (options_table), eventbox3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, eventbox3, _("If set, the spy will be able to kill any unit he attacks (except bombs), but he will die with the unit (think of it like a terrorist, with a body full of bombs)"), NULL);

  opt_bin1[2] = gtk_check_button_new_with_label (_("Terrorist Spy"));
  gtk_widget_set_name (opt_bin1[2], "opt_bin1[2]");
  gtk_widget_ref (opt_bin1[2]);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "opt_bin1[2]", opt_bin1[2],
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_bin1[2]);
  gtk_container_add (GTK_CONTAINER (eventbox3), opt_bin1[2]);
  gtk_widget_set_sensitive (opt_bin1[2], TRUE);

  eventbox4 = gtk_event_box_new ();
  gtk_widget_set_name (eventbox4, "eventbox4");
  gtk_widget_ref (eventbox4);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "eventbox4", eventbox4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (eventbox4);
  gtk_table_attach (GTK_TABLE (options_table), eventbox4, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, eventbox4, _("If set, the bombs can walk, just like regular officers. But they aren't able to attack! (think of it like a officer carrying a bomb)"), NULL);

  opt_bin1[3] = gtk_check_button_new_with_label (_("Moving Bombs"));
  gtk_widget_set_name (opt_bin1[3], "opt_bin1[3]");
  gtk_widget_ref (opt_bin1[3]);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "opt_bin1[3]", opt_bin1[3],
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_bin1[3]);
  gtk_container_add (GTK_CONTAINER (eventbox4), opt_bin1[3]);
  gtk_widget_set_sensitive (opt_bin1[3], TRUE);

  eventbox5 = gtk_event_box_new ();
  gtk_widget_set_name (eventbox5, "eventbox5");
  gtk_widget_ref (eventbox5);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "eventbox5", eventbox5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (eventbox5);
  gtk_table_attach (GTK_TABLE (options_table), eventbox5, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, eventbox5, _("If set, scouts are able to walk more then one square at a time and attack at the same turn."), NULL);

  opt_bin1[4] = gtk_check_button_new_with_label (_("Super Scout"));
  gtk_widget_set_name (opt_bin1[4], "opt_bin1[4]");
  gtk_widget_ref (opt_bin1[4]);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "opt_bin1[4]", opt_bin1[4],
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_bin1[4]);
  gtk_container_add (GTK_CONTAINER (eventbox5), opt_bin1[4]);
  gtk_widget_set_sensitive (opt_bin1[4], TRUE);

  eventbox6 = gtk_event_box_new ();
  gtk_widget_set_name (eventbox6, "eventbox6");
  gtk_widget_ref (eventbox6);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "eventbox6", eventbox6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (eventbox6);
  gtk_table_attach (GTK_TABLE (options_table), eventbox6, 0, 1, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, eventbox6, _("If set, the flags will be able to walk, just like a regular unit (think of it like a officer carrying a flag)"), NULL);

  opt_bin1[5] = gtk_check_button_new_with_label (_("Moving Flags"));
  gtk_widget_set_name (opt_bin1[5], "opt_bin1[5]");
  gtk_widget_ref (opt_bin1[5]);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "opt_bin1[5]", opt_bin1[5],
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_bin1[5]);
  gtk_container_add (GTK_CONTAINER (eventbox6), opt_bin1[5]);
  gtk_widget_set_sensitive (opt_bin1[5], TRUE);

  eventbox7 = gtk_event_box_new ();
  gtk_widget_set_name (eventbox7, "eventbox7");
  gtk_widget_ref (eventbox7);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "eventbox7", eventbox7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (eventbox7);
  gtk_table_attach (GTK_TABLE (options_table), eventbox7, 0, 1, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, eventbox7, _("If set, the outcome of attacks will be solved randomly. Spies and bombs are treated the normal way. The odds depend on the difference between the power of the two units. The first number is the odd of the highest-ranked unit winning, and the second number is the odd of both units dying. 0: 30/40, 1: 35/39, 2: 41/37, 3: 49/33, 4: 56/29, 5: 64/24, 6: 71/20, 7: 77/15, 8: 84/11"), NULL);

  opt_bin1[6] = gtk_check_button_new_with_label (_("Random outcome of attacks"));
  gtk_widget_set_name (opt_bin1[6], "opt_bin1[6]");
  gtk_widget_ref (opt_bin1[6]);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "opt_bin1[6]", opt_bin1[6],
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_bin1[6]);
  gtk_container_add (GTK_CONTAINER (eventbox7), opt_bin1[6]);
  gtk_widget_set_sensitive (opt_bin1[6], TRUE);

  eventbox8 = gtk_event_box_new ();
  gtk_widget_set_name (eventbox8, "eventbox8");
  gtk_widget_ref (eventbox8);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "eventbox8", eventbox8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (eventbox8);
  gtk_table_attach (GTK_TABLE (options_table), eventbox8, 0, 1, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, eventbox8, _("If set, units will be able to move (and attack) diagonally. (Scouts will be able of walking how many squares as they want diagonally)"), NULL);

  opt_bin1[7] = gtk_check_button_new_with_label (_("Allow diagonal moves"));
  gtk_widget_set_name (opt_bin1[7], "opt_bin1[7]");
  gtk_widget_ref (opt_bin1[7]);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "opt_bin1[7]", opt_bin1[7],
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_bin1[7]);
  gtk_container_add (GTK_CONTAINER (eventbox8), opt_bin1[7]);
  gtk_widget_set_sensitive (opt_bin1[7], TRUE);

  eventbox9 = gtk_event_box_new ();
  gtk_widget_set_name (eventbox9, "eventbox9");
  gtk_widget_ref (eventbox9);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "eventbox9", eventbox9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (eventbox9);
  gtk_table_attach (GTK_TABLE (options_table), eventbox9, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, eventbox9, _("If set, the type of the winning unit in a attack will not be shown."), NULL);

  opt_bin1[8] = gtk_check_button_new_with_label (_("Unknow Victor"));
  gtk_widget_set_name (opt_bin1[8], "opt_bin1[8]");
  gtk_widget_ref (opt_bin1[8]);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "opt_bin1[8]", opt_bin1[8],
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_bin1[8]);
  gtk_container_add (GTK_CONTAINER (eventbox9), opt_bin1[8]);
  gtk_widget_set_sensitive (opt_bin1[8], TRUE);

  eventbox10 = gtk_event_box_new ();
  gtk_widget_set_name (eventbox10, "eventbox10");
  gtk_widget_ref (eventbox10);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "eventbox10", eventbox10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (eventbox10);
  gtk_table_attach (GTK_TABLE (options_table), eventbox10, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, eventbox10, _("If set, the defense won't have to reveal what is his type, only wheter he wins or loses to the attacking unit."), NULL);

  opt_bin1[9] = gtk_check_button_new_with_label (_("Silent Defense"));
  gtk_widget_set_name (opt_bin1[9], "opt_bin1[9]");
  gtk_widget_ref (opt_bin1[9]);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "opt_bin1[9]", opt_bin1[9],
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_bin1[9]);
  gtk_container_add (GTK_CONTAINER (eventbox10), opt_bin1[9]);
  gtk_widget_set_sensitive (opt_bin1[9], TRUE);

  eventbox11 = gtk_event_box_new ();
  gtk_widget_set_name (eventbox11, "eventbox11");
  gtk_widget_ref (eventbox11);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "eventbox11", eventbox11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (eventbox11);
  gtk_table_attach (GTK_TABLE (options_table), eventbox11, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, eventbox11, _("If set, the attacking won't have to reveal it's type, only wheter it wins or loses to the defending unit."), NULL);

  opt_bin1[10] = gtk_check_button_new_with_label (_("Silent Offense"));
  gtk_widget_set_name (opt_bin1[10], "opt_bin1[10]");
  gtk_widget_ref (opt_bin1[10]);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "opt_bin1[10]", opt_bin1[10],
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_bin1[10]);
  gtk_container_add (GTK_CONTAINER (eventbox11), opt_bin1[10]);
  gtk_widget_set_sensitive (opt_bin1[10], TRUE);

  eventbox12 = gtk_event_box_new ();
  gtk_widget_set_name (eventbox12, "eventbox12");
  gtk_widget_ref (eventbox12);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "eventbox12", eventbox12,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (eventbox12);
  gtk_table_attach (GTK_TABLE (options_table), eventbox12, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, eventbox12, _("If set, the players won't be able to setup their units. It will be randomly set up by the server."), NULL);

  opt_bin1[11] = gtk_check_button_new_with_label (_("Random Setup"));
  gtk_widget_set_name (opt_bin1[11], "opt_bin1[11]");
  gtk_widget_ref (opt_bin1[11]);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "opt_bin1[11]", opt_bin1[11],
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_bin1[11]);
  gtk_container_add (GTK_CONTAINER (eventbox12), opt_bin1[11]);
  gtk_widget_set_sensitive (opt_bin1[11], TRUE);

  eventbox13 = gtk_event_box_new ();
  gtk_widget_set_name (eventbox13, "eventbox13");
  gtk_widget_ref (eventbox13);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "eventbox13", eventbox13,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (eventbox13);
  gtk_table_attach (GTK_TABLE (options_table), eventbox13, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, eventbox13, _("If set, the sergeant of all the teams will have been through a higly rigorous training, and will be able to walk through lakes. It won't be able to stay in a lake, instead, it will be able to appear at the opposite margin of a lake (it must go through a straight line, and it must be only lake tiles between the source and the destination)"), NULL);

  opt_bin1[12] = gtk_check_button_new_with_label (_("Special Forces Sergeant"));
  gtk_widget_set_name (opt_bin1[12], "opt_bin1[12]");
  gtk_widget_ref (opt_bin1[12]);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "opt_bin1[12]", opt_bin1[12],
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_bin1[12]);
  gtk_container_add (GTK_CONTAINER (eventbox13), opt_bin1[12]);
  gtk_widget_set_sensitive (opt_bin1[12], TRUE);

  eventbox14 = gtk_event_box_new ();
  gtk_widget_set_name (eventbox14, "eventbox14");
  gtk_widget_ref (eventbox14);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "eventbox14", eventbox14,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (eventbox14);
  gtk_table_attach (GTK_TABLE (options_table), eventbox14, 1, 2, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, eventbox14, _("If set, the units will be able to do a rush attack, when they walk how many tiles as they want (in a straight line, just like a scout) and then attack the enemy, in the same turn. However, your units die after this attack, even if they succeed doing it."), NULL);

  opt_bin1[13] = gtk_check_button_new_with_label (_("Rush Attack"));
  gtk_widget_set_name (opt_bin1[13], "opt_bin1[13]");
  gtk_widget_ref (opt_bin1[13]);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "opt_bin1[13]", opt_bin1[13],
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_bin1[13]);
  gtk_container_add (GTK_CONTAINER (eventbox14), opt_bin1[13]);
  gtk_widget_set_sensitive (opt_bin1[13], TRUE);

  eventbox15 = gtk_event_box_new ();
  gtk_widget_set_name (eventbox15, "eventbox15");
  gtk_widget_ref (eventbox15);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "eventbox15", eventbox15,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (eventbox15);
  gtk_table_attach (GTK_TABLE (options_table), eventbox15, 1, 2, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, eventbox15, _("If set, the players won't be able to see the remaining units for the opponent, ie, they won't know how many units of each type their opponent has left. Please note that this option can't be enforceable by the server, so just use it against trusted opponents."), NULL);

  opt_bin1[14] = gtk_check_button_new_with_label (_("Hide enemy's unit list"));
  gtk_widget_set_name (opt_bin1[14], "opt_bin1[14]");
  gtk_widget_ref (opt_bin1[14]);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "opt_bin1[14]", opt_bin1[14],
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_bin1[14]);
  gtk_container_add (GTK_CONTAINER (eventbox15), opt_bin1[14]);
  gtk_widget_set_sensitive (opt_bin1[14], TRUE);

  eventbox16 = gtk_event_box_new ();
  gtk_widget_set_name (eventbox16, "eventbox16");
  gtk_widget_ref (eventbox16);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "eventbox16", eventbox16,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (eventbox16);
  gtk_table_attach (GTK_TABLE (options_table), eventbox16, 1, 2, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, eventbox16, _("If set, the server will ask the clients to show the enemy units for just one turn (right after a attack, for instance). However, it can't be enforceable, and a bad client could cheat, showing all the enemy units it knows about (just the ones that the server tells them, like after an attack)"), NULL);

  opt_bin1[15] = gtk_check_button_new_with_label (_("Forget enemy units"));
  gtk_widget_set_name (opt_bin1[15], "opt_bin1[15]");
  gtk_widget_ref (opt_bin1[15]);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "opt_bin1[15]", opt_bin1[15],
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_bin1[15]);
  gtk_container_add (GTK_CONTAINER (eventbox16), opt_bin1[15]);
  gtk_widget_set_sensitive (opt_bin1[15], TRUE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (opt_bin1[15]), TRUE);

  opt_box_open_map = gtk_event_box_new ();
  gtk_widget_set_name (opt_box_open_map, "opt_box_open_map");
  gtk_widget_ref (opt_box_open_map);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "opt_box_open_map", opt_box_open_map,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_box_open_map);
  gtk_table_attach (GTK_TABLE (options_table), opt_box_open_map, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, opt_box_open_map, _("If set, all the units will be shown to both players"), NULL);

  opt_bin1[0] = gtk_check_button_new_with_label (_("Open Map"));
  gtk_widget_set_name (opt_bin1[0], "opt_bin1[0]");
  gtk_widget_ref (opt_bin1[0]);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "opt_bin1[0]", opt_bin1[0],
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (opt_bin1[0]);
  gtk_container_add (GTK_CONTAINER (opt_box_open_map), opt_bin1[0]);
  gtk_widget_set_sensitive (opt_bin1[0], TRUE);

  /* end of options! */

  label5 = gtk_label_new (_("Options"));
  gtk_widget_set_name (label5, "label5");
  gtk_widget_ref (label5);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "label5", label5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label5);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label5);

  hbox5 = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_name (hbox5, "hbox5");
  gtk_widget_ref (hbox5);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "hbox5", hbox5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox5);
  gtk_container_add (GTK_CONTAINER (notebook1), hbox5);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_name (scrolledwindow1, "scrolledwindow1");
  gtk_widget_ref (scrolledwindow1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "scrolledwindow1", scrolledwindow1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (hbox5), scrolledwindow1, FALSE, FALSE, 0);
  gtk_widget_set_usize (scrolledwindow1, 163, -2);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  maps_list = gtk_clist_new (1);
  gtk_widget_set_name (maps_list, "maps_list");
  gtk_widget_ref (maps_list);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "maps_list", maps_list,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (maps_list);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), maps_list);
  gtk_container_set_border_width (GTK_CONTAINER (maps_list), 2);
  gtk_clist_set_column_width (GTK_CLIST (maps_list), 0, 80);
  gtk_clist_column_titles_show (GTK_CLIST (maps_list));

  label10 = gtk_label_new (_("Stored Maps"));
  gtk_widget_set_name (label10, "label10");
  gtk_widget_ref (label10);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "label10", label10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label10);
  gtk_clist_set_column_widget (GTK_CLIST (maps_list), 0, label10);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_set_name (vbox2, "vbox2");
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "vbox2", vbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (hbox5), vbox2, TRUE, TRUE, 0);

  table2 = gtk_table_new (3, 3, FALSE);
  gtk_widget_set_name (table2, "table2");
  gtk_widget_ref (table2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "table2", table2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table2);
  gtk_box_pack_start (GTK_BOX (vbox2), table2, TRUE, TRUE, 0);

  preview_board = gtk_drawing_area_new ();
  gtk_widget_set_name (preview_board, "preview_board");
  gtk_widget_ref (preview_board);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "preview_board", preview_board,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (preview_board);
  gtk_table_attach (GTK_TABLE (table2), preview_board, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  //gtk_widget_set_usize (preview_board, 100, 100);

  preview_label = gtk_label_new ("Preview\nData");
  gtk_label_set_line_wrap(GTK_LABEL(preview_label), TRUE);
  gtk_widget_set_name (preview_label, "preview_label");
  gtk_widget_ref (preview_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "preview_label", preview_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (preview_label);
  gtk_table_attach (GTK_TABLE (table2), preview_label, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  preview_options_scroll = gtk_scrolled_window_new(NULL, NULL);
  gtk_object_set_data(GTK_OBJECT(dlg_options), "preview_options_scroll", preview_options_scroll);
  gtk_widget_show(preview_options_scroll);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(preview_options_scroll),
                                 GTK_POLICY_NEVER,
                                 GTK_POLICY_AUTOMATIC);
  gtk_table_attach(GTK_TABLE(table2), preview_options_scroll, 0, 3, 2, 3, (GtkAttachOptions) (GTK_SHRINK|GTK_FILL), (GtkAttachOptions) (GTK_SHRINK|GTK_FILL), 0, 0);
  gtk_widget_set_usize (preview_options_scroll, -1, 30);

#ifdef GTK2
  preview_options = gtk_text_view_new_with_buffer(gtk_text_buffer_new(NULL));
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(preview_options), GTK_WRAP_WORD);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(preview_options), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(preview_options), FALSE);
#else
  preview_options = gtk_text_new(gtk_scrolled_window_get_hadjustment(
                                  GTK_SCROLLED_WINDOW(preview_options_scroll)),
                                 gtk_scrolled_window_get_vadjustment(
                                  GTK_SCROLLED_WINDOW(preview_options_scroll)));
  gtk_text_set_word_wrap(GTK_TEXT(preview_options), TRUE);
  gtk_text_set_line_wrap(GTK_TEXT(preview_options), TRUE);
  gtk_text_set_editable(GTK_TEXT(preview_options), FALSE);
#endif
  gtk_widget_set_name(preview_options, "preview_options");
  gtk_object_set_data(GTK_OBJECT(dlg_options), "preview_options", preview_options);
  gtk_widget_show(preview_options);
  gtk_container_add(GTK_CONTAINER(preview_options_scroll), preview_options);
  //gtk_table_attach(GTK_TABLE(table2), preview_options, 0, 2, 2, 3, (GtkAttachOptions) (GTK_SHRINK|GTK_FILL), (GtkAttachOptions)(GTK_SHRINK|GTK_FILL), 0, 0);


  label11 = gtk_label_new (_("Map Preview"));
  gtk_widget_set_name (label11, "label11");
  gtk_widget_ref (label11);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "label11", label11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label11);
  gtk_table_attach (GTK_TABLE (table2), label11, 1, 2, 0, 1,
                    0, 0, 0, 3);

  hbuttonbox3 = gtk_hbutton_box_new ();
  gtk_widget_set_name (hbuttonbox3, "hbuttonbox3");
  gtk_widget_ref (hbuttonbox3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "hbuttonbox3", hbuttonbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox3);
  gtk_box_pack_start (GTK_BOX (vbox2), hbuttonbox3, FALSE, FALSE, 0);
  gtk_button_box_set_child_size (GTK_BUTTON_BOX (hbuttonbox3), 81, 27);
  gtk_button_box_set_child_ipadding (GTK_BUTTON_BOX (hbuttonbox3), 0, 0);

  load = gtk_button_new_with_label (_("Load"));
  gtk_widget_set_name (load, "load");
  gtk_widget_ref (load);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "load", load,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (load);
  gtk_container_add (GTK_CONTAINER (hbuttonbox3), load);
  GTK_WIDGET_SET_FLAGS (load, GTK_CAN_DEFAULT);

  save = gtk_button_new_with_label (_("Save"));
  gtk_widget_set_name (save, "save");
  gtk_widget_ref (save);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "save", save,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (save);
  gtk_container_add (GTK_CONTAINER (hbuttonbox3), save);
  GTK_WIDGET_SET_FLAGS (save, GTK_CAN_DEFAULT);

  delete = gtk_button_new_with_label (_("Delete"));
  gtk_widget_set_name (delete, "delete");
  gtk_widget_ref (delete);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "delete", delete,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (delete);
  gtk_container_add (GTK_CONTAINER (hbuttonbox3), delete);
  GTK_WIDGET_SET_FLAGS (delete, GTK_CAN_DEFAULT);

  label9 = gtk_label_new (_("Maps"));
  gtk_widget_set_name (label9, "label9");
  gtk_widget_ref (label9);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "label9", label9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label9);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 3), label9);


  dialog_action_area2 = GTK_DIALOG (dlg_options)->action_area;
  gtk_widget_set_name (dialog_action_area2, "dialog_action_area2");
  gtk_object_set_data (GTK_OBJECT (dlg_options), "dialog_action_area2", dialog_action_area2);
  gtk_widget_show (dialog_action_area2);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area2), 10);

  hbuttonbox2 = gtk_hbutton_box_new ();
  gtk_widget_set_name (hbuttonbox2, "hbuttonbox2");
  gtk_widget_ref (hbuttonbox2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "hbuttonbox2", hbuttonbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox2);
  gtk_box_pack_start (GTK_BOX (dialog_action_area2), hbuttonbox2, TRUE, TRUE, 0);

  ok_button = gtk_button_new_with_label (_("Start Game"));
  gtk_widget_set_name (ok_button, "ok_button");
  gtk_widget_ref (ok_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "ok_button", ok_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ok_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), ok_button);
  GTK_WIDGET_SET_FLAGS (ok_button, GTK_CAN_DEFAULT);

  cancel_button = gtk_button_new_with_label (_("Cancel"));
  gtk_widget_set_name (cancel_button, "cancel_button");
  gtk_widget_ref (cancel_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "cancel_button", cancel_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cancel_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), cancel_button);
  gtk_widget_set_sensitive (cancel_button, TRUE);
  GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);

	// Create the army setup stuff
	unit_label_box = gtk_vbox_new (TRUE, 0);
	gtk_widget_show (unit_label_box);
	unit_spin_box = gtk_vbox_new (TRUE, 0);
	gtk_widget_show (unit_spin_box);
	unit_stats_box = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (army_hbox), unit_label_box, FALSE, FALSE, 10);
	gtk_box_pack_start (GTK_BOX (army_hbox), unit_spin_box, TRUE, TRUE, 10);
	gtk_box_pack_start (GTK_BOX (army_hbox), unit_stats_box, FALSE, TRUE, 20);
	for (i = 0; i < 12; i++) {
		unit_label[i] = gtk_label_new (unitname[i]);
  	gtk_widget_show (unit_label[i]);
  	gtk_box_pack_start (GTK_BOX (unit_label_box), unit_label[i], FALSE, FALSE, 0);
  	unit_spin_adj[i] = gtk_adjustment_new (unitdefault[i], 0, 100, 1, 10, 10);
    if (i == U_FLAG)
      GTK_ADJUSTMENT(unit_spin_adj[i])->lower = 1;
  	unit_spin[i] = gtk_spin_button_new (GTK_ADJUSTMENT (unit_spin_adj[i]), 1, 0);
		gtk_widget_set_name (unit_spin[i], spin_name[i]);
		gtk_widget_ref(unit_spin[i]);
		gtk_object_set_data_full (GTK_OBJECT (dlg_options), spin_name[i], unit_spin[i],
															(GtkDestroyNotify) gtk_widget_unref);
		gtk_box_pack_start (GTK_BOX (unit_spin_box), unit_spin[i], TRUE, FALSE, 0);
		gtk_widget_show (unit_spin[i]);
		gtk_signal_connect_object(GTK_OBJECT (unit_spin[i]), "changed",
											 GTK_SIGNAL_FUNC (dlg_options_update), GTK_OBJECT (dlg_options));
	}
  // Add the stats
  army_total = gtk_label_new("Total:");
  army_player_1 = gtk_label_new("Player 1:");
  army_player_2 = gtk_label_new("Player 2:");
  gtk_widget_set_name(army_total, "army_total");
  gtk_widget_set_name(army_total, "army_player_1");
  gtk_widget_set_name(army_total, "army_player_2");
  gtk_widget_ref(army_total);
  gtk_widget_ref(army_player_1);
  gtk_widget_ref(army_player_2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "army_total", army_total,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "army_player_1", army_player_1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "army_player_2", army_player_2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_box_pack_start(GTK_BOX(unit_stats_box), army_total, FALSE, FALSE, 2);
  gtk_box_pack_start(GTK_BOX(unit_stats_box), army_player_1, FALSE, FALSE, 2);
  gtk_box_pack_start(GTK_BOX(unit_stats_box), army_player_2, FALSE, FALSE, 2);

  gtk_signal_connect(GTK_OBJECT(dlg_options), "delete_event",
                     GTK_SIGNAL_FUNC (game_refuse_options),
                     NULL);
  gtk_signal_connect(GTK_OBJECT(dlg_options), "destroy_event",
                     GTK_SIGNAL_FUNC (game_refuse_options),
                     NULL);
	gtk_signal_connect(GTK_OBJECT (cancel_button), "clicked",
									   GTK_SIGNAL_FUNC (cancel_button_clicked), 
                     dlg_options);
	gtk_signal_connect_object_after(GTK_OBJECT (cancel_button), "clicked",
									   GTK_SIGNAL_FUNC (gtk_widget_destroy), 
                     GTK_OBJECT(dlg_options));
	gtk_signal_connect(GTK_OBJECT (mini_board), "expose_event",
										 GTK_SIGNAL_FUNC (mini_board_expose), dlg_options);
	gtk_signal_connect(GTK_OBJECT (mini_board), "configure_event",
										 GTK_SIGNAL_FUNC (mini_board_configure), dlg_options);
	gtk_signal_connect(GTK_OBJECT (mini_board), "button_press_event",
									   GTK_SIGNAL_FUNC (mini_board_click), dlg_options);
	gtk_signal_connect(GTK_OBJECT (load), "clicked",
									   GTK_SIGNAL_FUNC (load_button_clicked), dlg_options);
	gtk_signal_connect(GTK_OBJECT (save), "clicked",
									   GTK_SIGNAL_FUNC (save_button_clicked), dlg_options);
	gtk_signal_connect(GTK_OBJECT (delete), "clicked",
									   GTK_SIGNAL_FUNC (delete_button_clicked), dlg_options);
	gtk_signal_connect(GTK_OBJECT (preview_board), "expose_event",
										 GTK_SIGNAL_FUNC (preview_expose), dlg_options);
	gtk_signal_connect(GTK_OBJECT (preview_board), "configure_event",
										 GTK_SIGNAL_FUNC (init_preview), dlg_options);
	gtk_signal_connect(GTK_OBJECT (maps_list), "select-row",
									   GTK_SIGNAL_FUNC (maps_list_selected), dlg_options);
	gtk_signal_connect_object(GTK_OBJECT (width), "changed",
										 GTK_SIGNAL_FUNC (dlg_options_update), GTK_OBJECT (dlg_options));
	gtk_signal_connect_object_after(GTK_OBJECT (width), "changed",
										 GTK_SIGNAL_FUNC (init_map_data), GTK_OBJECT (dlg_options));
	gtk_signal_connect_object(GTK_OBJECT (height), "changed",
										 GTK_SIGNAL_FUNC (dlg_options_update), GTK_OBJECT (dlg_options));
	gtk_signal_connect_object_after(GTK_OBJECT (height), "changed",
										 GTK_SIGNAL_FUNC (init_map_data), GTK_OBJECT (dlg_options));
  for (i = 0; i < 16; i++) {
    gtk_signal_connect_object (GTK_OBJECT (opt_bin1[i]), "toggled",
                               GTK_SIGNAL_FUNC (dlg_options_update),
                               GTK_OBJECT (dlg_options));
  }

  mini_buf = NULL;

  gtk_object_set_data(GTK_OBJECT(dlg_options), "number", GINT_TO_POINTER(number));

	dlg_options_update(dlg_options);

  init_map_data(dlg_options);

  def = dlg_options_list_maps(maps_list);
  namelist = gtk_object_get_data(GTK_OBJECT(maps_list), "maps");
  if (def >= 0 && namelist && namelist[def]) {
    load_map(namelist[def], dlg_options);
  }

  gtk_widget_grab_default (ok_button);
  return dlg_options;
}

void init_map_data(GtkWidget *dlg_options) {
  combat_game *options;
  int a;

	// Now init the data
  options = gtk_object_get_data(GTK_OBJECT(dlg_options), "options");
  if (!options)
    dlg_options_update(dlg_options);
  if (options->map) {
    ggz_free(options->map);
    options->map = NULL;
  }
	options->map = (tile *)ggz_malloc(sizeof(tile) * options->width * options->height + 1);
	for (a = 0; a < options->width * options->height; a++)
	  options->map[a].type = OPEN;

	gtk_object_set_data(GTK_OBJECT(dlg_options), "options", options);
  if (mini_buf)
    draw_mini_board(dlg_options);

}
 


void maps_list_selected (GtkCList *clist, gint row, gint column,
	 											 GdkEventButton *event, gpointer user_data) {
  combat_game *preview_game;
  GtkWidget *preview_label = lookup_widget(user_data, "preview_label");
  GtkWidget *preview_options = lookup_widget(user_data, "preview_options");
  char **filenames;
  char preview_string[256];
  char *preview_options_string;
  int changed = -1;
  int tot = 0, other = 0, a, pos = 0;
  gtk_object_set_data(GTK_OBJECT(clist), "row", GINT_TO_POINTER(row));
  filenames = gtk_object_get_data(GTK_OBJECT(clist), "maps");
  preview_game = (combat_game *)ggz_malloc(sizeof(combat_game));
  preview_game->number = GPOINTER_TO_INT(
                        gtk_object_get_data(GTK_OBJECT(user_data), "number"));
  preview_game->army = (char **)calloc(preview_game->number+1, sizeof(char *));
  preview_game->army[preview_game->number] = (char *)calloc(12, sizeof(char));
  preview_game->map = NULL;
  preview_game->name = NULL;
  preview_game->options = 0;
  map_load(preview_game, filenames[row], &changed);
  if (changed == 0)
    dlg_options_list_maps(GTK_WIDGET(clist));
  gtk_object_set_data(GTK_OBJECT(user_data), "preview", preview_game);
  /* TODO: Show preview */
  draw_preview(user_data);
  for (a = U_FLAG; a < U_SERGEANT; a++)
    tot+=ARMY(preview_game, a);
  for (a = U_SERGEANT; a < 12; a++) {
    tot+=preview_game->army[preview_game->number][a];
    other+=ARMY(preview_game, a);
  }
  sprintf(preview_string, "%d x %d\n\n%s: %d\n%s: %d\n%s: %d\n%s: %d\n%s: %d\nOthers: %d\n\nTotal: %d",
          preview_game->width, preview_game->height,
          unitname[U_FLAG],
          ARMY(preview_game, U_FLAG),
          unitname[U_BOMB],
          ARMY(preview_game, U_BOMB),
          unitname[U_SPY],
          ARMY(preview_game, U_SPY),
          unitname[U_SCOUT],
          ARMY(preview_game, U_SCOUT),
          unitname[U_MINER],
          ARMY(preview_game, U_MINER),
          other,
          tot);
  gtk_label_set_text(GTK_LABEL(preview_label), preview_string);
  gtk_editable_delete_text(GTK_EDITABLE(preview_options), 0, -1);
  if (preview_game->options)
    preview_options_string = combat_options_describe(preview_game, 1);
  else {
    preview_options_string = (char *)ggz_malloc(sizeof(char) * 11);
    sprintf(preview_options_string, "No options");
  }
  gtk_editable_insert_text(GTK_EDITABLE(preview_options), preview_options_string, strlen(preview_options_string), &pos);
  gtk_editable_set_position(GTK_EDITABLE(preview_options), 0);

}

GtkWidget *create_yes_no_dlg(char *text, GtkSignalFunc function, gpointer user_data) {
  GtkWidget *dlg = gtk_dialog_new();
  GtkWidget *yes, *no, *label;
  gtk_window_set_modal (GTK_WINDOW (dlg), TRUE);
  label = gtk_label_new(text);
  yes = gtk_button_new_with_label("Yes");
  no = gtk_button_new_with_label("No");
  gtk_object_set_data(GTK_OBJECT(dlg), "yes", yes);
  gtk_object_set_data(GTK_OBJECT(yes), "dlg", dlg);
  gtk_signal_connect_object(GTK_OBJECT(no), "clicked",
                      GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT(dlg));
  gtk_signal_connect_object_after(GTK_OBJECT(yes), "clicked",
                      GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT(dlg));
  gtk_signal_connect(GTK_OBJECT(yes), "clicked",
                     GTK_SIGNAL_FUNC (function), user_data); 
	gtk_container_set_border_width(GTK_CONTAINER(GTK_DIALOG(dlg)->vbox), 5);
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dlg)->action_area), yes);
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dlg)->action_area), no);
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dlg)->vbox), label);
  return dlg;
}

void delete_button_clicked(GtkButton *button, gpointer dialog) {
  gint selection;
  GtkWidget *maps_list = lookup_widget(dialog, "maps_list");
  GtkWidget *dlg = create_yes_no_dlg("Delete the map?", GTK_SIGNAL_FUNC(delete_map), maps_list);
  GtkWidget *yes = lookup_widget(dlg, "yes");
  char **namelist;
  selection = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(maps_list), "row"));
  namelist = gtk_object_get_data(GTK_OBJECT(maps_list), "maps");
  gtk_object_set_data(GTK_OBJECT(yes), "filename", namelist[selection]);
  gtk_widget_show_all(dlg);
}

void delete_map(GtkButton *button, gpointer user_data) {
  char *filename = gtk_object_get_data(GTK_OBJECT(button), "filename");
  unlink(filename);
  dlg_options_list_maps(GTK_WIDGET(user_data));
}

void save_button_clicked(GtkButton *button, gpointer dialog) {
  GtkWidget *save_dlg;
  GtkWidget *yes;
  save_dlg = create_dlg_save();
  yes = lookup_widget(save_dlg, "yes");
  gtk_signal_connect(GTK_OBJECT(yes), "clicked",
                     GTK_SIGNAL_FUNC (save_map), save_dlg); 
  gtk_object_set_data(GTK_OBJECT(save_dlg), "dlg_options", dialog);
  gtk_widget_show_all(save_dlg);
}

void cancel_button_clicked(GtkButton *button, gpointer dialog) {
  game_refuse_options(dialog, NULL, NULL);
}

void load_button_clicked(GtkButton *button, gpointer dialog) {
  GtkWidget *maps_list = lookup_widget(dialog, "maps_list");
  gint selection;
  char **namelist;
  selection = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(maps_list), "row"));
  namelist = gtk_object_get_data(GTK_OBJECT(maps_list), "maps");
  load_map(namelist[selection], (GtkWidget *)dialog);
}

void save_map(GtkButton *button, GtkWidget *dialog) {
  GtkWidget *dlg_options = gtk_object_get_data(GTK_OBJECT(dialog), "dlg_options");
  GtkWidget *map_name = lookup_widget(dialog, "map_name");
  const char *name = gtk_entry_get_text(GTK_ENTRY(map_name));
  combat_game *game = gtk_object_get_data(GTK_OBJECT(dlg_options), "options");
  game->name = ggz_strdup(name);
  map_save(game);
  dlg_options_list_maps(gtk_object_get_data(GTK_OBJECT(dlg_options), "maps_list"));
}


void load_map(char *filename, GtkWidget *dialog) {
  GtkWidget *width, *height;
  GtkWidget *unit_spin;
  GtkWidget *options_widget;
  char options_name[14];
  int a;
  combat_game *map = (combat_game *)ggz_malloc(sizeof(combat_game));
  map->number = GPOINTER_TO_INT(
                  gtk_object_get_data(GTK_OBJECT(dialog), "number"));
  map->army = (char **)calloc(map->number+1, sizeof(char *));
  map->army[map->number] = (char *)calloc(12, sizeof(char));
  map->map = NULL;
  map->name = NULL;
  map->options = 0;
  map_load(map, filename, NULL);
  /* Get the widgets */
  width = lookup_widget(dialog, "width");
  height = lookup_widget(dialog, "height");
  // Width / Height
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(width), map->width);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(height), map->height);
  // Army
  for (a = 0; a < 12; a++) {
    unit_spin = lookup_widget(dialog, spin_name[a]);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(unit_spin), ARMY(map, a));
  }
  // Options
  for (a = 0; a < 15; a++) {
    sprintf(options_name, "opt_bin1[%d]", a);
    options_widget = lookup_widget(dialog, options_name);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(options_widget), map->options&(1<<a));
  }
  options_widget = lookup_widget(dialog, "opt_bin1[15]");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(options_widget), !(map->options&OPT_SHOW_ENEMY_UNITS));
  // Terrain data
  gtk_object_set_data(GTK_OBJECT(dialog), "options", map);
  draw_mini_board(dialog);
}

GtkWidget*
create_dlg_save (void)
{
  GtkWidget *dlg_save;
  GtkWidget *dialog_vbox3;
  GtkWidget *hbox6;
  GtkWidget *label12;
  GtkWidget *map_name;
  GtkWidget *dialog_action_area3;
  GtkWidget *hbuttonbox4;
  GtkWidget *yes;
  GtkWidget *no;

  dlg_save = gtk_dialog_new ();
  gtk_widget_set_name (dlg_save, "dlg_save");
  gtk_object_set_data (GTK_OBJECT (dlg_save), "dlg_save", dlg_save);
  gtk_window_set_title (GTK_WINDOW (dlg_save), _("Save Map?"));
  gtk_window_set_modal (GTK_WINDOW (dlg_save), TRUE);
  gtk_window_set_policy (GTK_WINDOW (dlg_save), TRUE, TRUE, FALSE);

  dialog_vbox3 = GTK_DIALOG (dlg_save)->vbox;
  gtk_widget_set_name (dialog_vbox3, "dialog_vbox3");
  gtk_object_set_data (GTK_OBJECT (dlg_save), "dialog_vbox3", dialog_vbox3);
  gtk_widget_show (dialog_vbox3);

  hbox6 = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_name (hbox6, "hbox6");
  gtk_widget_ref (hbox6);
  gtk_object_set_data_full (GTK_OBJECT (dlg_save), "hbox6", hbox6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox6);
  gtk_box_pack_start (GTK_BOX (dialog_vbox3), hbox6, TRUE, TRUE, 0);

  label12 = gtk_label_new (_("Save map as:"));
  gtk_widget_set_name (label12, "label12");
  gtk_widget_ref (label12);
  gtk_object_set_data_full (GTK_OBJECT (dlg_save), "label12", label12,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label12);
  gtk_box_pack_start (GTK_BOX (hbox6), label12, FALSE, FALSE, 0);
  gtk_misc_set_padding (GTK_MISC (label12), 8, 0);

  map_name = gtk_entry_new ();
  gtk_widget_set_name (map_name, "map_name");
  gtk_widget_ref (map_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_save), "map_name", map_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (map_name);
  gtk_box_pack_start (GTK_BOX (hbox6), map_name, TRUE, TRUE, 5);
  gtk_widget_set_usize (map_name, 158, 19);

  dialog_action_area3 = GTK_DIALOG (dlg_save)->action_area;
  gtk_widget_set_name (dialog_action_area3, "dialog_action_area3");
  gtk_object_set_data (GTK_OBJECT (dlg_save), "dialog_action_area3", dialog_action_area3);
  gtk_widget_show (dialog_action_area3);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area3), 10);

  hbuttonbox4 = gtk_hbutton_box_new ();
  gtk_widget_set_name (hbuttonbox4, "hbuttonbox4");
  gtk_widget_ref (hbuttonbox4);
  gtk_object_set_data_full (GTK_OBJECT (dlg_save), "hbuttonbox4", hbuttonbox4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox4);
  gtk_box_pack_start (GTK_BOX (dialog_action_area3), hbuttonbox4, TRUE, TRUE, 0);

  yes = gtk_button_new_with_label (_("Yep"));
  gtk_widget_set_name (yes, "button3");
  gtk_widget_ref (yes);
  gtk_object_set_data_full (GTK_OBJECT (dlg_save), "yes", yes,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (yes);
  gtk_container_add (GTK_CONTAINER (hbuttonbox4), yes);
  GTK_WIDGET_SET_FLAGS (yes, GTK_CAN_DEFAULT);

  no = gtk_button_new_with_label (_("Nope"));
  gtk_widget_set_name (no, "button4");
  gtk_widget_ref (no);
  gtk_object_set_data_full (GTK_OBJECT (dlg_save), "no", no,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (no);
  gtk_container_add (GTK_CONTAINER (hbuttonbox4), no);
  GTK_WIDGET_SET_FLAGS (no, GTK_CAN_DEFAULT);

  gtk_signal_connect_object (GTK_OBJECT (no), "clicked",
                      GTK_SIGNAL_FUNC (gtk_widget_destroy),
                      GTK_OBJECT(dlg_save));
  gtk_signal_connect_object_after (GTK_OBJECT (yes), "clicked",
                      GTK_SIGNAL_FUNC (gtk_widget_destroy),
                      GTK_OBJECT(dlg_save));

  return dlg_save;
}

void dlg_options_update(GtkWidget *dlg_options) {
	GtkWidget *width, *height;
	GtkWidget *unit_spin;
  GtkWidget *option_button;
  char option_name[14];
	int a;
  combat_game *options;

  // Gets old value or allocs it
  options = gtk_object_get_data(GTK_OBJECT(dlg_options), "options");
  if (!options) {
    options = (combat_game *)ggz_malloc(sizeof(combat_game));
    options->number = GPOINTER_TO_INT(
                      gtk_object_get_data(GTK_OBJECT(dlg_options), "number"));
    options->army = (char **)calloc(options->number+1, sizeof(char *));
    options->army[options->number] = (char *)calloc(12, sizeof(char));
    options->map = NULL;
    options->name = NULL;
    options->options = 0;
  }
  gtk_object_set_data(GTK_OBJECT(dlg_options), "options", options);

	// Gets width / height
	width = lookup_widget(dlg_options, "width");
	height = lookup_widget(dlg_options, "height");
	options->width = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(width));
	options->height = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(height));

	// Gets army data
	for (a = 0; a < 12; a++) {
		unit_spin = gtk_object_get_data(GTK_OBJECT(dlg_options), spin_name[a]);
		ARMY(options, a) = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(unit_spin));
	}

  // Gets options
  options->options = 0;
  for (a = 0; a < 15; a++) {
    sprintf(option_name, "opt_bin1[%d]", a);
    option_button = lookup_widget(dlg_options, option_name);
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(option_button)))
        options->options |= (1<<a);
  }
  // OPT_SHOW_ENEMY_UNITS
  option_button = lookup_widget(dlg_options, "opt_bin1[15]");
  if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(option_button)))
    options->options |= OPT_SHOW_ENEMY_UNITS;

  // Map was changed! It doesn't have a name now!
  if (options->name) {
    ggz_free(options->name);
    options->name = NULL;
  }
	
	// Sets data
	gtk_object_set_data(GTK_OBJECT(dlg_options), "options", options);

  if (options->map)
    update_counters(dlg_options);

}

void update_counters(GtkWidget *dlg_options) {
  combat_game *options;
  GtkWidget *open_label, *black_label, *lake_label, *player_1_label, *player_2_label;
  GtkWidget *army_total, *army_player_1, *army_player_2;
  int a, n[5] = {0, 0, 0, 0, 0};
  int units = 0;
  char label[48];

  /* Map data counters */
  // Get options
  options = gtk_object_get_data(GTK_OBJECT(dlg_options), "options");

  // Get widgets
  open_label = GTK_BIN( lookup_widget(dlg_options, "open") )->child;
  black_label = GTK_BIN( lookup_widget(dlg_options, "black") )->child;
  lake_label = GTK_BIN( lookup_widget(dlg_options, "lake") )->child;
  player_1_label = GTK_BIN( lookup_widget(dlg_options, "player_1") )->child;
  player_2_label = GTK_BIN( lookup_widget(dlg_options, "player_2") )->child;

  // Count each type
  for (a = 0; a < options->width * options->height; a++) {
    if (options->map[a].type == OPEN)
      n[0]++;
    if (options->map[a].type == BLACK)
      n[1]++;
    if (options->map[a].type == LAKE)
      n[2]++;
    if (options->map[a].type == PLAYER_1)
      n[3]++;
    if (options->map[a].type == PLAYER_2)
      n[4]++;
  }

  /* Update the labels */
  // Open
  if (n[0] > 0) {
    sprintf(label, "Open (%d)", n[0]);
    gtk_label_set_text(GTK_LABEL(open_label), label);
  } else {
    gtk_label_set_text(GTK_LABEL(open_label), "Open");
  }
  // Black
  if (n[1] > 0) {
    sprintf(label, "Black (%d)", n[1]);
    gtk_label_set_text(GTK_LABEL(black_label), label);
  } else {
    gtk_label_set_text(GTK_LABEL(black_label), "Black");
  }
  // Open
  if (n[2] > 0) {
    sprintf(label, "Lake (%d)", n[2]);
    gtk_label_set_text(GTK_LABEL(lake_label), label);
  } else {
    gtk_label_set_text(GTK_LABEL(lake_label), "Lake");
  }
  // Open
  if (n[3] > 0) {
    sprintf(label, "Player 1 (%d)", n[3]);
    gtk_label_set_text(GTK_LABEL(player_1_label), label);
  } else {
    gtk_label_set_text(GTK_LABEL(player_1_label), "Player 1");
  }
  // Open
  if (n[4] > 0) {
    sprintf(label, "Player 2 (%d)", n[4]);
    gtk_label_set_text(GTK_LABEL(player_2_label), label);
  } else {
    gtk_label_set_text(GTK_LABEL(player_2_label), "Player 2");
  }

  /* Army data counters */

  // Get widgets
  army_total = lookup_widget(dlg_options, "army_total");
  army_player_1 = lookup_widget(dlg_options, "army_player_1");
  army_player_2 = lookup_widget(dlg_options, "army_player_2");

  // Count units
  for (a = 0; a < 12; a++)
    units+=ARMY(options, a);

  // Set widgets
  sprintf(label, "Total: %d", units);
  gtk_label_set_text(GTK_LABEL(army_total), label);
  
  sprintf(label, "Player 1: %d", n[3]);
  gtk_label_set_text(GTK_LABEL(army_player_1), label);

  sprintf(label, "Player 2: %d", n[4]);
  gtk_label_set_text(GTK_LABEL(army_player_2), label);
}

int dlg_options_list_maps(GtkWidget *dlg) {
  char **names;
  char *char_match[2];
  char **clist_name;
  int a, len, current = -1;
  int def = -1;
  names = map_list();
  current = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(dlg), "row"));
  clist_name = (char **)calloc(1, sizeof(char *));
  clist_name[0] = (char *)ggz_malloc(64 * sizeof(char));
  gtk_clist_clear(GTK_CLIST(dlg));
  gtk_object_set_data(GTK_OBJECT(dlg), "maps", names);
  for (a = 0; names[a]; a++) {
    char_match[0] = strrchr(names[a], '/');
    char_match[1] = strrchr(char_match[0], '.');
    len = strlen(char_match[0]) + 1;
    if (char_match[1])
      len -= strlen(char_match[1]);
    len-=2;
    strncpy(clist_name[0], char_match[0]+1, len);
    clist_name[0][len] = 0;
    if (strcmp(clist_name[0], "Default") == 0)
      def = a;
    gtk_clist_append(GTK_CLIST(dlg), clist_name);
  }
  if (current >= 0)
    gtk_clist_select_row(GTK_CLIST(dlg), current, 0);
  return def;
}

gboolean mini_board_expose               (GtkWidget       *widget, GdkEventExpose  *event, gpointer         user_data) {
	gdk_draw_pixmap( widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], mini_buf, event->area.x, event->area.y, event->area.x, event->area.y,
			event->area.width, event->area.height);
	return 1;
}

gboolean mini_board_configure            (GtkWidget       *widget, GdkEventConfigure *event, gpointer         user_data) {
  combat_game *options;
	options = gtk_object_get_data(GTK_OBJECT(user_data), "options");
  if (!options) {
    dlg_options_update(user_data);
  }
  init_mini_board(user_data);
	gtk_object_set_data(GTK_OBJECT(user_data), "options", options);
	return 1;
}

gboolean mini_board_click         (GtkWidget       *widget, GdkEventButton  *event, gpointer         user_data) {
	int width, height, pix_width, pix_height, x, y;
  combat_game *options;
	GSList *radio_button;
	int current = OPEN;

  options = gtk_object_get_data(GTK_OBJECT(user_data), "options");

	gdk_window_get_size(widget->window, &width, &height);

	pix_width = width/options->width;
	pix_height = height/options->height;

	x = event->x/(pix_width);
	y = event->y/(pix_height);

	radio_button = gtk_radio_button_group(GTK_RADIO_BUTTON(lookup_widget(user_data, "open")));

	while (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_button->data))) {
		radio_button = radio_button->next;
		current++;
		if (radio_button == NULL) {
			game_status("Big bad error!");
			return 0;
		}
	}

	current = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(radio_button->data), "type"));

	options->map[y*options->width+x].type = current;

  // Map was changed! It doesn't have a name now!
  if (options->name) {
    ggz_free(options->name);
    options->name = NULL;
  }

	draw_mini_board(user_data);

	return 1;
}

gboolean init_preview (GtkWidget *widget, GdkEventConfigure *event, 
                       gpointer user_data) {
  int width, height;
  gdk_window_get_size(widget->window, &width, &height);

	if (preview_buf)
		gdk_pixmap_unref(preview_buf);
	preview_buf = gdk_pixmap_new( widget->window, width, height, -1 );
  gtk_object_set_data(GTK_OBJECT(widget), "clean", GINT_TO_POINTER(FALSE));

  draw_preview(GTK_WIDGET(user_data));

  return TRUE;

}

gboolean preview_expose (GtkWidget *widget, GdkEventExpose *event, 
                       gpointer user_data) {
  if (GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(widget), "clean")))
    gdk_draw_pixmap( widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], preview_buf, event->area.x, event->area.y, event->area.x, event->area.y,
        event->area.width, event->area.height);
	return 1;
}


gboolean draw_preview (GtkWidget *dlg_options) {
  GtkWidget *widget = lookup_widget(dlg_options, "preview_board");
  combat_game *preview;
  int width, height;
  int t_width, t_height;
  int x, y;
  GdkGC *solid_gc;
  preview = gtk_object_get_data(GTK_OBJECT(dlg_options), "preview");
  if (!preview)
    return FALSE;
  if (!widget || !widget->window)
    return FALSE;
  if (!preview_buf)
    return FALSE;
  if (!dlg_options)
    return FALSE;

  solid_gc = gdk_gc_new(widget->window);
  gdk_window_get_size(widget->window, &width, &height);

  // Draw background
  gdk_draw_rectangle(preview_buf, 
                     dlg_options->style->bg_gc[GTK_WIDGET_STATE(dlg_options)],
                     TRUE,
                     0, 0, width, height);

  // Gets the size of each square
  t_width = width / preview->width;
  t_height = height / preview->height;

  // Draw terrain
  for (y = 0; y < preview->height; y++) {
    for (x = 0; x < preview->width; x++) {
      switch (preview->map[y*preview->width+x].type) {
        case (OPEN):
          gdk_gc_set_foreground(solid_gc, &open_color);
          break;
				case (LAKE):
					gdk_gc_set_foreground(solid_gc, &lake_color);
					break;
				case (BLACK):
					gdk_gc_set_foreground(solid_gc, &widget->style->black);
					break;
				case (PLAYER_1):
					gdk_gc_set_foreground(solid_gc, &player_colors[0]);
					break;
				case (PLAYER_2):
					gdk_gc_set_foreground(solid_gc, &player_colors[1]);
					break;
			}
			gdk_draw_rectangle( preview_buf,
					solid_gc,
					TRUE,
					x*t_width, y*t_height,
					t_width, t_height);
		}
	}

	// Draw lines
	for (x = 0; x < preview->width; x++) {
		gdk_draw_line(preview_buf, widget->style->black_gc,
				x*t_width, 0, x*t_width, preview->height*t_height);
	}

	for (y = 0; y < preview->height; y++) {
		gdk_draw_line(preview_buf, widget->style->black_gc,
				0, y*t_height, preview->width*t_width, y*t_height);
	}

  gtk_object_set_data(GTK_OBJECT(widget), "clean", GINT_TO_POINTER(TRUE));

  gtk_widget_draw(widget, NULL);

  return TRUE;


}

void init_mini_board(GtkWidget *dlg_options) {
	int width, height;
	GtkWidget *widget = lookup_widget(dlg_options, "mini_board");

	if (widget == NULL) {
		game_status("Can't find widget!");
		return;
	}

	// Init the pixmap
	gdk_window_get_size(widget->window, &width, &height);
	if (mini_buf)
		gdk_pixmap_unref(mini_buf);
	mini_buf = gdk_pixmap_new( widget->window, width, height, -1 );

	draw_mini_board(dlg_options);

}

void draw_mini_board(GtkWidget *dlg_options) {
	GtkWidget *widget = lookup_widget(dlg_options, "mini_board");
	int width, height, pix_width, pix_height, i, j;
  combat_game *options;
	GdkGC *solid_gc;

  options = gtk_object_get_data(GTK_OBJECT(dlg_options), "options");
  if (!options)
    dlg_options_update(dlg_options);
  if (!widget || !widget->window)
    return;

	solid_gc = gdk_gc_new(widget->window);

	gdk_window_get_size(widget->window, &width, &height);

	// Draw background
	gdk_draw_rectangle (mini_buf,
			dlg_options->style->bg_gc[GTK_WIDGET_STATE(dlg_options)],
      TRUE, 0, 0, width, height);

	// Gets the size of each square
	pix_width = width/options->width;
	pix_height = height/options->height;

	// Draw terrain
	for (j = 0; j < options->height; j++) {
		for (i = 0; i < options->width; i++) {
			switch (options->map[j*options->width+i].type) {
				case (OPEN):
					gdk_gc_set_foreground(solid_gc, &open_color);
					break;
				case (LAKE):
					gdk_gc_set_foreground(solid_gc, &lake_color);
					break;
				case (BLACK):
					gdk_gc_set_foreground(solid_gc, &widget->style->black);
					break;
				case (PLAYER_1):
					gdk_gc_set_foreground(solid_gc, &player_colors[0]);
					break;
				case (PLAYER_2):
					gdk_gc_set_foreground(solid_gc, &player_colors[1]);
					break;
			}
			gdk_draw_rectangle( mini_buf,
					solid_gc,
					TRUE,
					i*pix_width, j*pix_height,
					pix_width, pix_height);
		}
	}


	// Draw lines
	for (i = 0; i < options->width; i++) {
		gdk_draw_line(mini_buf, widget->style->black_gc,
				i*pix_width, 0, i*pix_width, options->height*pix_height);
	}

	for (i = 0; i < options->height; i++) {
		gdk_draw_line(mini_buf, widget->style->black_gc,
				0, i*pix_height, options->width*pix_width, i*pix_height);
	}

	gtk_widget_draw(widget, NULL);

  update_counters(dlg_options);

}
	

