/* 
 * File: dlg_options.h
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 12/09/2001
 * Desc: Creates the option request dialog
 * $Id: dlg_options.h 8259 2006-06-23 06:53:15Z jdorje $
 *
 * Copyright (C) 2001-2002 GGZ Dev Team.
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

/* Display a dialog window for the user to make an options selection. */
void dlg_option_display(int option_cnt,
			char **types, char **descriptions,
			int *option_sizes, int *defaults, char ***options);
void dlg_options_destroy(void);
