/*
 * File: animation.h
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 12/18/2001
 * Desc: Animation code for GTK table
 * $Id: animation.h 2939 2001-12-18 20:47:03Z jdorje $
 *
 * Copyright (C) 2001 GGZ Development Team.
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

#include "common.h"

/* if this is defined, the animation code will be used. I've disable it
   because I broke it... */
/* #define ANIMATION */


#ifdef ANIMATION

/* Are we currently in the process of animating? */
extern int animating;

void animation_start(int player, card_t card, int card_num);
gint animation_callback(gpointer ignored);
void animation_abort(void);
void animation_zip(gboolean restore);

#endif /* ANIMATION */
