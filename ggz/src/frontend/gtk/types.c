/*
 * File: info.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: types.c 6287 2004-11-06 08:47:13Z jdorje $
 *
 * This dialog is used to display information about a selected room to
 * the user.
 *
 * Copyright (C) 2001 Justin Zaun.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <ggzcore.h>

#include "client.h"
#include "types.h"
#include "msgbox.h"
#include "server.h"
#include "support.h"

static GtkWidget *types_dialog;
static GtkWidget* create_dlg_types(void);

#if 0 /* currently unused */
static gboolean types_clist_events(GtkWidget *widget, GdkEvent *event,
				   gpointer data);
static void types_upgrade(GtkMenuItem *menuitem, gpointer data);
static void types_URL(GtkMenuItem *menuitem, gpointer data);
#endif
static void types_filter_button(GtkWidget *widget, gpointer data);


/* types_create_or_raise() - Displays the dialog or updates current dialog
 *                          with game list
 *
 * Recieves:
 *
 * Returns:
 */

void types_create_or_raise(void)
{
	GtkWidget *tmp, *menu;
	GtkWidget  *menuitem;
	gchar *gtype[6];
	gint x;
	GGZGameType *gt;

	if (!types_dialog) {
		types_dialog = create_dlg_types();

		tmp = lookup_widget(types_dialog, "filter_optionmenu");
		menu = gtk_option_menu_get_menu(GTK_OPTION_MENU(tmp));
		menuitem = gtk_menu_item_new_with_label("None");
		gtk_menu_append(GTK_MENU(menu), menuitem);
		gtk_widget_show(menuitem);
		for(x=0; x<ggzcore_server_get_num_gametypes(server); x++)
		{
			gt = ggzcore_server_get_nth_gametype(server, x);
			gtype[0] = g_strdup(ggzcore_gametype_get_name(gt));
			gtype[1] = g_strdup(ggzcore_gametype_get_author(gt));
			gtype[2] = g_strdup(ggzcore_gametype_get_url(gt));
			gtype[3] = g_strdup(ggzcore_gametype_get_desc(gt));
			tmp = lookup_widget(types_dialog, "types_clist");
			gtk_clist_append(GTK_CLIST(tmp), gtype);
			g_free(gtype[0]);
			g_free(gtype[1]);
			g_free(gtype[2]);
			g_free(gtype[3]);

			tmp = lookup_widget(types_dialog, "filter_optionmenu");
			menu = gtk_option_menu_get_menu(GTK_OPTION_MENU(tmp));
			menuitem = gtk_menu_item_new_with_label(ggzcore_gametype_get_name(gt));
			gtk_menu_append(GTK_MENU(menu), menuitem);
			gtk_widget_show(menuitem);
		}

		gtk_widget_show(types_dialog);
	}
	else {
		gdk_window_raise(types_dialog->window);
	}
}

#if 0 /* currently unused */
static gboolean types_clist_events(GtkWidget *widget, GdkEvent *event, gpointer data)
{

	return FALSE;
}


static void types_upgrade(GtkMenuItem *menuitem, gpointer data)
{

}


static void types_URL(GtkMenuItem *menuitem, gpointer data)
{

}
#endif


static void types_filter_button(GtkWidget *widget, gpointer data)
{
        msgbox("Room filtering is not implemented yet. If\nyou would like to help head over to\nhttp://ggz.sourceforge.net.", "Not Implemented",
                MSGBOX_OKONLY, MSGBOX_NONE, MSGBOX_NORMAL);

}






GtkWidget*
create_dlg_types (void)
{
  GtkWidget *dlg_types;
  GtkWidget *scrolledwindow1;
  GtkWidget *viewport1;
  GtkWidget *vbox1;
  GtkWidget *scrolledwindow2;
  GtkWidget *types_clist;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *label8;
  GtkWidget *label9;
  GtkWidget *toolbar1;
  GtkWidget *filter_label;
  GtkWidget *filter_optionmenu;
  GtkWidget *filter_optionmenu_menu;
  GtkWidget *filter_button;

  dlg_types = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_transient_for(GTK_WINDOW(dlg_types), GTK_WINDOW(win_main));
  g_object_set_data(G_OBJECT (dlg_types), "dlg_types", dlg_types);
  gtk_widget_set_size_request(dlg_types, 600, 300);
  gtk_window_set_title (GTK_WINDOW (dlg_types), _("Game Types"));

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow1);
  g_object_set_data_full(G_OBJECT (dlg_types), "scrolledwindow1", scrolledwindow1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow1);
  gtk_container_add (GTK_CONTAINER (dlg_types), scrolledwindow1);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  viewport1 = gtk_viewport_new (NULL, NULL);
  gtk_widget_ref (viewport1);
  g_object_set_data_full(G_OBJECT (dlg_types), "viewport1", viewport1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (viewport1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), viewport1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  g_object_set_data_full(G_OBJECT (dlg_types), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (viewport1), vbox1);

  scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow2);
  g_object_set_data_full(G_OBJECT (dlg_types), "scrolledwindow2", scrolledwindow2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow2);
  gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow2, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  types_clist = gtk_clist_new (4);
  gtk_widget_ref (types_clist);
  g_object_set_data_full(G_OBJECT (dlg_types), "types_clist", types_clist,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (types_clist);
  gtk_container_add (GTK_CONTAINER (scrolledwindow2), types_clist);
  gtk_clist_set_column_width (GTK_CLIST (types_clist), 0, 74);
  gtk_clist_set_column_width (GTK_CLIST (types_clist), 1, 88);
  gtk_clist_set_column_width (GTK_CLIST (types_clist), 2, 175);
  gtk_clist_set_column_width (GTK_CLIST (types_clist), 3, 80);
  gtk_clist_column_titles_show (GTK_CLIST (types_clist));

  label6 = gtk_label_new (_("Name"));
  gtk_widget_ref (label6);
  g_object_set_data_full(G_OBJECT (dlg_types), "label6", label6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label6);
  gtk_clist_set_column_widget (GTK_CLIST (types_clist), 0, label6);

  label7 = gtk_label_new (_("Author"));
  gtk_widget_ref (label7);
  g_object_set_data_full(G_OBJECT (dlg_types), "label7", label7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label7);
  gtk_clist_set_column_widget (GTK_CLIST (types_clist), 1, label7);

  label8 = gtk_label_new (_("Web Address"));
  gtk_widget_ref (label8);
  g_object_set_data_full(G_OBJECT (dlg_types), "label8", label8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label8);
  gtk_clist_set_column_widget (GTK_CLIST (types_clist), 2, label8);

  label9 = gtk_label_new (_("Description"));
  gtk_widget_ref (label9);
  g_object_set_data_full(G_OBJECT (dlg_types), "label9", label9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label9);
  gtk_clist_set_column_widget (GTK_CLIST (types_clist), 3, label9);

  toolbar1 = gtk_toolbar_new();
  gtk_widget_ref (toolbar1);
  g_object_set_data_full(G_OBJECT (dlg_types), "toolbar1", toolbar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar1);
  gtk_box_pack_start (GTK_BOX (vbox1), toolbar1, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (toolbar1), 4);

  filter_label = gtk_label_new (_("Room List Filter:"));
  gtk_widget_ref (filter_label);
  g_object_set_data_full(G_OBJECT (dlg_types), "filter_label", filter_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (filter_label);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar1), filter_label, NULL, NULL);

  filter_optionmenu = gtk_option_menu_new ();
  gtk_widget_ref (filter_optionmenu);
  g_object_set_data_full(G_OBJECT (dlg_types), "filter_optionmenu", filter_optionmenu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (filter_optionmenu);
  gtk_toolbar_append_space (GTK_TOOLBAR (toolbar1));

  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar1), filter_optionmenu, NULL, NULL);
  filter_optionmenu_menu = gtk_menu_new ();
  gtk_option_menu_set_menu (GTK_OPTION_MENU (filter_optionmenu), filter_optionmenu_menu);

  gtk_toolbar_append_space (GTK_TOOLBAR (toolbar1));

  filter_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Set"),
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (filter_button);
  g_object_set_data_full(G_OBJECT (dlg_types), "filter_button", filter_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (filter_button);

  g_signal_connect (GTK_OBJECT (dlg_types), "destroy",
                      GTK_SIGNAL_FUNC (gtk_widget_destroyed),
                      &types_dialog);
  g_signal_connect (GTK_OBJECT (filter_button), "clicked",
                      GTK_SIGNAL_FUNC (types_filter_button),
                      NULL);

  return dlg_types;
}

#if 0 /* currently unused */
GtkWidget*
create_menu1 (void)
{
  GtkWidget *menu1;
  GtkAccelGroup *menu1_accels;
  GtkWidget *upgrage_game;
  GtkWidget *goto_web_site;

  menu1 = gtk_menu_new ();
  g_object_set_data(G_OBJECT (menu1), "menu1", menu1);
  menu1_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (menu1));

  upgrage_game = gtk_menu_item_new_with_label (_("Upgrage Game"));
  gtk_widget_ref (upgrage_game);
  g_object_set_data_full(G_OBJECT (menu1), "upgrage_game", upgrage_game,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (upgrage_game);
  gtk_container_add (GTK_CONTAINER (menu1), upgrage_game);

  goto_web_site = gtk_menu_item_new_with_label (_("Goto Web Site"));
  gtk_widget_ref (goto_web_site);
  g_object_set_data_full(G_OBJECT (menu1), "goto_web_site", goto_web_site,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (goto_web_site);
  gtk_container_add (GTK_CONTAINER (menu1), goto_web_site);

  g_signal_connect (GTK_OBJECT (upgrage_game), "activate",
                      GTK_SIGNAL_FUNC (types_upgrade),
                      NULL);
  g_signal_connect (GTK_OBJECT (goto_web_site), "activate",
                      GTK_SIGNAL_FUNC (types_URL),
                      NULL);

  return menu1;
}
#endif
