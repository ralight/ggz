/* 
 * File: layout.h
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 06/21/2001
 * Desc: Routines to get the layout for the game table
 * $Id: layout.h 6293 2004-11-07 05:51:47Z jdorje $
 *
 * Copyright (C) 2000-2002 Brent Hendricks.
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

#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#include "shared.h"

typedef enum {
	FACE_BOTTOM,
	FACE_LEFT,
	FACE_TOP,
	FACE_RIGHT
} orientation_t;

#include "drawcard.h"	/* FIXME */

/* no more than 6 players will work */
#define MAX_NUM_PLAYERS 6

/* Don't try to change this...yet! */
#define NUM_CARD_ROWS get_num_card_rows()

/* an interesting check of the code is to make this really big */
#define XWIDTH		CARD_OFFSET

/* #define WINDOW_WIDTH 469 */
/* more cards will fit if you just change table_max_hand_size */
#define HAND_WIDTH get_hand_width()
#define TEXT_WIDTH get_text_width()

#define CARD_BOX_WIDTH	(HAND_WIDTH + 2 * XWIDTH)
#define TEXT_BOX_WIDTH	(TEXT_WIDTH + 2 * XWIDTH)

int get_num_card_rows(void);

int get_min_text_width(void);
bool set_min_text_width(int min_text_width);

int get_table_width(void);
int get_table_height(void);

int get_hand_width(void);
int get_text_width(void);

int get_max_hand_size(void);
void set_max_hand_size(int max_hand_size);

orientation_t orientation(int p);

void get_tablecard_pos(int p, int *x, int *y);
void get_table_dim(int *x, int *y, int *w, int *h);
void get_fulltable_dim(int *x, int *y, int *w, int *h);

void get_text_box_pos(int p, int *x, int *y);
void get_card_box_pos(int p, int *x, int *y);
void get_card_box_dim(int p, int *w, int *h);
void get_full_card_area(int p, int *x, int *y, int *w, int *h);
void get_inner_card_area_pos(int p, int *x, int *y);

void get_card_pos(int p, int card_num, bool selected, int *x, int *y);

#endif /* __LAYOUT_H__ */
