/*
 * File: drawcard.h
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 04/20/2002
 * Desc: Routines to display cards
 * $Id: drawcard.h 4027 2002-04-21 01:36:44Z jdorje $
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

void load_card_data(void);
void get_card_coordinates(card_t card, int orientation, int *x, int *y);
void draw_card(card_t card, int orientation, int x, int y, GdkPixmap * image);
void get_card_size(int orientation, int *w, int *h);

/* The following will eventually be allowed to change. */
#define CARDWIDTH	71
#define CARDHEIGHT	96

/* This is the minimum amount of the card that must be visible. */
#define CARD_VISIBILITY ((float)CARDWIDTH / 4.0)

#endif /* __DRAWCARD_H__ */
