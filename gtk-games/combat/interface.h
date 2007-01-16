/*
 * File: interface.c
 * Author: GGZ Development Team
 * Project: GGZ Combat Client
 * Date: 2001?
 * Desc: Interface code
 * $Id: interface.h 8929 2007-01-16 02:18:40Z jdorje $
 *
 * Copyright (C) 2001-2004 GGZ Development Team
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

#define GAME_SAVE_MAP _("/Game/Save current map")
#define GAME_SHOW_OPTIONS _("/Game/Show game options")

GtkWidget *create_main_window(void);
