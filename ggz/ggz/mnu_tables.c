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
#include "support.h"

/* Globals for this menu */
GtkWidget *mnu_tables;
extern GtkWidget *dlg_launch;
extern struct ConnectInfo client;
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
  GtkWidget *mnu_tables;
  GtkAccelGroup *mnu_tables_accels;
  GtkWidget *launch;
  GtkWidget *join1;
  GtkWidget *separator5;
  GtkWidget *options1;
  GtkWidget *players1;

  mnu_tables = gtk_menu_new ();
  gtk_object_set_data (GTK_OBJECT (mnu_tables), "mnu_tables", mnu_tables);
  mnu_tables_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (mnu_tables));

  launch = gtk_menu_item_new_with_label (_("Launch"));
  gtk_widget_ref (launch);
  gtk_object_set_data_full (GTK_OBJECT (mnu_tables), "launch", launch,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (launch);
  gtk_container_add (GTK_CONTAINER (mnu_tables), launch);

  join1 = gtk_menu_item_new_with_label (_("Join"));
  gtk_widget_ref (join1);
  gtk_object_set_data_full (GTK_OBJECT (mnu_tables), "join1", join1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (join1);
  gtk_container_add (GTK_CONTAINER (mnu_tables), join1);

  separator5 = gtk_menu_item_new ();
  gtk_widget_ref (separator5);
  gtk_object_set_data_full (GTK_OBJECT (mnu_tables), "separator5", separator5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator5);
  gtk_container_add (GTK_CONTAINER (mnu_tables), separator5);
  gtk_widget_set_sensitive (separator5, FALSE);

  options1 = gtk_menu_item_new_with_label (_("Options"));
  gtk_widget_ref (options1);
  gtk_object_set_data_full (GTK_OBJECT (mnu_tables), "options1", options1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (options1);
  gtk_container_add (GTK_CONTAINER (mnu_tables), options1);

  players1 = gtk_menu_item_new_with_label (_("Players"));
  gtk_widget_ref (players1);
  gtk_object_set_data_full (GTK_OBJECT (mnu_tables), "players1", players1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (players1);
  gtk_container_add (GTK_CONTAINER (mnu_tables), players1);

  gtk_signal_connect (GTK_OBJECT (launch), "activate",
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

  return mnu_tables;
}

void on_launch1_activate(GtkButton * button, gpointer user_data)
{
	dlg_launch = create_dlg_launch();
	gtk_widget_show(GTK_WIDGET(dlg_launch));
}

void on_join1_activate(GtkButton * button, gpointer user_data)
{
	dbg_msg("joining game");
	es_write_int(client.sock, REQ_TABLE_JOIN);
	es_write_int(client.sock, selected_table);
}

void on_options1_activate(GtkButton * button, gpointer user_data)
{
	msg(0,_("Table Players"),_("Table options will go here.\n Network protocol has to be made to get the info from the server"));
}

void on_players1_activate(GtkButton * button, gpointer user_data)
{
#if 0	
	gint i=0;
	gchar buf[2048];
	strcpy(buf,_("The selected table's players are as follows:\n\n"));
	for (i=0;i<MAX_TABLE_SIZE;i++)
	{
		if(tables.info[selected_table].seats[i] == GGZ_SEAT_OPEN)
			strcat(buf,_("\t<Open>\n"));
		if(tables.info[selected_table].seats[i] == GGZ_SEAT_COMP)
			strcat(buf,_("\t<Computer>\n"));
		if((tables.info[selected_table].seats[i] >= 0)
		|| (tables.info[selected_table].seats[i] == GGZ_SEAT_RESV))
		{
			strcat(buf,"\t");
			strcat(buf, (char*)&tables.info[selected_table].names[i]);
			strcat(buf,"\n");
		}
	}

	msg(0,_("Table Players"),buf);
#endif
}
