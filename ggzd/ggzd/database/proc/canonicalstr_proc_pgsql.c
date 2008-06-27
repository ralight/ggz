/*
 * Function to return the normalized version of a string (e.g. username)
 * according to a "stringprep" profile.
 *
 * This code runs as a plugin directly in the database server. As such
 * it is subject to careful changes only!
 */

/* Server-side includes */
#include <postgres.h>
#include <executor/spi.h>
#include <commands/trigger.h>

/* Unicode includes */
#ifdef WITH_ICU
#include <unicode/ustring.h>
#include <unicode/uchar.h>
#include <unicode/usprep.h>
#endif

/* System includes */
#include <string.h>

/* Standard PostgreSQL module declarations */
#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

/* Function which returns canonicalized string */
/* FIXME: taken from unicode.c - see there for comments about bugs */
static char *username_canonical(const char *username)
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
		elog(ERROR, "Error: unable to open stringprep profile.");
		return NULL;
	}

	ustr = (UChar*)malloc(strlen(username) * 4);
	length = strlen(username) * 4;

	ustr = u_strFromUTF8(ustr, length, &length, username, -1, &error);
	free(ustr);
	if(U_FAILURE(error)) {
		elog(ERROR, "Error: conversion failure");
		usprep_close(profile);
		return NULL;
	}
	ustr = (UChar*)malloc(sizeof(UChar) * length);
	if(!ustr) {
		elog(ERROR, "Error: malloc failure");
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
		elog(ERROR, "Error: stringprep failure");
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
		elog(ERROR, "Error: conversion failure");
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


extern Datum canonicalstr(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(canonicalstr);
Datum canonicalstr(PG_FUNCTION_ARGS)
{
	text *t, *canonical_t;
	int len;
	char *username, *can;

	if(PG_ARGISNULL(0))
	{
		PG_RETURN_NULL();
	}

	t = PG_GETARG_TEXT_P(0);
	len = VARSIZE(t) - VARHDRSZ;
	username = (char*)palloc(len + 1);
	memcpy(username, VARDATA(t), len);
	username[len] = '\0';
#if 0
	elog(INFO, "Received text: %s (%i bytes)", username, len);
#endif

	can = username_canonical(username);
	if(!can)
	{
		/* FIXME: what to do in case of errors? */
		PG_RETURN_NULL();
	}
	len = strlen(can);
#if 0
	elog(INFO, "Returned text: %s (%i bytes)", can, len);
#endif

	canonical_t = (text*)palloc(len + VARHDRSZ);
	VARATT_SIZEP(canonical_t) = len + VARHDRSZ;

	memcpy((void*)VARDATA(canonical_t), (void*)can, len);
	free(can);

	PG_RETURN_TEXT_P(canonical_t);
}

