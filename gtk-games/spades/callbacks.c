/*
 * File: callbacks.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * $Id: callbacks.c 7637 2005-11-19 02:53:43Z jdorje $
 *
 * This file contains the functions which operate on the type card. See the
 * header file card.h for a better description of these functions.
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
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <ggz.h>

#include <client.h>
#include <gtk_connect.h>
#include <gtk_dlg_options.h>
#include <gtk_dlg_error.h>
#include "gtk_play.h"
#include <options.h>
#include <display.h>
#include <callbacks.h>


GtkWidget *detail_window = NULL;
extern gameState_t gameState;
extern option_t options;


void launch_game(GtkButton * button, gpointer user_data)
{
	input_t *in = (input_t *) user_data;

	options.endGame =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(in->endGame));
	options.minBid =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(in->minBid));

	CheckWriteInt(gameState.spadesSock, sizeof(options));
	write(gameState.spadesSock, &options, sizeof(options));
	ggz_debug("main", "Sent options\n");

	/*DisplayInit(); */
}

void ExitDialog(GtkWidget * widget, gpointer user_data)
{
	ggz_show_exit_dialog(FALSE);
}
