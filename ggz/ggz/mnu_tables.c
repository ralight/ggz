#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "mnu_tables.h"


/* Global GtkWidget for this dialog */
GtkWidget *mnu_table;

/* Local callbacks which no other file will call */
void on_launch1_activate(GtkButton * button, gpointer user_data);
void on_options1_activate(GtkButton * button, gpointer user_data);
void on_players1_activate(GtkButton * button, gpointer user_data);


GtkWidget*
create_mnu_tables (void)
{
  GtkWidget *mnu_table;
  GtkAccelGroup *mnu_table_accels;
  GtkWidget *launch1;
  GtkWidget *separator5;
  GtkWidget *options1;
  GtkWidget *players1;

  mnu_table = gtk_menu_new ();
  gtk_object_set_data (GTK_OBJECT (mnu_table), "mnu_table", mnu_table);
  mnu_table_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (mnu_table));

  launch1 = gtk_menu_item_new_with_label ("Launch");
  gtk_widget_ref (launch1);
  gtk_object_set_data_full (GTK_OBJECT (mnu_table), "launch1", launch1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (launch1);
  gtk_container_add (GTK_CONTAINER (mnu_table), launch1);

  separator5 = gtk_menu_item_new ();
  gtk_widget_ref (separator5);
  gtk_object_set_data_full (GTK_OBJECT (mnu_table), "separator5", separator5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator5);
  gtk_container_add (GTK_CONTAINER (mnu_table), separator5);
  gtk_widget_set_sensitive (separator5, FALSE);

  options1 = gtk_menu_item_new_with_label ("Options");
  gtk_widget_ref (options1);
  gtk_object_set_data_full (GTK_OBJECT (mnu_table), "options1", options1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (options1);
  gtk_container_add (GTK_CONTAINER (mnu_table), options1);

  players1 = gtk_menu_item_new_with_label ("Players");
  gtk_widget_ref (players1);
  gtk_object_set_data_full (GTK_OBJECT (mnu_table), "players1", players1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (players1);
  gtk_container_add (GTK_CONTAINER (mnu_table), players1);

  gtk_signal_connect (GTK_OBJECT (launch1), "activate",
                      GTK_SIGNAL_FUNC (on_launch1_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (options1), "activate",
                      GTK_SIGNAL_FUNC (on_options1_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (players1), "activate",
                      GTK_SIGNAL_FUNC (on_players1_activate),
                      NULL);

  return mnu_table;
}

void on_launch1_activate(GtkButton * button, gpointer user_data)
{

}

void on_options1_activate(GtkButton * button, gpointer user_data)
{

}

void on_players1_activate(GtkButton * button, gpointer user_data)
{

}
