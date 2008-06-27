/* System includes */
#include <stdlib.h>
#include <string.h>

#include <mysql/mysql.h>

#include "canonicalstr.h"

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

