/*
 * File: confio.c
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

#include <config.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//#include "../../src/ggzcore/ggzcore.h"
#include "lists.h"
#include "confio.h"


/* The majority of this code deals with maintaining a set of lists to
 * cache all the configuration variables in memory until they need to
 * be committed.  There are three types of lists:
 *
 * 1) A list of configuration files containing
 *	a) the path to the configuration file on disk
 *	b) an integer handle which the caller uses to reference the file
 *	c) a pointer to a list of configuration file sections
 * 2) Section lists, which contain
 *	a) the section name
 *	b) a pointer to a list of key/value pair entries
 * 3) Entry lists, which contain
 *	a) the key (variable name)
 *	b) the value (variable data)
 */

/* Our private functions and vars */
static _ggzcore_list * file_parser(char *path);
static void parse_line(char *line, char **varname, char **varvalue);
static int section_compare(void *a, void *b);
static void *section_create(void *data);
static void section_destroy(void *data);
static int entry_compare(void *a, void *b);
static void *entry_create(void *data);
static void entry_destroy(void *data);
static char *dup_string(char *src);
static _ggzcore_confio_file * get_file_data(int handle);

static _ggzcore_list	*file_list=NULL;


/* _ggzcore_confio_read_string()
 *	Search the lists using the specified handle to find the string
 *	stored under section/key.
 *
 *	Returns:
 *	  - ptr to a malloc()'ed copy of value
 *	  - ptr to a malloc()'ed copy of default if not found
 *	  - ptr to NULL if not found and no default
 */
char * _ggzcore_confio_read_string(int handle, char *section,
				   char *key, char *def)
{
	_ggzcore_confio_file	*f_data;
	_ggzcore_list_entry	*s_entry, *e_entry;
	_ggzcore_confio_section	*s_data;
	_ggzcore_confio_entry	e_srch, *e_data;

	/* Find this file entry in our file list */
	if((f_data = get_file_data(handle)) == NULL)
		goto do_default;

	/* Find the requested [Section] */
	s_entry = _ggzcore_list_search(f_data->section_list, section);
	if(s_entry == NULL)
		goto do_default;
	s_data = _ggzcore_list_get_data(s_entry);

	/* Locate the requested Key */
	e_srch.key = key;
	e_entry = _ggzcore_list_search(s_data->entry_list, &e_srch);
	if(e_entry == NULL)
		goto do_default;
	e_data = _ggzcore_list_get_data(e_entry);

	/* Duplicate the resulting value and return it to the caller */
	return dup_string(e_data->value);

do_default:
	/* Any failure causes a branch to here to return */
	/* a defualt value if provided by the caller.	 */
	if(def != NULL)
		return dup_string(def);
	else
		return NULL;
}


/* _ggzcore_confio_read_int()
 *	This is simply a wrapper around read_string() to convert the
 *	string into an integer.
 *
 *	Returns:
 *	  - int value from variable, or def if not found
 */
int _ggzcore_confio_read_int(int handle, char *section, char *key, int def)
{
	char	tmp[20], *tmp2;
	int	value;

	sprintf(tmp, "%d", def);
	tmp2 = _ggzcore_confio_read_string(handle, section, key, tmp);
	value = atoi(tmp2);
	free(tmp2);

	return value;
}


/* _ggzcore_confio_read_list()
 *	This is simply a wrapper around read_string() to convert the
 *	string into a list.
 *
 *	Returns:
 *	  - an array and count of entries via the arglist
 *	AND
 *	  - 0 on success
 *	  - -1 on error
 */
int _ggzcore_confio_read_list(int handle, char *section, char *key,
			      int *argcp, char ***argvp)
{
	int	index, rc;
	char	*p, *s1, *s2;
	char	*str, *tmp, *tmp2;
	char	saw_space=0, saw_backspace;

	str = _ggzcore_confio_read_string(handle, section, key, NULL);

	if (str != NULL) {
		rc = 0;
		for (*argcp = 1, p = str; *p != '\0'; p++) {
			if (*p == '\\' && *(p+1)) {
				p++;
				if (saw_space) {
					*argcp += 1;
					saw_space = 0;
				}
			} else if (*p == ' ') {
				saw_space = 1;
			} else if (saw_space) {
				*argcp += 1;
				saw_space = 0;
			}
		}

		*argvp = (char **) malloc((*argcp + 1) * sizeof(char *));

		p = str;
		index = 0;
		do {
			tmp = p;

			for (saw_backspace = 0;
			     *p != '\0' && (saw_backspace ? 1 : (*p != ' '));
			     p++) {
				if (*p == '\\')
					saw_backspace = 1;
				else
					saw_backspace = 0;
			}

			if((tmp2 = malloc(p-tmp+1)) == NULL)
				printf("malloc failed in _ggzcore_confio_read_list");
			(*argvp)[index] = strncpy(tmp2, tmp, p - tmp);
			tmp2[p-tmp] = '\0';
			s1 = s2 = (*argvp)[index++];

			while (*s1) {
				if (*s1 == '\\')
					s1++;
				if (!*s1) break;
				*s2++ = *s1++;
			}
			*s2 = '\0';

			while (*p && *p == ' ')
				p++;
		} while (*p);

		free(str);
	} else {
		rc = -1;
		*argcp = 0;
	}

	return rc;
}


/* _ggzcore_confio_write_string()
 *	Store the value specified into the list structures for the config
 *	file referred to by handle.  Will create a new section and/or
 *	key entry if needed.
 *
 *	Returns:
 *	  - 0 on no error
 *	  - -1 on error
 */
int _ggzcore_confio_write_string(int handle, char *section,
				    char *key, char *value)
{
	_ggzcore_confio_file	*f_data;
	_ggzcore_list_entry	*s_entry;
	_ggzcore_confio_section	*s_data;
	_ggzcore_confio_entry	e_data;

	/* Find this file entry in our file list */
	if((f_data = get_file_data(handle)) == NULL)
		return -1;

	/* Find the requested [Section] */
	s_entry = _ggzcore_list_search(f_data->section_list, section);
	if(s_entry == NULL) {
		/* We need to create a new [Section] */
		if(_ggzcore_list_insert(f_data->section_list, section) < 0) {
			printf("_ggzcore_confio_write_string: insertion error");
			return -1;
		}
		s_entry = _ggzcore_list_search(f_data->section_list, section);
	}
	s_data = _ggzcore_list_get_data(s_entry);

	/* Insert the new value into the [Section]'s list */
	e_data.key = key;
	e_data.value = value;
	if(_ggzcore_list_insert(s_data->entry_list, &e_data) < 0) {
		printf("_ggzcore_confio_write_string: insertion error");
		return -1;
	}

	return 0;
}


/* _ggzcore_confio_write_int()
 *	This is simply a wrapper around write_string() to convert the
 *	string into an integer.
 *
 *	Returns:
 *	  - 0 on success
 *	  - -1 on failure
 */
int _ggzcore_confio_write_int(int handle, char *section, char *key, int value)
{
	char	tmp[20];

	sprintf(tmp, "%d", value);
	return _ggzcore_confio_write_string(handle, section, key, tmp);
}


/* _ggzcore_confio_write_list()
 *	Converts an array of list entries into a text list that read_list()
 *	can parse.  The text is then written using write_string().
 *
 *	Note that the length of the list cannot exceed 1023 characters.  This
 *	limit is set as the maximum line length readable from a config file
 *	is 1024 characters including a carriage return that we don't represent
 *	here.
 *
 *	Returns:
 *	  - 0 on success
 *	  - -1 on failure
 */
int _ggzcore_confio_write_list(int handle, char *section, char *key,
			       int argc, char **argv)
{
	int	i;
	char	buf[1023];
	char	*src, *dst, *eob;

	dst = buf;
	eob = buf+1023;
	for(i=0; i<argc; i++) {
		src = argv[i];
		while(*src != '\0') {
			if(*src == ' ') {
				if(dst >= eob)
					return -1;
				*dst++ = '\\';
			}
			if(dst >= eob)
				return -1;
			*dst++ = *src;
		}
		if(dst >= eob)
			return -1;
		*dst++ = ' ';
	}

	dst--;
	*dst = '\0';

	return _ggzcore_confio_write_string(handle, section, key, buf);
}


/* _ggzcore_confio_remove_section()
 *	Removes the specified section from the config file pointed to by
 *	handle.
 *
 *	Returns:
 *	  - 0 on success
 *	  - 1 if [Section] did not exist (soft error)
 *	  - -1 on failure
 */
int _ggzcore_confio_remove_section(int handle, char *section)
{
	_ggzcore_confio_file	*f_data;
	_ggzcore_list_entry	*s_entry;

	/* Find this file entry in our file list */
	if((f_data = get_file_data(handle)) == NULL)
		return -1;

	/* Find the requested [Section] */
	s_entry = _ggzcore_list_search(f_data->section_list, section);
	if(s_entry == NULL)
		return 1;

	/* Since the entry list will automatically be destroyed, all */
	/* we need to do is remove this section entry */
	_ggzcore_list_delete_entry(f_data->section_list, s_entry);

	return 0;
}


/* _ggzcore_confio_remove_key()
 *	Removes the specified key from the config file pointed to by
 *	handle and section.
 *
 *	Returns:
 *	  - 0 on success
 *	  - 1 if [Section] or Key did not exist (soft error)
 *	  - -1 on failure
 */
int _ggzcore_confio_remove_key(int handle, char *section, char *key)
{
	_ggzcore_confio_file	*f_data;
	_ggzcore_list_entry	*s_entry, *e_entry;
	_ggzcore_confio_section	*s_data;
	_ggzcore_confio_entry	e_data;

	/* Find this file entry in our file list */
	if((f_data = get_file_data(handle)) == NULL)
		return -1;

	/* Find the requested [Section] */
	s_entry = _ggzcore_list_search(f_data->section_list, section);
	if(s_entry == NULL)
		return 1;
	s_data = _ggzcore_list_get_data(s_entry);

	/* Find the requested Key */
	e_data.key = key;
	e_entry = _ggzcore_list_search(s_data->entry_list, &e_data);
	if(e_entry == NULL)
		return 1;

	/* Remove the key's list entry */
	_ggzcore_list_delete_entry(s_data->entry_list, e_entry);

	return 0;
}


/* _ggzcore_confio_commit()
 *	This commits any changes that have been made to internal variables
 *	into the on-disk config file.  This should be done as frequently
 *	as the user feels necessary.
 *
 *	Returns:
 *	  - 0 on success
 *	  - -1 on failure
 */
int _ggzcore_confio_commit(int handle)
{
	_ggzcore_confio_file	*f_data;
	_ggzcore_list_entry	*s_entry, *e_entry;
	_ggzcore_confio_section	*s_data;
	_ggzcore_confio_entry	*e_data;
	FILE			*c_file;
	int			firstline=1;

	/* Find this file entry in our file list */
	if((f_data = get_file_data(handle)) == NULL)
		return -1;

	/* Open our configuration file for writing */
	if((c_file = fopen(f_data->path, "w")) == NULL) {
		printf("Unable to write config file %s",
			      f_data->path);
		return -1;
	}

	/* Now step through each section, writing each entry from it's list */
	s_entry = _ggzcore_list_head(f_data->section_list);
	while(s_entry) {
		s_data = _ggzcore_list_get_data(s_entry);

		/* Output a line for our [SectionID] */
		if(firstline) {
			fprintf(c_file, "[%s]\n", s_data->name);
			firstline = 0;
		} else
			fprintf(c_file, "\n[%s]\n", s_data->name);

		/* Bounce through the key entry list */
		e_entry = _ggzcore_list_head(s_data->entry_list);
		while(e_entry) {
			e_data = _ggzcore_list_get_data(e_entry);

			/* Output a line for our Key = Value */
			fprintf(c_file, "%s = %s\n", e_data->key,
						     e_data->value);

			e_entry = _ggzcore_list_next(e_entry);
		}

		s_entry = _ggzcore_list_next(s_entry);
	}

	fclose(c_file);

	return 0;
}


/* _ggzcore_confio_cleanup()
 *	This destroys all lists for configuration files including the main
 *	list.  Note that new configuration files may be opened after this
 *	function is called, as confio will reinitialize itself automatically.
 *
 *	WARNING - It might not be immediately recognized, but this function
 *	does NOT commit any changes before freeing up all allocated memory.
 *	The caller is assumed to have committed the config files before using
 *	this routine to clear out old files.
 */
void _ggzcore_confio_cleanup(void)
{
	_ggzcore_list_entry	*f_entry;
	_ggzcore_confio_file	*f_data;

	f_entry = _ggzcore_list_head(file_list);
	while(f_entry) {
		f_data = _ggzcore_list_get_data(f_entry);
		_ggzcore_list_destroy(f_data->section_list);
		free(f_data->path);
		free(f_data);
		f_entry = _ggzcore_list_next(f_entry);
	}

	_ggzcore_list_destroy(file_list);
	file_list = NULL;
}


/* _ggzcore_confio_parse(path)
 *	Load up and parse a configuration file into a set of linked lists.
 *
 *	Returns:
 *	  - an integer handle which the caller can use to access the variables
 *	  - -1 on failure
 */
int	_ggzcore_confio_parse(char *path)
{
	static int		next_handle=0;

	_ggzcore_confio_file	*file_data;
	_ggzcore_list		*section_list;


	/* Our first time through we need to intialize the file list */
	if(!file_list)
		file_list = _ggzcore_list_create(NULL, NULL, NULL,
						 _GGZCORE_LIST_ALLOW_DUPS);

	/* Go do the dirty work and give us a section_list */
	section_list = file_parser(path);

	/* Build our file list data entry */
	file_data = malloc(sizeof(_ggzcore_confio_file));
	if(!file_data)
		printf("malloc failed: _ggzcore_confio_parse");
	file_data->path = dup_string(path);
	file_data->handle = next_handle;
	file_data->section_list = section_list;

	/* Now store the new list entry */
	if(_ggzcore_list_insert(file_list, file_data) < 0) {
		/* ACK - PUKE */
		_ggzcore_list_destroy(section_list);
		return -1;
	}

	return next_handle++;
}


/* file_parser()
 *	Opens the file 'path', builds a section list and associated
 *	entry lists for all parsed variable entries, then closes the
 *	file.
 *
 *	Returns:
 *	  - ptr to a section list
 *	  - NULL on failure
 */
static _ggzcore_list * file_parser(char *path)
{
	FILE			*c_file;
	char			line[1024];
	char			*varname, *varvalue;
	int			linenum = 0;
	_ggzcore_list		*s_list;
	_ggzcore_list_entry	*s_entry;
	_ggzcore_confio_section	*s_data=NULL;
	_ggzcore_confio_entry	*e_data=NULL;


	/* Create the section list */
	s_list = _ggzcore_list_create(section_compare,
				      section_create,
				      section_destroy,
				      _GGZCORE_LIST_REPLACE_DUPS);
	if(!s_list)
		return NULL;

	/* Open the input config file */
	if((c_file = fopen(path, "r")) == NULL) {
		printf("Unable to read file %s", path);
		return NULL;
	}

	/* Setup some temp storage to use */
	if((e_data = malloc(sizeof(_ggzcore_confio_entry))) == NULL)
		printf("malloc failed: file_parser");

	/* Read individual lines and pass them off to be parsed */
	while(fgets(line, 1024, c_file)) {
		linenum++;
		parse_line(line, &varname, &varvalue);
		if(varname == NULL)
			continue;	/* Blank line or comment */
		if(varvalue == NULL) {
			/* Might be a [SectionID] */
			if(varname[0] == '['
			   && varname[strlen(varname)-1] == ']') {
				/* It is, so setup a new section entry */
				varname[strlen(varname)-1] = '\0';
				varname++;
				if(_ggzcore_list_insert(s_list, varname) < 0)
					printf("list insert error: file_parser");
				s_entry = _ggzcore_list_search(s_list, varname);
				s_data = _ggzcore_list_get_data(s_entry);
			} else
				printf("Syntax error, %s (line %d)",
						  path, linenum);
			continue;
		}

		/* We have a valid varname/varvalue, add them to entry list */
		if(!s_data) {
			/* We haven't seen a [SectionID] yet :( */
			printf("Syntax error, %s (line %d)",
					  path, linenum);
			continue;
		}
		e_data->key = varname;
		e_data->value = varvalue;
		if(_ggzcore_list_insert(s_data->entry_list, e_data) < 0)
			printf("list insert error: file_parser");
	}

	/* Cleanup after ourselves */
	free(e_data);
	fclose(c_file);

	return s_list;
}


/* parse_line()
 *	Parses a single line of input into a left half and right half
 *	separated by an optional equals sign.  Pointers to the lhs (varname)
 *	and rhs (varvalue) are returned via the argument list.
 */
static void parse_line(char *p, char **varname, char **varvalue)
{
	char csave, *psave, *sol;

	/* Save start-of-line in sol */
	sol = p;

	*varname = NULL;
	/* Skip over whitespace */
	while((*p == ' ' || *p == '\t' || *p == '\n') && *p != '\0')
		p++;
	if(*p == '\0' || *p == '#')
		return;		/* The line is a comment */

	*varname = p;

	*varvalue = NULL;
	/* Skip until we find an equals sign */
	while(*p != '=' && *p != '\n' && *p != '\0')
		p++;
	csave = *p;
	psave = p;

	if(*p == '=') {
		/* Found '=', now backspace to remove trailing space */
		do {
			p--;
		} while(p >= sol && (*p == ' ' || *p == '\t' || *p == '\n'));
		p++;
	}
	*p = '\0';
	p = psave;
	p++;

	if(**varname == '\0' || p == sol) {
		*varname = NULL;
		return;
	}

	if(csave == '\n' || csave == '\0')
		return;		/* There is no argument */

	/* There appears to be an argument, skip to the start of it */
	while((*p == ' ' || *p == '\t' || *p == '\n' || *p == '=')&& *p != '\0')
		p++;
	if(*p == '\0')
		return;		/* Argument is missing */

	/* There definitely is an argument */
	*varvalue = p;

	/* Terminate it ... */
	while(*p != '\n' && *p != '\0')
		p++;
	/* Found EOL, now backspace over whitespace to remove trailing space */
	p--;
	while(*p == ' ' || *p == '\t' || *p == '\n')
		p--;
	p++;
	/* Finally terminate it with a NUL */
	*p = '\0';	/* Might have already been the NUL, but who cares? */
}


/* dup_string()
 *	A convenience function to malloc space for and duplicate a string.
 *
 *	Returns:
 *	  - ptr to malloc'ed copy of src
 */
static char *dup_string(char *src)
{
	char *dst;

	if((dst = malloc(strlen(src) + 1)) == NULL)
		printf("malloc failed in dup_string()");

	strcpy(dst, src);

	return dst;
}


/* get_file_data()
 *	Convenience function to return a pointer to the file data structure
 *	for a specified conf file handle.
 *
 *	Returns:
 *	  - ptr to conf file data structure
 *	  - NULL on error
 */
static _ggzcore_confio_file * get_file_data(int handle)
{
	_ggzcore_list_entry	*f_entry;
	_ggzcore_confio_file	*f_data=NULL;

	f_entry = _ggzcore_list_head(file_list);
	while(f_entry) {
		f_data = _ggzcore_list_get_data(f_entry);
		if(f_data->handle == handle)
			break;
		f_entry = _ggzcore_list_next(f_entry);
	}
	if(f_entry == NULL) {
		printf("get_file_data:  Invalid conf handle requested");
		f_data = NULL;
	}

	return f_data;
}


/* The following three functions perform list comparisons  */
/* and data creation and destruction for the section lists */
static int section_compare(void *a, void *b)
{
	/* Note that this function is a little odd since the 'b' passed */
	/* is not expected to be the full struct, but just the name str */
	_ggzcore_confio_section	*s_a;

	s_a = a;
	return strcmp(s_a->name, b);
}

static void *section_create(void *data)
{
	/* Note that this function is a little odd since the data passed */
	/* is not expected to be the full struct, but just the name str  */
	_ggzcore_confio_section	*dst;

	if((dst = malloc(sizeof(_ggzcore_confio_section))) == NULL)
		printf("malloc failed: section_create");

	/* Copy the section name and create an entry list */
	dst->name = dup_string(data);
	dst->entry_list = _ggzcore_list_create(entry_compare,
					       entry_create,
					       entry_destroy,
					       _GGZCORE_LIST_REPLACE_DUPS);
	if(!dst->entry_list) {
		free(dst->name);
		free(dst);
		dst = NULL;
	}

	return dst;
}

static void section_destroy(void *data)
{
	_ggzcore_confio_section	*s_data;

	s_data = data;
	free(s_data->name);
	_ggzcore_list_destroy(s_data->entry_list);
	free(s_data);
}


/* The following three functions perform list comparisons */
/* and data creation and destruction for the entry lists  */
static int entry_compare(void *a, void *b)
{
	_ggzcore_confio_entry	*e_a, *e_b;

	e_a = a;
	e_b = b;

	return strcmp(e_a->key, e_b->key);
}

static void *entry_create(void *data)
{
	_ggzcore_confio_entry	*src, *dst;

	src = data;
	if((dst = malloc(sizeof(_ggzcore_confio_entry))) == NULL)
		printf("malloc failed: entry_create");

	/* Copy the key and value data */
	dst->key = dup_string(src->key);
	dst->value = dup_string(src->value);

	return dst;
}

static void entry_destroy(void *data)
{
	_ggzcore_confio_entry	*e_data;

	e_data = data;
	free(e_data->key);
	free(e_data->value);
	free(e_data);
}

