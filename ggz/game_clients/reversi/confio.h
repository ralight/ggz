/*
 * File: confio.h
 * Author: Rich Gade
 * Project: GGZ Core Client Lib
 * Date: 11/27/00
 *
 * Internal functions for handling configuration files
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

#ifndef _GGZCORE_LIST_REPLACE_DUPS	/* Make sure we have list datatypes */
#include "lists.h"
#endif


/****
 **** Structure definitions for our two list element types
 ****/

typedef struct _ggzcore_confio_file {
	char		*path;
	int		handle;
	_ggzcore_list	*section_list;
} _ggzcore_confio_file;

typedef struct _ggzcore_confio_section {
	char		*name;
	_ggzcore_list	*entry_list;
} _ggzcore_confio_section;

typedef struct _ggzcore_confio_entry {
	char	*key;
	char	*value;
} _ggzcore_confio_entry;


/****
 **** Function prototypes
 ****/

int _ggzcore_confio_parse		(char *path);

int _ggzcore_confio_commit		(int handle);

void _ggzcore_confio_cleanup		(void);

int _ggzcore_confio_write_string	(int	handle,
					 char	*section,
					 char	*key,
					 char	*value);

int _ggzcore_confio_write_int		(int	handle,
					 char	*section,
					 char	*key,
					 int	value);

int _ggzcore_confio_write_list		(int	handle,
					 char	*section,
					 char	*key,
					 int	argc,
					 char	**argv);

char * _ggzcore_confio_read_string	(int	handle,
					 char	*section,
					 char	*key,
					 char	*def);

int _ggzcore_confio_read_int		(int	handle,
					 char	*section,
					 char	*key,
					 int	def);

int _ggzcore_confio_read_list		(int	handle,
					 char	*section,
					 char	*key,
					 int	*argcp,
					 char	***argvp);

int _ggzcore_confio_remove_section	(int	handle,
					 char	*section);

int _ggzcore_confio_remove_key		(int	handle,
					 char	*section,
					 char	*key);
