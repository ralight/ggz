/*
 * File: menus.c
 * Author: GGZ Development Team
 * Project: GGZ GTK Games
 * Date: 10/25/2002
 * Desc: Main window menus
 * $Id: menus.c 5043 2002-10-26 04:43:10Z jdorje $
 *
 * The point of this file is to help games to achieve a consistent look
 * and feel in their menus.  The TABLE_MENU and HELP_MENU should be used
 * by all games.  A "Game" menu should provide game-specific featurs.  An
 * "Options" menu should be used if necessary for options.
 *
 * Copyright (C) 2002 GGZ Development Team.
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
#  include <config.h>
#endif

#include <assert.h>

#include "menus.h"

static GtkItemFactory *menu = NULL;

GtkWidget *ggz_create_menus(GtkWidget *window,
			    GtkItemFactoryEntry *items,
			    const unsigned int num_items)
{
	GtkAccelGroup *accel_group = gtk_accel_group_new();

	assert(menu == NULL);

	menu = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>", accel_group);
	gtk_item_factory_create_items(menu, num_items, items, NULL);
	gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

	gtk_object_set_data(GTK_OBJECT(window), "mbar", menu);

	return gtk_item_factory_get_widget(menu, "<main>");
}

GtkWidget *get_menu_item(const char *item)
{
	return gtk_item_factory_get_widget(menu, item);
}

void set_menu_sensitive(const char *item, gboolean sensitive)
{
	GtkWidget *menu_item = get_menu_item(item);
	gtk_widget_set_sensitive(menu_item, sensitive);
}
