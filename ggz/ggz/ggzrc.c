/*
 * File: ggzrc.c
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 03/02/2000
 *
 * This fils contains functions for connecting with the server
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

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "ggzrc.h"
#include "err_func.h"


/* Local use only functions */
static void ggzrc_load_rc(FILE *);
static void ggzrc_parse_line(char *);
static gboolean ggzrc_free_keyval(gpointer, gpointer, gpointer);
static gint ggzrc_strcmp(gconstpointer, gconstpointer);
static char *ggzrc_encoded_string(char *);
static char *ggzrc_decoded_string(char *);

/* Local use only variables */
static char *varname;
static char *varvalue;
static GHashTable *rc_hash=NULL;
static GSList *rc_list=NULL;
static gboolean local_conf = FALSE;


/* Write a string variable to the hash table */
void ggzrc_write_string(const char *section, const char *key, const char *value)
{
	char *hashkey;
	gpointer old_key, old_value;

	hashkey = g_strdup_printf("[%s]%s", section, key);
	if(g_hash_table_lookup_extended(rc_hash, hashkey, &old_key, &old_value)
	   == TRUE) {
		/* If it existed, let's use the same old key */
		g_free(old_value);
		g_free(hashkey);
		hashkey = old_key;
	} else {
		/* If it didn't exist, put it into the rc_list */
		rc_list = g_slist_insert_sorted(rc_list, hashkey, ggzrc_strcmp);
	}
	g_hash_table_insert(rc_hash, hashkey, g_strdup(value));
}


/* Read a string variable out of the hash table */
char *ggzrc_read_string(const char *section, const char *key, const char *def)
{
	char *hashkey, *data;

	hashkey = g_strdup_printf("[%s]%s", section, key);
	data = g_hash_table_lookup(rc_hash, hashkey);
	if(data == NULL) {
		if(def) {
			ggzrc_write_string(section, key, def);
			return g_strdup(def);
		} else
			return NULL;
	}
	return g_strdup(data);
}


/* Wrap the string write function for integers */
void ggzrc_write_int(const char *section, const char *key, const int value)
{
	char *tmp = g_strdup_printf("%d", value);
	ggzrc_write_string(section, key, tmp);
	g_free(tmp);
}


/* Wrap the string read function for integers */
int ggzrc_read_int(const char *section, const char *key, const int def)
{
	char *tmp = g_strdup_printf("%d", def);
	char *tmp2 = ggzrc_read_string(section, key, tmp);
	int retval = atoi(tmp2);
	g_free(tmp);
	g_free(tmp2);
	return retval;
}


/* Wrap the string write function for lists */
void ggzrc_write_list(const char *section, const char *key,
		       const int argc, const char **argv)
{
	int i;
	char *current_item;
	char *final_str;
	char *tmp;
	char *dest_ptr;
	const char *src_ptr;

	final_str = g_strdup("");
	
	for (i = 0; i < argc; i++) {
		current_item = g_malloc((strlen(argv[i]) * 2) + 1);
		dest_ptr = current_item;
		src_ptr = argv[i];
		
		while(*src_ptr != '\0') {
			if (*src_ptr == ' ') {
				*dest_ptr++ = '\\';
			}
			*dest_ptr++ = *src_ptr++;
		}
		*dest_ptr = '\0';

		tmp = g_strdup_printf("%s %s", final_str, current_item);
		g_free(final_str);
		g_free(current_item);
		final_str = tmp;
	}

	ggzrc_write_string(section, key, final_str);
	g_free(final_str);
}


/* Wrap the string read function for lists */
void ggzrc_read_list(const char *section, const char *key,
		      int *argcp, char ***argvp)
{
	int index;
	char *p, *s1, *s2;
	char *tmp;
	char *str = ggzrc_read_string(section, key, NULL);
	gboolean saw_space = FALSE;
	gboolean saw_backspace;
	
	if (str != NULL) {
		for (*argcp = 1, p = str; *p != '\0'; p++) {
			if (*p == '\\' && *(p+1)) {
				p++;
				if (saw_space) {
					*argcp += 1;
					saw_space = FALSE;
				}
			} else if (*p == ' ') {
				saw_space = TRUE;
			} else if (saw_space) {
				*argcp += 1;
				saw_space = FALSE;
			}
		}

		*argvp = (char **) g_malloc((*argcp + 1) * sizeof(char *));

		p = str;
		index = 0;
		do {
			tmp = p;
			
			for (saw_backspace = FALSE;
			     *p != '\0' && (saw_backspace ? 1 : (*p != ' '));
			     p++) {
				if (*p == '\\')
					saw_backspace = TRUE;
				else
					saw_backspace = FALSE;
			}
			
			(*argvp)[index] = (char *) g_strndup(tmp, p - tmp);
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

		g_free(str);		
	} else {
		*argcp = 0;
	}
}

/* Remove an entire section from the configuration data */
void ggzrc_remove_section(const char *section)
{
	int section_length = strlen(section);
	GSList *iter_list = rc_list;

	while(iter_list) {
		if (strncmp(section, &((char *)iter_list->data)[1],
		    section_length) == 0)
			g_hash_table_remove(rc_hash, iter_list->data);

		iter_list = g_slist_next(iter_list);
	}
}

/* Remove a single key from the configuration data */
void ggzrc_remove_key(const char *section, const char *key)
{
	char *hashkey = g_strdup_printf("[%s]%s", section, key);

	g_hash_table_remove(rc_hash, hashkey);
	g_free(hashkey);
}

/* Initialize and read in the configuration file(s) */
int ggzrc_initialize(char *rc_fname)
{
	char *tempstr;
	char *home;
	FILE *rc_file;
	int status = -1;

	rc_hash = g_hash_table_new(g_str_hash, g_str_equal);

	if(rc_fname) {
		/* Open and parse a user specified rc file */
		if((rc_file = fopen(rc_fname, "r")) == NULL) {
			err_sys("Unable to read configuration file");
			return -1;
		}
		dbg_msg("ggzrc: Reading %s", rc_fname);
		ggzrc_load_rc(rc_file);
		local_conf = TRUE;
		return 0;
	}

	/* Open and parse the main ggz.rc file */
	tempstr = g_strconcat(GGZCONFDIR, "/ggz.rc", NULL);
	if((rc_file = fopen(tempstr, "r"))) {
		dbg_msg("ggzrc: Reading %s", tempstr);
		ggzrc_load_rc(rc_file);
		status = 0;
	} else
		dbg_msg("ggzrc: Unable to open %s/ggz.rc", GGZCONFDIR);
	g_free(tempstr);

	/* Open and parse the ~/.ggzrc file */
	if((home = getenv("HOME")) != NULL) {
		tempstr = g_strconcat(home, "/.ggzrc", NULL);
		if((rc_file = fopen(tempstr, "r"))) {
			dbg_msg("ggzrc: Reading %s/.ggzrc", home);
			ggzrc_load_rc(rc_file);
			status = 0;
		} else
			dbg_msg("ggzrc: Unable to open %s/.ggzrc", home);
		g_free(tempstr);
	} else
		dbg_msg("ggzrc: $HOME environment variable not set");

	return status;
}


/* Parse the pre-openend rc file, close the file when done */
static void ggzrc_load_rc(FILE *rc_file)
{
	char line[256];		/* Lines longer than 256 are trunced */
	char *hashkey;
	char *section;
	int linenum = 0;
	gpointer old_key, old_value;

	/* Prepare the hash table for mass updates */
	g_hash_table_freeze(rc_hash);

	section = g_strdup("[none]");
	while(fgets(line, 256, rc_file)) {
		linenum++;
		ggzrc_parse_line(line);
		if(varname == NULL)
			continue; /* Blank line or comment */
		if(varvalue == NULL) {
			/* Might be a [SectionID] */
			if(varname[0] == '['
			   && varname[strlen(varname)-1] == ']') {
				g_free(section);
				section = g_strdup(varname);
			} else
				err_msg("ggzrc: Syntax error, line %d",linenum);
			continue;
		}

		/* We have a valid varname/varvalue, add them to hash table */
		dbg_msg("ggzrc: found '%s %s = %s'", section,varname,varvalue);
		hashkey = g_strconcat(section, varname, NULL);
		if(g_hash_table_lookup_extended(rc_hash, hashkey,
						&old_key, &old_value)
	   	   == TRUE) {
			/* If it existed, let's use the same old key */
			g_free(old_value);
			g_free(hashkey);
			hashkey = old_key;
		} else {
			/* If it didn't exist, put it into the rc_list */
			rc_list = g_slist_prepend(rc_list, hashkey);
		}
		g_hash_table_insert(rc_hash, hashkey,
				    ggzrc_decoded_string(varvalue));
	}

	/* Do time intensive stuff now that we are out of the loop */
	rc_list = g_slist_sort(rc_list, ggzrc_strcmp);
	g_hash_table_thaw(rc_hash);

	g_free(section);
	fclose(rc_file);
}


/* Commit the hash table into an rc file */
int ggzrc_commit_changes(void)
{
	FILE *rc_file;
	char *filename;
	char *home;
	char *data;
	char *encoded_value;
	char *cur_section;
	int first_section;
	char *tmp, *section, *key, *value;
	GSList *iter_list=rc_list;

	/* We don't overwrite real configuration files from test files */
	if(local_conf)
		return 0;

	if((home=getenv("HOME")) == NULL) {
		err_msg("ggzrc: Can't write ~/.ggzrc, can't find $HOME");
		return -1;
	}
	filename = g_strconcat(home, "/.ggzrc", NULL);
	if((rc_file=fopen(filename, "w")) == NULL) {
		err_msg("ggzrc: Can't open %s for write", filename);
		return -1;
	}

	cur_section = g_strdup("none");
	first_section = 1;
	while(iter_list) {
		data = g_strdup(iter_list->data);
		value = g_hash_table_lookup(rc_hash, data);
		tmp = section = data+1;
		while(*tmp != ']')
			tmp++;
		*tmp = '\0';
		key = tmp+1;
		if(strcmp(cur_section, section)) {
			g_free(cur_section);
			cur_section = g_strdup(section);
			if(first_section) {
				fprintf(rc_file, "[%s]\n", cur_section);
				first_section = 0;
			} else
				fprintf(rc_file, "\n[%s]\n", cur_section);
		}
		encoded_value = ggzrc_encoded_string(value);
		fprintf(rc_file, "%s = %s\n", key,
			(encoded_value != NULL) ? encoded_value : value);
		if (encoded_value != NULL) g_free(encoded_value);
		g_free(data);
		iter_list = g_slist_next(iter_list);
	}

	fclose(rc_file);
	return 0;
}


/* Cleanup by freeing up the hash table and list */
void ggzrc_cleanup(void)
{
	g_hash_table_foreach_remove(rc_hash, ggzrc_free_keyval, NULL);
	g_hash_table_destroy(rc_hash);
	rc_hash = NULL;
	g_slist_free(rc_list);
	rc_list = NULL;
}


/* Phase 1 of cleanup is to free the keys/values */
static gboolean ggzrc_free_keyval(gpointer key, gpointer value, gpointer data)
{
	g_return_val_if_fail(key != NULL, FALSE);
	g_return_val_if_fail(value != NULL, FALSE);
	g_free(key);
	g_free(value);
	return TRUE;
}


/* This wraps strcmp to eliminate a compile warning */
gint ggzrc_strcmp(gconstpointer a, gconstpointer b)
{
	return strcmp(a, b);
}


/* Parse a single line of input into a left half and a right half */
/* separated by an (optional) equals sign                         */
static void ggzrc_parse_line(char *p)
{
	char csave, *psave, *sol;
	
	/* Save start-of-line in sol */
	sol = p;

	varname = NULL;
	/* Skip over whitespace */
	while((*p == ' ' || *p == '\t' || *p == '\n') && *p != '\0')
		p++;
	if(*p == '\0' || *p == '#')
		return;	/* The line is a comment */

	varname = p;

	varvalue = NULL;
	/* Skip until we find an equals sign */
	while(*p != '=' && *p != '\n' && *p != '\0')
		p++;
	csave = *p;
	psave = p;

	if (*p == '=') {
		/* Found '=', now backspace to remove trailing space */
		do {
			p--;
		} while(p >= sol && (*p == ' ' || *p == '\t' || *p == '\n'));
		p++;
	}
	*p = '\0';
	p = psave;
	p++;
	
	if(*varname == '\0' || p == sol) {
		varname = NULL;
		return;
	}

	if(csave == '\n' || csave == '\0')
		return;	/* There is no argument */

	/* There appears to be an argument, skip to the start of it */
	while((*p == ' ' || *p == '\t' || *p == '\n' || *p == '=')&& *p != '\0')
		p++;
	if(*p == '\0')
		return; /* Argument is missing */

	/* There is an argument ... */
	varvalue = p;

	/* Terminate it ... */
	while(*p != '\n' && *p != '\0')
		p++;
	/* Found EOL, now backspace over whitespace to remove trailing space */
	p--;
	while(*p == ' ' || *p == '\t' || *p == '\n')
		p--;
	p++;
	/* Finally terminate it with a NUL */
	*p = '\0'; /* Might have already been the NUL, but who cares? */
}

/* Encode special characters within string using C notation.          */
/* The string returned by this function must be free'd by the caller. */
/* On error, returns NULL.                                            */
static char *ggzrc_encoded_string(char *str)
{
	char *buffer;
	char *encoded_str;
	char *buffer_ptr;
	
	buffer = g_malloc((strlen(str) * 2) + 1);
	if (buffer != NULL) {
		buffer_ptr = buffer;
		while (*str != '\0') {
			switch(*str) {
			case '\n':
				*buffer_ptr++ = '\\';
				*buffer_ptr++ = 'n';
				break;

			case '\r':
				*buffer_ptr++ = '\\';
				*buffer_ptr++ = 'r';
				break;

			case '\t':
				*buffer_ptr++ = '\\';
				*buffer_ptr++ = 't';
				break;

			case '\\':
				*buffer_ptr++ = '\\';
				*buffer_ptr++ = '\\';
				break;

			default:
				*buffer_ptr++ = *str;
				break;
			}
			str++;
		}
		*buffer_ptr = '\0';

		encoded_str = g_strdup(buffer);
		g_free(buffer);
	} else {
		encoded_str = NULL;
	}

	return encoded_str;
}

/* Unencode escaped characters into ASCII equivalents.                */
/* The string returned by this function must be free'd by the caller. */
static char *ggzrc_decoded_string(char *str)
{
	char *decoded_str;
	char *dest_ptr;
	
	decoded_str = g_strdup(str);
	dest_ptr = decoded_str;

	while (*str != '\0') {
		if(*str == '\\') {
			switch(*(str+1)) {
			case 'n':
				*dest_ptr++ = '\n';
				break;

			case 'r':
				*dest_ptr++ = '\r';
				break;

			case 't':
				*dest_ptr++ = '\t';
				break;

			case '\\':
				*dest_ptr++ = '\\';
				break;

			default:
				*dest_ptr++ = '\\';
				*dest_ptr++ = *(str+1);
			}
			str++;
		} else {
			*dest_ptr++ = *str;
		}

		str++;
	}
	*dest_ptr = '\0';

	return decoded_str;
}
