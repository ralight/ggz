/* 
 * File: drawcard.h
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 04/20/2002
 * Desc: Routines to display cards
 * $Id: drawcard.h 4180 2002-05-07 09:44:19Z jdorje $
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

#ifndef __DRAWCARD_H__
#define __DRAWCARD_H__

#include <gtk/gtk.h>

#include "layout.h"

/* This value is used as the vertical offset for cards. */
#define CARD_OFFSET 10

#define CARDWIDTH (get_card_width(0))
#define CARDHEIGHT (get_card_height(0))
#define CARD_VISIBILITY (get_card_visibility_width())

void load_card_data(cardset_type_t cardset_type);
void draw_card(card_t card, orientation_t orientation, int x, int y,
	       GdkPixmap * image);

int get_card_width(orientation_t orientation);
int get_card_height(orientation_t orientation);

/* This is the minimum amount of the card that must be visible. */
float get_card_visibility_width(void);
float get_card_visibility_height(void);

#endif /* __DRAWCARD_H__ */
