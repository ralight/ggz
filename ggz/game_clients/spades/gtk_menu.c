/*
 * File: gtk_menu.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 1/23/99
 * $Id: gtk_menu.c 4892 2002-10-12 20:55:46Z jdorje $
 *
 * This file contains functions for creating and handling menus
 *
 * Copyright (C) 1998-2002 Brent Hendricks.
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

#include <gtk/gtk.h>

#include "dlg_about.h"
#include "dlg_exit.h"
#include "ggzintl.h"

#include <gtk_connect.h>
#include <gtk_io.h>
#include <gtk_play.h>
#include <gtk_menu.h>
#include <gtk_dlg_options.h>
#include <gtk_dlg_disconnect.h>
#include <gtk_dlg_stat.h>
#include <client.h>
#include <display.h>


/* Global widget structure */
extern playArea_t *playArea;

static void AboutDialog(GtkWidget *widget, gpointer data)
{
	create_or_raise_dlg_about();
}

GtkWidget *CreateMenus(GtkWidget * window)
{

	GtkAccelGroup *accelGroup;
	GtkWidget *menuBar, *menuItem;
	GtkItemFactoryEntry menuItems[] = {
	  {_("/_Game"), NULL, NULL, 0, "<Branch>"},
	  {_("/Game/_New"), NULL, OptionsDialog, 0, NULL},
	  {_("/Game/_End"), NULL, DisconnectDialog, 0, NULL},
	  {_("/Game/_Statistics"), NULL, StatDialog, 0, NULL},
	  {_("/Game/sep1"), NULL, NULL, 0, "<Separator>"},
	  {_("/Game/_Quit"), "<alt>F4", ExitDialog, 0, NULL},
	  {_("/_Help"), NULL, NULL, 0, "<LastBranch>"},
#if 0
	  {_("/Help/_GamePlay"), NULL, HelpDialog, 0, NULL},
#endif
	  {_("/Help/_About"), "<ctrl>A", AboutDialog, 0, NULL},
	};



	int nItems = sizeof(menuItems) / sizeof(menuItems[0]);

	accelGroup = gtk_accel_group_new();
	playArea->menu =
	    gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>", accelGroup);
	gtk_item_factory_create_items(playArea->menu, nItems, menuItems,
				      NULL);
	gtk_accel_group_attach(accelGroup, GTK_OBJECT(window));
	menuBar = gtk_item_factory_get_widget(playArea->menu, "<main>");

	menuItem =
	    gtk_item_factory_get_widget(playArea->menu, _("<main>/Game/End"));
	gtk_widget_set_sensitive(menuItem, FALSE);

	return menuBar;

}
