/*
 * File: map.h
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Date: 09/17/2000
 * Desc: Game header
 * $Id: map.h 8953 2007-01-17 00:39:28Z jdorje $
 *
 * Copyright (C) 2000 Ismael Orenstein.
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

void map_load(combat_game *, char *name, gboolean *changed);

int map_save(combat_game * map);

int map_search(combat_game * game);

char **map_list(void);
