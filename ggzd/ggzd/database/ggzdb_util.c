/*
 * Helper functions for ggzd database modules
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

#ifdef HAVE_CONFIG_H
# include <config.h>		/* Site specific config */
#endif

/* Unicode includes */
#ifdef WITH_ICU
#include <unicode/ustring.h>
#include <unicode/uchar.h>
#include <unicode/usprep.h>
#endif

#include <ggz.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ggzdb.h"
#include "ggzdb_util.h"

/* Helper function, might go into libggz*/
char *_ggz_sql_escape(const char *str)
{
	char *newstr, *q;
	const char *p;
	size_t len = 0;

	if(str == NULL)
		return NULL;

	len = strlen(str);

	for(p = str; *p != '\0'; p++) {
		if(*p == '\'') {
			len += 1;
		}
	}

	if(len == strlen(str))
		return ggz_strdup(str);

	newstr = ggz_malloc(len + 1);
	q = newstr;

	for(p = str; *p != '\0'; p++) {
		if(*p == '\'') {
			*q++ = '\'';
			*q = *p;
		} else {
			*q = *p;
		}
		q++;
	}
	*q = '\0';

	return newstr;
}

/* Helper function, might go into libggz*/
char *_ggz_sql_unescape(const char *str)
{
	char *new, *q;
	const char *p;
	size_t len = 0;

	if(str == NULL)
		return NULL;

	len = strlen(str);

	for(p = str; *p != '\0'; p++) {
		if(!strncmp(p, "\'\'", 2)) {
			p += 1;
		}
		len++;
	}

	if(len == strlen(str))
		return ggz_strdup(str);

	q = new = ggz_malloc(len + 1);
	for(p = str; *p != '\0'; p++) {
		if(!strncmp(p, "\'\'", 2)) {
			*q = '\'';
			q++;
			p += 1;
		} else {
			*q = *p;
			q++;
		}
	}
	*q = '\0';

	return new;
}

void _ggzdb_sgpstr_free(void *str)
{
	ggzdbSavegamePlayers *sp = str;
	int i;

	ggz_free(sp->owner);
	ggz_free(sp->savegame);
	for(i = 0; i < sp->count; i++) {
		ggz_free(sp->names[i]);
	}
	if(sp->names)
		ggz_free(sp->names);
	if(sp->types)
		ggz_free(sp->types);
	ggz_free(sp);
}

/* Function which returns canonicalized string */
/* FIXME: taken from unicode.c - see there for comments about bugs */
char *username_canonical(const char *username)
{
#ifdef WITH_ICU
	char *canonical;
	UStringPrepProfile *profile;
	UErrorCode status;
	uint32_t numchars;
	UChar *ustr = NULL;
	UChar *ustr2;
	int32_t length = 0;
	int32_t length2;
	UErrorCode error = U_ZERO_ERROR;

	if(!username)
		return NULL;
	if(!strlen(username))
		return ggz_strdup(username);

	status = U_ZERO_ERROR;
	profile = usprep_open(NULL, "uidna", &status);
	if(U_FAILURE(status))
	{
#if 0
		elog(ERROR, "Error: unable to open stringprep profile.");
#endif
		return NULL;
	}

	ustr = (UChar*)ggz_malloc(strlen(username) * 4);
	length = strlen(username) * 4;

	ustr = u_strFromUTF8(ustr, length, &length, username, -1, &error);
	ggz_free(ustr);
	if(U_FAILURE(error)) {
#if 0
		elog(ERROR, "Error: conversion failure");
#endif
		usprep_close(profile);
		return NULL;
	}
	ustr = (UChar*)ggz_malloc(sizeof(UChar) * length);
	if(!ustr) {
#if 0
		elog(ERROR, "Error: malloc failure");
#endif
		usprep_close(profile);
		return NULL;
	}
	ustr = u_strFromUTF8(ustr, length, NULL, username, -1, &error);

	length2 = length * 4;
	ustr2 = (UChar*)ggz_malloc(sizeof(UChar) * length2);

	numchars = usprep_prepare(profile, ustr, length, ustr2, length2, USPREP_DEFAULT, NULL, &error);
	ggz_free(ustr);
	if(U_FAILURE(error))
	{
#if 0
		elog(ERROR, "Error: stringprep failure");
#endif
		ggz_free(ustr2);
		usprep_close(profile);
		return NULL;
	}

	length = length2 * 4;
	canonical = ggz_malloc(length);
	error = U_ZERO_ERROR;
	u_strToUTF8(canonical, length, &length, ustr2, numchars, &error);
	if(U_FAILURE(error))
	{
#if 0
		elog(ERROR, "Error: conversion failure");
#endif
		ggz_free(canonical);
		ggz_free(ustr2);
		usprep_close(profile);
		return NULL;
	}

	ggz_free(ustr2);
	usprep_close(profile);

	return canonical;
#else
	return ggz_strdup(username);
#endif
}

/* Initialize the database tables from an external SQL schema file. */
int _ggz_setupschema(const char *filename, GGZDBExecSQL sql_exec, void *sql_conn)
{
	char buffer[1024];
	char sql_error[256];
	int sql_rc;
	char *completebuffer = NULL;
	int len;
	unsigned int i;
	int rc = 1;

	FILE *f = fopen(filename, "r");
	if(!f)
	{
		ggz_error_msg("Schema read error from %s.", filename);
		return 0;
	}

	while(fgets(buffer, sizeof(buffer), f))
	{
		if(strlen(buffer) == 1 && completebuffer)
		{
			sql_rc = sql_exec(sql_conn, completebuffer, sql_error, sizeof(sql_error));
			if(!sql_rc)
			{
				ggz_error_msg("Table creation error %s.",
					sql_error);
				rc = 0;
			}

			ggz_free(completebuffer);
			completebuffer = NULL;
			continue;
		}

		buffer[strlen(buffer) - 1] = '\0';
		for(i = 0; i < strlen(buffer); i++)
		{
			if(buffer[i] == '\t') buffer[i] = ' ';
		}

		len = (completebuffer ? strlen(completebuffer) : 0);
		completebuffer = (char*)ggz_realloc(completebuffer,
			len + strlen(buffer) + 1);
		if(len)
			strncat(completebuffer, buffer, strlen(buffer) + 1);
		else
			strncpy(completebuffer, buffer, strlen(buffer) + 1);
	}

	if(completebuffer) ggz_free(completebuffer);

	fclose(f);

	return rc;
}

