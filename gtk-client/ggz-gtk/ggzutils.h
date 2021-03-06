/*
 * File: ggzutils.c
 * Author: GGZ Dev Team
 * Project: GGZ GTK Client
 * Date: 12/28/2006
 * $Id: playerlist.c 8763 2006-12-27 10:02:33Z jdorje $
 * 
 * GGZ-specific utility functions
 * 
 * Copyright (C) 2006 GGZ Development Team
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

#include <ggz_common.h>

#include "support.h"
#include "xtext.h"

#ifndef __GGZUTILS_H__
#define __GGZUTILS_H__

const char INTERNAL *perm_name(GGZPerm p);

GtkWidget INTERNAL *create_xtext(void);

/* xtext color palette */
void INTERNAL colors_init(void);
void INTERNAL colors_activate(GtkXText *xtext);
GdkColor INTERNAL color_white(void);
GdkColor INTERNAL color_black(void);
GdkColor INTERNAL color_get(int id);
void INTERNAL color_set(int id, GdkColor color);

#endif /* __GGZUTILS_H__ */
