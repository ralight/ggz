#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "support.h"
#include "dlg_options.h"
#include <stdio.h>
#include "game.h"
#include "confio.h"

GdkPixmap *mini_buf;
extern GdkColor lake_color;
extern GdkColor open_color;
extern GdkColor *player_colors;

extern int unitdefault[12];
extern char unitname[12][36];

extern char file_unit_name[12][36];

static const char spin_name[12][8] = {"spin_F", "spin_B", "spin_1", "spin_2", "spin_3", "spin_4", "spin_5", "spin_6", "spin_7", "spin_8", "spin_9", "spin_10"};

GtkWidget*
create_dlg_options (void)
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
  GtkWidget *sorry;
  GtkWidget *label5;
  GtkWidget *hbox5;
  GtkWidget *scrolledwindow1;
  GtkWidget *maps_list;
  GtkWidget *label10;
  GtkWidget *vbox2;
  GtkWidget *table2;
  GtkWidget *preview_board;
  GtkWidget *label11;
  GtkWidget *hbuttonbox3;
  GtkWidget *load;
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
	GtkWidget *unit_dummy_box;
	int i;

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
  gtk_box_pack_start (GTK_BOX (hbox3), mini_board, FALSE, TRUE, 0);
  gtk_widget_set_usize (mini_board, 240, 240);
  gtk_widget_set_events (mini_board, GDK_BUTTON_PRESS_MASK);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_set_name (vbox1, "vbox1");
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (hbox3), vbox1, FALSE, TRUE, 0);

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

  width_adj = gtk_adjustment_new (10, 1, 100, 1, 10, 10);
  width = gtk_spin_button_new (GTK_ADJUSTMENT (width_adj), 1, 0);
  gtk_widget_set_name (width, "width");
  gtk_widget_ref (width);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "width", width,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (width);
  gtk_table_attach (GTK_TABLE (table1), width, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  height_adj = gtk_adjustment_new (10, 1, 100, 1, 10, 10);
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

  sorry = gtk_label_new (_("No options yet! Sorry..."));
  gtk_widget_set_name (sorry, "sorry");
  gtk_widget_ref (sorry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "sorry", sorry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (sorry);
  gtk_container_add (GTK_CONTAINER (notebook1), sorry);

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
  gtk_box_pack_start (GTK_BOX (hbox5), scrolledwindow1, TRUE, TRUE, 0);
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
  gtk_widget_set_usize (preview_board, 100, 100);

  label11 = gtk_label_new (_("Map Preview"));
  gtk_widget_set_name (label11, "label11");
  gtk_widget_ref (label11);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "label11", label11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label11);
  gtk_table_attach (GTK_TABLE (table2), label11, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND),
                    (GtkAttachOptions) (GTK_EXPAND), 0, 0);

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
  gtk_widget_set_sensitive (cancel_button, FALSE);
  GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);

	// Create the army setup stuff
	unit_label_box = gtk_vbox_new (TRUE, 0);
	gtk_widget_show (unit_label_box);
	unit_spin_box = gtk_vbox_new (TRUE, 0);
	gtk_widget_show (unit_spin_box);
	unit_dummy_box = gtk_vbox_new (TRUE, 0);
  //gtk_container_add (GTK_CONTAINER (army_hbox), unit_label_box);
	//gtk_container_add (GTK_CONTAINER (army_hbox), unit_spin_box);
	gtk_box_pack_start (GTK_BOX (army_hbox), unit_label_box, FALSE, FALSE, 10);
	gtk_box_pack_start (GTK_BOX (army_hbox), unit_spin_box, TRUE, TRUE, 10);
	gtk_box_pack_start (GTK_BOX (army_hbox), unit_dummy_box, TRUE, TRUE, 20);
	for (i = 0; i < 12; i++) {
		unit_label[i] = gtk_label_new (unitname[i]);
  	gtk_widget_show (unit_label[i]);
  	gtk_box_pack_start (GTK_BOX (unit_label_box), unit_label[i], FALSE, FALSE, 0);
  	unit_spin_adj[i] = gtk_adjustment_new (unitdefault[i], 0, 100, 1, 10, 10);
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

	gtk_signal_connect(GTK_OBJECT (mini_board), "expose_event",
										 GTK_SIGNAL_FUNC (mini_board_expose), dlg_options);
	gtk_signal_connect(GTK_OBJECT (mini_board), "configure_event",
										 GTK_SIGNAL_FUNC (mini_board_configure), dlg_options);
	gtk_signal_connect(GTK_OBJECT (mini_board), "button_press_event",
									   GTK_SIGNAL_FUNC (mini_board_click), dlg_options);
	gtk_signal_connect(GTK_OBJECT (load), "clicked",
									   GTK_SIGNAL_FUNC (load_button_clicked), dlg_options);
	gtk_signal_connect(GTK_OBJECT (maps_list), "select-row",
									   GTK_SIGNAL_FUNC (maps_list_selected), dlg_options);
	gtk_signal_connect_object(GTK_OBJECT (width), "changed",
										 GTK_SIGNAL_FUNC (dlg_options_update), GTK_OBJECT (dlg_options));
	gtk_signal_connect_object_after(GTK_OBJECT (width), "changed",
										 GTK_SIGNAL_FUNC (init_mini_board), GTK_OBJECT (dlg_options));
	gtk_signal_connect_object(GTK_OBJECT (height), "changed",
										 GTK_SIGNAL_FUNC (dlg_options_update), GTK_OBJECT (dlg_options));
	gtk_signal_connect_object_after(GTK_OBJECT (height), "changed",
										 GTK_SIGNAL_FUNC (init_mini_board), GTK_OBJECT (dlg_options));

	dlg_options_update(dlg_options);

  dlg_options_list_maps(maps_list);

  gtk_widget_grab_default (ok_button);
  return dlg_options;
}

void maps_list_selected (GtkCList *clist, gint row, gint column,
	 											 GdkEventButton *event, gpointer user_data) {
  gtk_object_set_data(GTK_OBJECT(clist), "row", GINT_TO_POINTER(row));
}

void load_button_clicked(GtkButton *button, gpointer dialog) {
  GtkWidget *maps_list = lookup_widget(dialog, "maps_list");
  gint selection;
  char **namelist;
  selection = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(maps_list), "row"));
  namelist = gtk_object_get_data(GTK_OBJECT(maps_list), "maps");
  printf("Selected: %s\n", namelist[selection]);
  load_map(namelist[selection], (GtkWidget *)dialog);
}

void load_map(char *filename, GtkWidget *dialog) {
  int handle;
  GtkWidget *width, *height;
  GtkWidget *unit_spin;
  int a;
  char *terrain_data;
  handle = _ggzcore_confio_parse(filename, 0);
  if (handle < 0) {
    printf("Can't find the map?\n");
    return;
  }
  /* Get the widgets */
  width = lookup_widget(dialog, "width");
  height = lookup_widget(dialog, "height");
  /* Get the data from the file */
  // Width / Height
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(width), 
           _ggzcore_confio_read_int(handle, "map", "width", 10));
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(height),
           _ggzcore_confio_read_int(handle, "map", "height", 10));
  // Army
  for (a = 0; a < 12; a++) {
    unit_spin = lookup_widget(dialog, spin_name[a]);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(unit_spin),
        _ggzcore_confio_read_int(handle, "army", file_unit_name[a], 0));
  }
  // Terrain data
  terrain_data = _ggzcore_confio_read_string(handle, "map", "data", NULL);
  if (terrain_data) {
    a = strlen(terrain_data);
    while (--a>=0) {
      switch(terrain_data[a]) {
        case 'O':
          terrain_data[a] = OPEN;
          break;
        case 'L':
          terrain_data[a] = LAKE;
          break;
        case 'N':
          terrain_data[a] = BLACK;
          break;
        case '1':
          terrain_data[a] = PLAYER_1;
          break;
        case '2':
          terrain_data[a] = PLAYER_2;
          break;
        default:
          terrain_data[a] = OPEN;
          break;
      }
    }
    gtk_object_set_data(GTK_OBJECT(dialog), "map_data", terrain_data);
    draw_mini_board(dialog);
  }

  dlg_options_update(dialog);


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
	int *army_data = NULL;
	int a;
	int width_v, height_v;

	// Gets width / height
	width = lookup_widget(dlg_options, "width");
	height = lookup_widget(dlg_options, "height");
	width_v = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(width));
	height_v = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(height));

	// Try to get old army data
	army_data = gtk_object_get_data(GTK_OBJECT(dlg_options), "army_data");
	if (!army_data)
		army_data = (int *)malloc(sizeof(int) * 12);

	// Gets army data
	for (a = 0; a < 12; a++) {
		unit_spin = gtk_object_get_data(GTK_OBJECT(dlg_options), spin_name[a]);
		army_data[a] = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(unit_spin));
	}
	
	// Sets data
	gtk_object_set_data(GTK_OBJECT(dlg_options), "width_v", GINT_TO_POINTER(width_v));
	gtk_object_set_data(GTK_OBJECT(dlg_options), "height_v", GINT_TO_POINTER(height_v));
	gtk_object_set_data(GTK_OBJECT(dlg_options), "army_data", army_data);

}

int maps_only(const struct dirent *entry);

void dlg_options_list_maps(GtkWidget *dlg) {
  int n;
  struct dirent **dirlist;
  char **clist_names;
  char **names;

  n = scandir("maps", &dirlist, maps_only, alphasort);
  clist_names = (char **)calloc(1, sizeof(char *));
  names = (char **)calloc(n, sizeof(char *));
  printf("Number of maps: %d\n", n);
  while (n-- > 0) {
    clist_names[0] = dirlist[n]->d_name;
    names[n] = (char *)malloc(15 + strlen(dirlist[n]->d_name));
    strcpy(names[n], "maps/");
    strcat(names[n], dirlist[n]->d_name);
    gtk_clist_prepend(GTK_CLIST(dlg), clist_names);
  }
  gtk_object_set_data(GTK_OBJECT(dlg), "maps", names);
}

gboolean mini_board_expose               (GtkWidget       *widget, GdkEventExpose  *event, gpointer         user_data) {
	gdk_draw_pixmap( widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], mini_buf, event->area.x, event->area.y, event->area.x, event->area.y,
			event->area.width, event->area.height);
	return 1;
}

gboolean mini_board_configure            (GtkWidget       *widget, GdkEventConfigure *event, gpointer         user_data) {
	char *map_data = gtk_object_get_data(GTK_OBJECT(user_data), "map_data");
	if (!map_data)
		init_mini_board(user_data);
	return 1;
}

gboolean mini_board_click         (GtkWidget       *widget, GdkEventButton  *event, gpointer         user_data) {
	int width, height, width_v, height_v, pix_width, pix_height, x, y;
	char *map_data = gtk_object_get_data(GTK_OBJECT(user_data), "map_data");
	GSList *radio_button;
	int current = OPEN;

	gdk_window_get_size(widget->window, &width, &height);

	width_v = GPOINTER_TO_INT (gtk_object_get_data(GTK_OBJECT(user_data), "width_v"));
	height_v = GPOINTER_TO_INT (gtk_object_get_data(GTK_OBJECT(user_data), "height_v"));

	pix_width = width/width_v;
	pix_height = height/height_v;

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

	map_data[y*width_v+x] = current;

	draw_mini_board(user_data);

	return 1;
}

void init_mini_board(GtkWidget *dlg_options) {
	int width, height, a;
	GtkWidget *widget = lookup_widget(dlg_options, "mini_board");
	char *map_data;

	if (widget == NULL) {
		game_status("Can't find widget!");
		return;
	}

	// Init the pixmap
	gdk_window_get_size(widget->window, &width, &height);
	printf("W: %d, H: %d\n", width, height);
	if (mini_buf)
		gdk_pixmap_unref(mini_buf);
	mini_buf = gdk_pixmap_new( widget->window, width, height, -1 );

	// Now init the data
	width = GPOINTER_TO_INT (gtk_object_get_data(GTK_OBJECT(dlg_options), "width_v"));
	height = GPOINTER_TO_INT (gtk_object_get_data(GTK_OBJECT(dlg_options), "height_v"));
	map_data = (char *)malloc(sizeof(char) * width * height + 1);
	for (a = 0; a < width * height; a++)
		map_data[a] = OPEN;
	gtk_object_set_data(GTK_OBJECT(dlg_options), "map_data", map_data);

	draw_mini_board(dlg_options);

}

void draw_mini_board(GtkWidget *dlg_options) {
	GtkWidget *widget = lookup_widget(dlg_options, "mini_board");
	int width, height, width_v, height_v, pix_width, pix_height, i, j;
	char *map_data = gtk_object_get_data(GTK_OBJECT(dlg_options), "map_data");
	GdkGC *solid_gc;

	solid_gc = gdk_gc_new(widget->window);

	gdk_window_get_size(widget->window, &width, &height);

	width_v = GPOINTER_TO_INT (gtk_object_get_data(GTK_OBJECT(dlg_options), "width_v"));
	height_v = GPOINTER_TO_INT (gtk_object_get_data(GTK_OBJECT(dlg_options), "height_v"));

	// Draw background
	gdk_draw_rectangle (mini_buf,
			dlg_options->style->white_gc, TRUE, 0, 0, width, height);

	// Gets the size of each square
	pix_width = width/width_v;
	pix_height = height/height_v;

	// Draw terrain
	for (j = 0; j < height_v; j++) {
		for (i = 0; i < width_v; i++) {
			switch (map_data[j*width_v+i]) {
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
	for (i = 0; i < width_v; i++) {
		gdk_draw_line(mini_buf, widget->style->black_gc,
				i*pix_width, 0, i*pix_width, height);
	}

	for (i = 0; i < height_v; i++) {
		gdk_draw_line(mini_buf, widget->style->black_gc,
				0, i*pix_height, width, i*pix_height);
	}

	gtk_widget_draw(widget, NULL);


}
	

