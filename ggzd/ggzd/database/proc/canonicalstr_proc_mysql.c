#include <unicode/ustring.h>
#include <unicode/uchar.h>
#include <unicode/usprep.h>

/* System includes */
#include <stdlib.h>
#include <string.h>

#include <mysql/mysql.h>

/* Function which returns canonicalized string */
/* FIXME: taken from unicode.c - see there for comments about bugs */
static char *username_canonical(const char *username)
{
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
		return NULL;
	}

	ustr = (UChar*)malloc(strlen(username) * 4);
	length = strlen(username) * 4;

	ustr = u_strFromUTF8(ustr, length, &length, username, -1, &error);
	free(ustr);
	if(U_FAILURE(error)) {
		usprep_close(profile);
		return NULL;
	}
	ustr = (UChar*)malloc(sizeof(UChar) * length);
	if(!ustr) {
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
		free(canonical);
		free(ustr2);
		usprep_close(profile);
		return NULL;
	}

	free(ustr2);
	usprep_close(profile);

	return canonical;
}


my_bool canonicalstr_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  return 0;
}


void canonicalstr_deinit(UDF_INIT *initid __attribute__((unused)))
{
}


char *canonicalstr(UDF_INIT *initid __attribute((unused)),
					UDF_ARGS *args, char *result, unsigned long *length,
					char *is_null, char *error __attribute__((unused)))
{
	const char *username = args->args[0];
	char *can;

	if(!username){
		*is_null = 1;
		return NULL;
	}

	can = username_canonical(username);
	if(!can)
	{
		*is_null = 1;
		return NULL;
	}

	*length = strlen(can);
	return can;
}

