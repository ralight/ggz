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

#ifndef _GGZ_LIST_REPLACE_DUPS	/* Make sure we have list datatypes */
#include "ggz.h"
#endif


/****
 **** Structure definitions for our two list element types
 ****/

typedef struct _ggz_confio_file {
	char		*path;
	int		handle;
	int		writeable;
	GGZList	        *section_list;
} _ggz_confio_file;

typedef struct _ggz_confio_section {
	char		*name;
	GGZList	        *entry_list;
} _ggz_confio_section;

typedef struct _ggz_confio_entry {
	char	*key;
	char	*value;
} _ggz_confio_entry;


/****
 **** Function prototypes
 ****/

int  _ggz_confio_parse(const char *path, const unsigned char options);
void _ggz_confio_cleanup		(void);

/* CONFIO Options */
#define GGZ_CONFIO_RDONLY	((unsigned char) 0x01)
#define GGZ_CONFIO_RDWR		((unsigned char) 0x02)
#define GGZ_CONFIO_CREATE	((unsigned char) 0x04)

/* CONFIO Functions */
int _ggz_confio_parse		(const char *path,
					 const unsigned char options);
void _ggz_confio_close		(int handle);
int _ggz_confio_commit		(int handle);
int _ggz_confio_write_string		(int	handle,
					 const char	*section,
					 const char	*key,
					 const char	*value);
int _ggz_confio_write_int		(int	handle,
					 const char	*section,
					 const char	*key,
					 int	value);
int _ggz_confio_write_list		(int	handle,
					 const char	*section,
					 const char	*key,
					 int	argc,
					 char	**argv);
char * _ggz_confio_read_string	(int	handle,
					 const char	*section,
					 const char	*key,
					 const char	*def);
int _ggz_confio_read_int		(int	handle,
					 const char	*section,
					 const char	*key,
					 int	def);
int _ggz_confio_read_list		(int	handle,
					 const char	*section,
					 const char	*key,
					 int	*argcp,
					 char	***argvp);
int _ggz_confio_remove_section	(int	handle,
					 const char	*section);
int _ggz_confio_remove_key		(int	handle,
					 const char	*section,
					 const char	*key);

