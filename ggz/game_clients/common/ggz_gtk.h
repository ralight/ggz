/* 
 * File: ggz_gtk.h
 * Author: GGZ Development Team
 * Project: GGZ GTK games
 * Date: 11/11/2004
 * Desc: GGZ Handlers for GTK games
 * $Id: ggz_gtk.h 6333 2004-11-12 02:27:20Z jdorje $
 *
 * Copyright (C) 2004 GGZ Development Team
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

#include "dlg_about.h"
#include "dlg_chat.h"
#include "ggzintl.h"
#include "dlg_players.h"

GGZMod *init_ggz_gtk(gboolean(*game_handler) (GGZMod * mod));
