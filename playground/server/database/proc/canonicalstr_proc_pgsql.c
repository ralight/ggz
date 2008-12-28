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

/* Server-side includes */
#include <postgres.h>
#include <executor/spi.h>
#include <commands/trigger.h>

/* System includes */
#include <string.h>

#include "canonicalstr.h"

/* Standard PostgreSQL module declarations */
#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif


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

