/*
 * GGZ convenience C programming library (libggz)
 * URI handling functions, originally in libggzmeta
 * Copyright (C) 2006, 2007 Josef Spillner <josef@ggzgamingzone.org>

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/* Header files */
#include "ggz.h"

/* System includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

ggz_uri_t ggz_uri_from_string(const char *uristring)
{
	ggz_uri_t uri;
	char *ptr, *pwdptr;
	char *uricopy;
	char *base;
	char *port;

	uri.protocol = NULL;
	uri.user = NULL;
	uri.password = NULL;
	uri.host = NULL;
	uri.port = 0;
	uri.path = NULL;

	if(!uristring) return uri;

	uricopy = ggz_strdup(uristring);
	base = uricopy;
	port = NULL;

	ptr = strstr(base, "://");
	if(ptr)
	{
		*ptr = '\0';
		uri.protocol = ggz_strdup(base);
		base = ptr + 3;
	}

	ptr = strstr(base, "@");
	if(ptr)
	{
		*ptr = '\0';
		pwdptr = strstr(base, ":");
		if(pwdptr)
		{
			*pwdptr = '\0';
			uri.user = ggz_strdup(base);
			uri.password = ggz_strdup(pwdptr + 1);
		}
		else
		{
			uri.user = ggz_strdup(base);
		}
		base = ptr + 1;
	}

	ptr = strstr(base, ":");
	if(ptr)
	{
		*ptr = '\0';
		uri.host = ggz_strdup(base);
		base = ptr + 1;
	}
	else
	{
		port = ggz_strdup("0");
	}

	ptr = strstr(base, "/");
	if(ptr)
	{
		*ptr = '\0';
		if(!port) port = ggz_strdup(base);
		else uri.host = ggz_strdup(base);
		base = ptr + 1;
		uri.path = ggz_strdup(base);
	}
	else
	{
		if(!uri.host) uri.host = ggz_strdup(base);
		else port = ggz_strdup(base);
	}

	if(port)
	{
		uri.port = atoi(port);
		ggz_free(port);
	}

	ggz_free(uricopy);

	return uri;
}

char *ggz_uri_to_string(ggz_uri_t uri)
{
	char protocol[32];
	char user[128];
	char password[128];
	char host[128];
	char port[32];
	char path[128];

	protocol[0] = '\0';
	user[0] = '\0';
	password[0] = '\0';
	host[0] = '\0';
	port[0] = '\0';
	path[0] = '\0';

	if(uri.protocol)
		snprintf(protocol, sizeof(protocol), "%s://", uri.protocol);
	if(uri.user)
	{
		if(uri.password)
			snprintf(user, sizeof(user), "%s:*****@", uri.user);
		else
			snprintf(user, sizeof(user), "%s@", uri.user);
	}
	if(uri.host)
		snprintf(host, sizeof(host), "%s", uri.host);
	if(uri.port > 0)
		snprintf(port, sizeof(port), ":%i", uri.port);
	if(uri.path)
		snprintf(path, sizeof(path), "%s", uri.path);

	char *s = (char*)malloc(256);
	snprintf(s, 256, "%s%s%s%s%s", protocol, user, host, port, path);
	return s;
}

void ggz_uri_free(ggz_uri_t uri)
{
	if(uri.protocol)
		ggz_free(uri.protocol);
	if(uri.user)
		ggz_free(uri.user);
	if(uri.password)
		ggz_free(uri.password);
	if(uri.host)
		ggz_free(uri.host);
	if(uri.path)
		ggz_free(uri.path);

	uri.protocol = NULL;
	uri.user = NULL;
	uri.password = NULL;
	uri.host = NULL;
	uri.port = 0;
	uri.path = NULL;
}

