/*
 * The GGZ Gaming Zone Metaserver Project
 * Copyright (C) 2001 - 2003 Josef Spillner, josef@ggzgamingzone.org
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

char *meta_uri_host_internal(const char *uri)
{
	char *s, *tmp;
	char *host;

	tmp = strdup(uri);
	s = strtok(tmp, ":");
	if(s)
	{
		s = strtok(NULL, ":");
		if(s)
		{
			host = strdup(s + 2);
			while(s) s = strtok(NULL, ":");
			free(tmp);
			return host;
		}
	}
	return NULL;
}

int meta_uri_port_internal(const char *uri)
{
	char *s, *tmp;
	int port;

	tmp = strdup(uri);
	s = strtok(tmp, ":");
	if(s)
	{
		s = strtok(NULL, ":");
		if(s)
		{
			s = strtok(NULL, ":");
			if(s)
			{
				port = atoi(s);
				while(s) s = strtok(NULL, ":");
				free(tmp);
				return port;
			}
		}
	}
	return 0;
}

