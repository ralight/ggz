/*
 * File: dlg_yesno.h
 * Author: Josef Spillner
 * Project: GGZ Hastings1066 Client
 * Date: 11/01/2002
 * Desc: Create a "Yes/No" Gtk dialog
 *
 * Copyright (C) 2000 - 2002 Josef Spillner
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

#ifndef HASTINGS_DLG_YESNO_H
#define HASTINGS_DLG_YESNO_H

/* Show a question to be answered with yes or no */
GtkWidget *create_dlg_yesno(const char *question);

#endif
