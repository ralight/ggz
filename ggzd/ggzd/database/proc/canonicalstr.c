/*
 * Function to return the normalized version of a string (e.g. username)
 * according to a "stringprep" profile.
 *
 * This code runs as a plugin directly in the database server. As such
 * it is subject to careful changes only!
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

/* System includes */
#include <stdlib.h>
#include <string.h>

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
	return strdup(username);
#endif
}

