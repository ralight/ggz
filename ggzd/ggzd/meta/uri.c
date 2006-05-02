#include "uri.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

URI uri_from_string(const char *uristring)
{
	URI uri;
	char *ptr;
	char *uricopy;
	char *base;
	char *port;

	uri.protocol = NULL;
	uri.user = NULL;
	uri.host = NULL;
	uri.port = 0;
	uri.path = NULL;

	if(!uristring) return uri;

	uricopy = strdup(uristring);
	base = uricopy;
	port = NULL;

	ptr = strstr(base, "://");
	if(ptr)
	{
		*ptr = '\0';
		uri.protocol = strdup(base);
		base = ptr + 3;
	}

	ptr = strstr(base, "@");
	if(ptr)
	{
		*ptr = '\0';
		uri.user = strdup(base);
		base = ptr + 1;
	}

	ptr = strstr(base, ":");
	if(ptr)
	{
		*ptr = '\0';
		uri.host = strdup(base);
		base = ptr + 1;
	}
	else
	{
		port = strdup("0");
	}

	ptr = strstr(base, "/");
	if(ptr)
	{
		*ptr = '\0';
		if(!port) port = strdup(base);
		else uri.host = strdup(base);
		base = ptr + 1;
		uri.path = strdup(base);
	}
	else
	{
		if(!uri.host) uri.host = strdup(base);
		else port = strdup(base);
	}

	if(port)
	{
		uri.port = atoi(port);
		free(port);
	}

	free(uricopy);

	return uri;
}

char *uri_to_string(URI uri)
{
	char protocol[32];
	char user[128];
	char host[128];
	char port[32];
	char path[128];

	protocol[0] = '\0';
	user[0] = '\0';
	host[0] = '\0';
	port[0] = '\0';
	path[0] = '\0';

	if(uri.protocol)
		snprintf(protocol, sizeof(protocol), "%s://", uri.protocol);
	if(uri.user)
		snprintf(user, sizeof(user), "%s@", uri.user);
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

void uri_free(URI uri)
{
	if(uri.protocol) free(uri.protocol);
	if(uri.user) free(uri.user);
	if(uri.host) free(uri.host);
	if(uri.path) free(uri.path);

	uri.protocol = NULL;
	uri.user = NULL;
	uri.host = NULL;
	uri.port = 0;
	uri.path = NULL;
}

