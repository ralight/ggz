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

	status = U_ZERO_ERROR;
	profile = usprep_open(NULL, "uidna", &status);
	if(U_FAILURE(status))
	{
#if 0
		elog(ERROR, "Error: unable to open stringprep profile.");
#endif
		return NULL;
	}

	ustr = (UChar*)malloc(strlen(username) * 4);
	length = strlen(username) * 4;

	ustr = u_strFromUTF8(ustr, length, &length, username, -1, &error);
	free(ustr);
	if(U_FAILURE(error)) {
#if 0
		elog(ERROR, "Error: conversion failure");
#endif
		usprep_close(profile);
		return NULL;
	}
	ustr = (UChar*)malloc(sizeof(UChar) * length);
	if(!ustr) {
#if 0
		elog(ERROR, "Error: malloc failure");
#endif
		usprep_close(profile);
		return NULL;
	}
	ustr = u_strFromUTF8(ustr, length, NULL, username, -1, &error);

	length2 = length * 4;
	ustr2 = (UChar*)malloc(sizeof(UChar) * length2);

	numchars = usprep_prepare(profile, ustr, length, ustr2, length2, USPREP_DEFAULT, NULL, &error);
	free(ustr);
	if(U_FAILURE(error))
	{
#if 0
		elog(ERROR, "Error: stringprep failure");
#endif
		free(ustr2);
		usprep_close(profile);
		return NULL;
	}

	length = length2 * 4;
	canonical = malloc(length);
	error = U_ZERO_ERROR;
	u_strToUTF8(canonical, length, &length, ustr2, numchars, &error);
	if(U_FAILURE(error))
	{
#if 0
		elog(ERROR, "Error: conversion failure");
#endif
		free(canonical);
		free(ustr2);
		usprep_close(profile);
		return NULL;
	}

	free(ustr2);
	usprep_close(profile);

	return canonical;
#else
	return ggz_strdup(username);
#endif
}

