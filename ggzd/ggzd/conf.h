/*
 * File: conf.h
 * Author: Rich Gade
 * Project: GGZ Core Client Lib
 *	    Modified from confio for use by server (rgade - 08/06/01)
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

#ifndef LIST_REPLACE_DUPS	/* Make sure we have list datatypes */
#include "lists.h"
#endif


/****
 **** Structure definitions for our two list element types
 ****/

typedef struct conf_file_t {
	char		*path;
	int		handle;
	int		writeable;
	list_t		*section_list;
} conf_file_t;

typedef struct conf_section_t {
	char		*name;
	list_t		*entry_list;
} conf_section_t;

typedef struct conf_entry_t {
	char	*key;
	char	*value;
} conf_entry_t;

#define CONF_RDONLY	((unsigned char) 0x01)
#define CONF_RDWR	((unsigned char) 0x02)
#define CONF_CREATE	((unsigned char) 0x04)


/****
 **** Function prototypes
 ****/

void conf_cleanup (void);

int conf_parse		(const char *path,
			 const unsigned char options);
int conf_commit		(int handle);
int conf_write_string	(int	handle,
			 const char	*section,
			 const char	*key,
			 const char	*value);
int conf_write_int	(int	handle,
			 const char	*section,
			 const char	*key,
			 int	value);
int conf_write_list	(int	handle,
			 const char	*section,
			 const char	*key,
			 int	argc,
			 char	**argv);
char * conf_read_string	(int	handle,
			 const char	*section,
			 const char	*key,
			 const char	*def);
int conf_read_int	(int	handle,
			 const char	*section,
			 const char	*key,
			 int	def);
int conf_read_list	(int	handle,
			 const char	*section,
			 const char	*key,
			 int	*argcp,
			 char	***argvp);
int conf_remove_section	(int	handle,
			 const char	*section);
int conf_remove_key	(int	handle,
			 const char	*section,
			 const char	*key);
