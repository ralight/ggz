/*
 * File: games/fortytwo.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 04/21/2002
 * Desc: Game-dependent game data for Forty-Two (for use by AI)
 * $Id: fortytwo.h 4069 2002-04-23 22:43:51Z jdorje $
 *
 * Copyright (C) 2001-2002 GGZ Development Team.
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

/* Special bid values for fortytwo (i.e. bid.sbid.spec) */
enum {
	/* A bid in the range 30-42; this value is stored in the val. */
	FORTYTWO_BID,

	/* A bid in the range 84/126/168/210; the value of the double
	   (i.e. 2 for "84") is stored in the val. */
	FORTYTWO_DOUBLE,

	/* A pass. */
	FORTYTWO_PASS,
	
	/* A bid to choose trump.  The suit indicates the suit (0-6). */
	FORTYTWO_TRUMP
};
