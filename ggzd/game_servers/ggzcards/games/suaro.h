/* 
 * File: games/suaro.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/02/2001
 * Desc: Game-dependent game data for Suaro (for use by AI)
 * $Id: suaro.h 4040 2002-04-21 21:21:07Z jdorje $
 *
 * Copyright (C) 2001-2002 Brent Hendricks.
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

/* special bids */
enum {
	SUARO_BID,
	SUARO_PASS,
	SUARO_DOUBLE,
	SUARO_REDOUBLE,
	SUARO_KITTY
};

/* special suits */
#define SUARO_LOW 0
	/* clubs-spades = 1-4 = regular values + 1 */
#define SUARO_HIGH 5
