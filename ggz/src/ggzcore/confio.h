/*
 * File: confio.h
 * Author: Rich Gade
 * Project: GGZ Core Client Lib
 * Date: 11/27/00
 *
 * Internal functions for handling configuration files
 *
 * Copyright (C) 2000, 2001 Brent Hendricks.
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


#include <ggz.h>


/****
 **** Structure definitions for our two list element types
 ****/

typedef struct _ggzcore_confio_file {
	char		*path;
	int		handle;
	int		writeable;
	GGZList	*section_list;
} _ggzcore_confio_file;

typedef struct _ggzcore_confio_section {
	char		*name;
	GGZList	*entry_list;
} _ggzcore_confio_section;

typedef struct _ggzcore_confio_entry {
	char	*key;
	char	*value;
} _ggzcore_confio_entry;


/****
 **** Function prototypes
 ****/

void _ggzcore_confio_cleanup		(void);
