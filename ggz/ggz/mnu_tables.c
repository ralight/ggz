#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <string.h>

#include "connect.h"
#include "datatypes.h"
#include "dlg_error.h"
#include "dlg_launch.h"
#include "dlg_msgbox.h"
#include "easysock.h"
#include "err_func.h"
#include "protocols.h"

/* Globals for this menu */
GtkWidget *mnu_tables;
extern GtkWidget *dlg_launch;
extern struct ConnectInfo connection;
extern gint selected_table;
extern struct GameTables tables;

/* Local callbacks which no other file will call */
void on_launch1_activate(GtkButton * button, gpointer user_data);
void on_join1_activate(GtkButton * button, gpointer user_data);
void on_options1_activate(GtkButton * button, gpointer user_data);
void on_players1_activate(GtkButton * button, gpointer user_data);


GtkWidget*
create_mnu_tables (void)
{
  GtkWidget *mnu_table;
  GtkAccelGroup *mnu_table_accels;
  GtkWidget *launch1;
  GtkWidget *join1;
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

  join1 = gtk_menu_item_new_with_label ("Join");
  gtk_widget_ref (join1);
  gtk_object_set_data_full (GTK_OBJECT (mnu_table), "join1", join1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (join1);
  gtk_container_add (GTK_CONTAINER (mnu_table), join1);

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
  gtk_signal_connect (GTK_OBJECT (join1), "activate",
                      GTK_SIGNAL_FUNC (on_join1_activate),
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
	dlg_launch = create_dlg_launch();
	gtk_widget_show(GTK_WIDGET(dlg_launch));
}

void on_join1_activate(GtkButton * button, gpointer user_data)
{
	dbg_msg("joining game");
	es_write_int(connection.sock, REQ_TABLE_JOIN);
	es_write_int(connection.sock, selected_table);
}

void on_options1_activate(GtkButton * button, gpointer user_data)
{
	msg(0,"Table Players","Table options will go here.\n Network protocol has to be made to get the info from the server");
}

void on_players1_activate(GtkButton * button, gpointer user_data)
{
	gint i=0;
	gchar buf[2048];
	
	strcpy(buf,"The selected table's players are as follows:\n\n");
	for (i=0;i<MAX_TABLE_SIZE;i++)
	{
		if(tables.info[selected_table].seats[i] == GGZ_SEAT_OPEN)
			strcat(buf,"\t<Open>\n");
		if(tables.info[selected_table].seats[i] == GGZ_SEAT_COMP)
			strcat(buf,"\t<Computer>\n");
		if((tables.info[selected_table].seats[i] >= 0)
		|| (tables.info[selected_table].seats[i] == GGZ_SEAT_RESV))
		{
			strcat(buf,"\t");
			strcat(buf,&tables.info[selected_table].names[i]);
			strcat(buf,"\n");
		}
	}

	msg(0,"Table Players",buf);
}
