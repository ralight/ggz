/* 
 * File: dlg_bid.h
 * Author: Jason Short, Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Creates the bid request dialog
 * $Id: dlg_bid.h 4656 2002-09-23 00:48:07Z jdorje $
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

/* Display a dialog window for the user to make a bid choice. */
void dlg_bid_display(int possible_bids,
		     char **bid_texts, char **bid_descriptions);
void dlg_bid_destroy(void);
