#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "mnu_players.h"


/* Global GtkWidget for this dialog */
GtkWidget *mnu_players;

/* Local callbacks which no other file will call */
void on_info1_activate(GtkButton * button, gpointer user_data);
void on_stats1_activate(GtkButton * button, gpointer user_data);

GtkWidget*
create_mnu_players (void)
{
  GtkWidget *mnu_players;
  GtkAccelGroup *mnu_players_accels;
  GtkWidget *info1;
  GtkWidget *stats1;

  mnu_players = gtk_menu_new ();
  gtk_object_set_data (GTK_OBJECT (mnu_players), "mnu_players", mnu_players);
  mnu_players_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (mnu_players));

  info1 = gtk_menu_item_new_with_label ("Info");
  gtk_widget_ref (info1);
  gtk_object_set_data_full (GTK_OBJECT (mnu_players), "info1", info1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (info1);
  gtk_container_add (GTK_CONTAINER (mnu_players), info1);

  stats1 = gtk_menu_item_new_with_label ("Stats");
  gtk_widget_ref (stats1);
  gtk_object_set_data_full (GTK_OBJECT (mnu_players), "stats1", stats1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (stats1);
  gtk_container_add (GTK_CONTAINER (mnu_players), stats1);

  gtk_signal_connect (GTK_OBJECT (info1), "activate",
                      GTK_SIGNAL_FUNC (on_info1_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (stats1), "activate",
                      GTK_SIGNAL_FUNC (on_stats1_activate),
                      NULL);

  return mnu_players;
}

void on_info1_activate(GtkButton * button, gpointer user_data)
{

}

void on_stats1_activate(GtkButton * button, gpointer user_data)
{

}
