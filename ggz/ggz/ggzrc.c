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
static FILE *ggzrc_open_rc(void);
static void ggzrc_load_rc(FILE *);
static void ggzrc_parse_line(char *);
static gboolean ggzrc_free_keyval(gpointer, gpointer, gpointer);

/* Local use only variables */
static char *varname;
static char *varvalue;
static GHashTable *rc_hash;
static GSList *rc_list=NULL;


/* Initialize and read in the configuration file */
int ggzrc_initialize(void)
{
	FILE *rc_file;

	/* Open our rc file */
	if((rc_file = ggzrc_open_rc()) == NULL) {
		dbg_msg("ggzrc: Unable to located ggz.rc or .ggzrc");
		return -1;
	}

	/* Parse all the lines in the rc file */
	ggzrc_load_rc(rc_file);
	return 0;
}


/* Locate the rc file to use */
static FILE *ggzrc_open_rc(void)
{
	FILE *rc_file;
	char *home;
	char *tempstr=NULL;

	/* Look for a user level rc file first */
	if((home = getenv("HOME")) != NULL)
		tempstr = g_strconcat(home, "/.ggzrc", NULL);
	if(tempstr && (rc_file=fopen(tempstr, "r")))
		dbg_msg("ggzrc: Using %s/.ggzrc", home);
	else {
		/* Look for an admin level rc file */
		if(tempstr)
			g_free(tempstr);
		tempstr = g_strconcat(SYSCONFDIR, "/ggz.rc", NULL);
		if((rc_file=fopen(tempstr, "r")))
			dbg_msg("ggzrc: Using %s", tempstr);
	}

	g_free(tempstr);
	return rc_file;
}


/* Parse the pre-openend rc file, close the file when done */
static void ggzrc_load_rc(FILE *rc_file)
{
	char line[256];		/* Lines longer than 256 are trunced */
	char *hashname;
	char *section;
	int linenum = 0;

	/* Initialize the hash and prepare for mass updates */
	rc_hash = g_hash_table_new(g_str_hash, g_str_equal);
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
		hashname = g_strconcat(section, varname, NULL);
		g_hash_table_insert(rc_hash, hashname, g_strdup(varvalue));
		rc_list = g_slist_prepend(rc_list, hashname);
	}

	/* Do time intensive stuff now that we are out of the loop */
	rc_list = g_slist_sort(rc_list, strcmp);
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
	char *cur_section;
	int first_section;
	char *tmp, *section, *key, *value;
	GSList *iter_list=rc_list;
	
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
		fprintf(rc_file, "%s = %s\n", key, value);
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


/* Parse a single line of input into a left half and a right half */
/* separated by an (optional) equals sign                         */
static void ggzrc_parse_line(char *p)
{
	char csave;

	varname = NULL;
	/* Skip over whitespace */
	while((*p == ' ' || *p == '\t' || *p == '\n') && *p != '\0')
		p++;
	if(*p == '\0' || *p == '#')
		return;	/* The line is a comment */

	varname = p;

	varvalue = NULL;
	/* Skip until we find the end of the variable name */
	while(*p != ' ' && *p != '\t' && *p != '\n' && *p != '=' && *p != '\0')
		p++;
	csave = *p;
	*p = '\0';
	p++;

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
