/*
 * File: client.h
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 7/13/99
 *
 * This file dfines options structures and masks to be used by both client 
 *  and server
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#ifndef _OPTIONS_H
#define _OPTIONS_H

typedef struct {
	int bitOpt;
	int endGame;
	int minBid;
} option_t;


#define MSK_OFFLINE  1	/* %0000 0001 */
#define MSK_COMP_1   2	/* %0000 0010 */
#define MSK_COMP_2   4	/* %0000 0100 */
#define MSK_COMP_3   8	/* %0000 1000 */
#define MSK_AI      14	/* %0000 1110 */
#define MSK_GAME    48	/* %0011 0000 */
#define GAME_SPADES  0	/* %xx00 xxxx */
#define GAME_HEARTS 16	/* %xx01 xxxx */
#define MSK_NILS    64	/* %0100 0000 */
#define MSK_BAGS   128	/* %1000 0000 */

#endif
