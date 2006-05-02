/*
 * The GGZ Gaming Zone Metaserver Project
 * Copyright (C) 2001 - 2006 Josef Spillner <josef@ggzgamingzone.org>
 * Published under GNU GPL conditions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "helper.h"

extern char *logfile;

void logline(const char *fmt, ...)
{
	FILE *f;
	time_t t;
	char *ct;
	va_list ap;
	int closefile = 0;

	if(!logfile) return;

	if(!strcmp(logfile, "stdout")) f = stdout;
	else if(!strcmp(logfile, "stderr")) f = stderr;
	else
	{
		f = fopen(logfile, "a");
		closefile = 1;
	}

	if(f)
	{
		t = time(NULL);
		ct = ctime(&t);
		if(ct) ct[strlen(ct) - 1] = 0;
		else ct = "*";
		fprintf(f, "%s: ", ct);
		va_start(ap, fmt);
		vfprintf(f, fmt, ap);
		va_end(ap);
		fprintf(f, "\n");
		if(closefile) fclose(f);
	}
}

/* except for URL quoting, none of this should be necessary */
/* but we still do it to prevent a nasty misconfiguration in the cache */
/* also, in the metaserver updates, attributes and elements get swapped! */
/* attributes need '/" quoted, elements need </>, and all need & */
/* URL whitespace needs to become %20 but this is a semantic issue */
char *xml_strdup(const char *s, int attribute, int url)
{
	char *s2 = NULL;
	int i;
	int j = 0;

	if(!s) return s2;

	s2 = (char*)malloc(j + 1);
	s2[j] = 0;
	j++;
	for(i = 0; i < strlen(s); i++)
	{
		if((s[i] == '&')
		&& ((i > strlen(s) - 5)
		   || (strncmp(s + i, "&amp;", 5))
		   || (strncmp(s + i, "&lt;", 4))
		   || (strncmp(s + i, "&gt;", 4))
		   || (strncmp(s + i, "&quot;", 6))
		   || (strncmp(s + i, "&apos;", 6))))
		{
			s2 = (char*)realloc(s2, j + 5);
			strcpy(s2 + j - 1, "&amp;");
			s2[j + 4] = 0;
			j += 5;
			continue;
		}

		if((s[i] == '>') && (!attribute))
		{
			s2 = (char*)realloc(s2, j + 4);
			strcpy(s2 + j - 1, "&gt;");
			s2[j + 3] = 0;
			j += 4;
			continue;
		}

		if((s[i] == '<') && (!attribute))
		{
			s2 = (char*)realloc(s2, j + 4);
			strcpy(s2 + j - 1, "&lt;");
			s2[j + 3] = 0;
			j += 4;
			continue;
		}

		if((s[i] == '\'') && (attribute))
		{
			s2 = (char*)realloc(s2, j + 6);
			strcpy(s2 + j - 1, "&apos;");
			s2[j + 5] = 0;
			j += 6;
			continue;
		}

		if((s[i] == '\"') && (attribute))
		{
			s2 = (char*)realloc(s2, j + 6);
			strcpy(s2 + j - 1, "&quot;");
			s2[j + 5] = 0;
			j += 6;
			continue;
		}

		if((s[i] == ' ') && (url))
		{
			s2 = (char*)realloc(s2, j + 3);
			strcpy(s2 + j - 1, "%20");
			s2[j + 2] = 0;
			j += 3;
			continue;
		}

		s2 = (char*)realloc(s2, j + 1);
		s2[j - 1] = s[i];
		s2[j] = 0;
		j++;
	}

	return s2;
}

