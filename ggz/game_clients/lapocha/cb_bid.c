/*
 * File: cb_bid.c
 * Author: Rich Gade
 * Project: GGZ La Pocha Client
 * Date: 08/14/2000
 * Desc: Callbacks for bid request Gtk fixed dialog
 *
 * Copyright (C) 2000 Brent Hendricks.
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

#include <gtk/gtk.h>

#include "cb_bid.h"
#include "dlg_bid.h"
#include "support.h"
#include "game.h"


void
on_dlg_bid_submit_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	int bid;

	bid = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(dlg_bid_spin));
	game_send_bid(bid);

	gtk_widget_hide(dlg_bid_fixed);
}

