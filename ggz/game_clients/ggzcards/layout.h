/* $Id: layout.h 2860 2001-12-10 17:17:29Z jdorje $ */
/* 
 * File: layout.h
 * Author: Jason Short, Rich Gade
 * Project: GGZCards Client
 * Date: 06/21/2001
 * Desc: Routines to get the layout for the game table
 *
 * Copyright (C) 2000 Brent Hendricks.
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

/* no more than 6 players will work */
#define MAX_NUM_PLAYERS 6

/* The following would be hell to change */
#define CARDWIDTH	71
#define CARDHEIGHT	96

/* an interesting check of the code is to make this really big */
#define XWIDTH		10

/* #define WINDOW_WIDTH 469 */
/* more cards will fit if you just change table_max_hand_size */
#define HAND_WIDTH 	( (table_max_hand_size+3) * (CARDWIDTH/4.0) + 0.5 )
#define TEXT_WIDTH	CARDHEIGHT

#define CARD_BOX_WIDTH	(HAND_WIDTH + 2*XWIDTH)
#define TEXT_BOX_WIDTH	(TEXT_WIDTH + 2*XWIDTH)

int get_table_width();
int get_table_height();

int orientation(int p);

void get_tablecard_pos(int p, int *x, int *y);
void get_table_dim(int *x, int *y, int *w, int *h);
void get_fulltable_size(int *w, int *h);

void get_text_box_pos(int p, int *x, int *y);
void get_card_box_pos(int p, int *x, int *y);
void get_card_box_dim(int p, int *w, int *h);
void get_full_card_area(int p, int *x, int *y,	/* the (x, y) position of the 
						   upper left corner */
			int *w, int *h,	/* the width and height of the box */
			int *xo, int *yo	/* the x and y offsets for
						   "selected" cards */ );
void get_inner_card_area_pos(int p, int *x, int *y);

void get_card_offset(int p, float *w, float *h);
void get_card_size(int orientation, int *w, int *h);
