#include <gtk/gtk.h>

#include "game.h"


GtkWidget *gtk_player_info_new(GtkWidget *parent, char *name) {
	GtkWidget *player_info;
	GtkWidget *player_name;
	GtkWidget *unit_view;
	GtkWidget *unit_list;
	GtkWidget *name_lbl;
	GtkWidget *power_lbl;
	GtkWidget *number_lbl;

	// Create the initial vbox
  player_info = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (player_info);
  gtk_object_set_data_full (GTK_OBJECT (parent), name, player_info,
                            (GtkDestroyNotify) gtk_widget_unref);

	// Name of the player
  player_name = gtk_label_new ("Player Name");
  gtk_widget_ref (player_name);
  gtk_object_set_data_full (GTK_OBJECT(player_info), "player_name", player_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_name);
  gtk_box_pack_start (GTK_BOX(player_info), player_name, FALSE, FALSE, 0);
  gtk_widget_set_usize (player_name, 70, -2);

	// List of units
  unit_view = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (unit_view);
  gtk_object_set_data_full (GTK_OBJECT (player_info), "unit_view", unit_view,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (unit_view);
  gtk_box_pack_start (GTK_BOX (player_info), unit_view, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (unit_view), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  unit_list = gtk_clist_new (3);
  gtk_widget_ref (unit_list);
  gtk_object_set_data_full (GTK_OBJECT (player_info), "unit_list", unit_list,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (unit_list);
  gtk_container_add (GTK_CONTAINER (unit_view), unit_list);
  gtk_widget_set_usize (unit_list, 150, -2);
  gtk_clist_set_column_width (GTK_CLIST (unit_list), 0, 50);
  gtk_clist_set_column_width (GTK_CLIST (unit_list), 1, 55);
  gtk_clist_set_column_width (GTK_CLIST (unit_list), 2, 45);
  gtk_clist_column_titles_show (GTK_CLIST (unit_list));

	// Collums
  name_lbl = gtk_label_new ("Name");
  gtk_widget_ref (name_lbl);
  gtk_object_set_data_full (GTK_OBJECT (player_info), "name_lbl", name_lbl,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (name_lbl);
  gtk_clist_set_column_widget (GTK_CLIST (unit_list), 0, name_lbl);

  number_lbl = gtk_label_new ("Number");
  gtk_widget_ref (number_lbl);
  gtk_object_set_data_full (GTK_OBJECT (player_info), "number_lbl", number_lbl,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (number_lbl);
  gtk_clist_set_column_widget (GTK_CLIST (unit_list), 1, number_lbl);

  power_lbl = gtk_label_new ("Power");
  gtk_widget_ref (power_lbl);
  gtk_object_set_data_full (GTK_OBJECT (player_info), "Power", power_lbl,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (power_lbl);
  gtk_clist_set_column_widget (GTK_CLIST (unit_list), 2, power_lbl);


	// Connect signals
	gtk_signal_connect (GTK_OBJECT(unit_list), "select-row",
										  GTK_SIGNAL_FUNC (game_unit_list_handle),
											GINT_TO_POINTER(1));
	gtk_signal_connect (GTK_OBJECT(unit_list), "unselect-row",
											GTK_SIGNAL_FUNC (game_unit_list_handle),
											GINT_TO_POINTER(-1));
	

	return player_info;
}
