
/*
 * File: gtk_dlg_options.h
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 5/11/99
 *
 * This fils contains functions for creating and handling the options
 * dialog box
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#include <gtk/gtk.h>

typedef struct {
	GtkWidget *offlineTog;
	GtkWidget *serverEntry;
	GtkWidget *portEntry;
	GtkWidget *tauntEntry;
	GtkWidget *playerTog1;
	GtkWidget *playerTog2;
	GtkWidget *playerTog3;
	GtkWidget *minBid;
	GtkWidget *endGame;
	GtkWidget *nilBids;
} input_t;


void OptionsDialog(GtkWidget *, gpointer);
void ConnectDialogReset(GtkWidget *);
void OfflineToggled(GtkWidget *, input_t *);
void PlayerToggled(GtkWidget *, gpointer);
void NilToggled(GtkWidget *, gpointer);
void SafeCloseDialog(void);
