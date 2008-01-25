/*
 * GGZ Metaserver access library (libggzmeta)
 * Client-side access to the metaserver
 * Copyright (C) 2001 - 2006 Josef Spillner <josef@ggzgamingzone.org>

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* Header files */
#include "meta.h"
#include "minidom.h"

/* System includes */
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/stat.h>

/* Prototypes */
static char *meta_query_internal(int fd, const char *text);
static int meta_connect_internal(const char *hostname, int port);

/* === Public functions === */
/* ======================== */

ServerEntry **meta_network_load()
{
	FILE *f;
	char cachefile[1024];
	char buf[1024];
	ggz_uri_t uri;
	ServerEntry *server;
	ServerEntry **metaservers = NULL;

	snprintf(cachefile, sizeof(cachefile),
		"%s/.ggz/metaserver.cache", getenv("HOME"));

	f = fopen(cachefile, "r");
	if(f)
	{
		while(fgets(buf, sizeof(buf), f))
		{
			buf[strlen(buf) - 1] = 0;
			if(strlen(buf))
			{
				uri = ggz_uri_from_string(buf);
				server = meta_server_new(uri);
				metaservers = meta_list_server(metaservers, server);
			}
		}
		fclose(f);
	}

	return metaservers;
}

void meta_network_store(ServerEntry **servers)
{
	FILE *f;
	char cachefile[1024];
	int i;
	char *uristr;

	if(!servers) return;

	snprintf(cachefile, sizeof(cachefile),
		"%s/.ggz", getenv("HOME"));
	mkdir(cachefile, S_IRWXU);

	snprintf(cachefile, sizeof(cachefile),
		"%s/.ggz/metaserver.cache", getenv("HOME"));

	f = fopen(cachefile, "w");
	if(f)
	{
		for(i = 0; servers[i]; i++)
		{
			uristr = ggz_uri_to_string(servers[i]->uri);
			fprintf(f, "%s\n", uristr);
			free(uristr);
		}
		fclose(f);
	}
}

int meta_add(ServerEntry *server, const char *classname, const char *category,
	const char *metaserveruri, const char *username, const char *password)
{
	char query[1024];
	char options[512];
	char oldoptions[512];
	int fd;
	char *s;
	DOM *dom;
	ELE *root;
	ELE *el;
	ggz_uri_t uri;
	char *serveruri;
	int accepted;
	int i;

	accepted = 0;

	uri = ggz_uri_from_string(metaserveruri);
	if(!uri.port) uri.port = 15689;
	fd = meta_connect_internal(uri.host, uri.port);
	ggz_uri_free(uri);

	if(fd >= 0)
	{
		serveruri = ggz_uri_to_string(server->uri);
		strcpy(options, "");
		if(server->attributes)
		{
			for(i = 0; server->attributes[i].key; i++)
			{
				snprintf(oldoptions, sizeof(oldoptions),
					"%s",
					options);
				snprintf(options, sizeof(options),
					"%s"
					"<option name=\"%s\">%s</option>",
					oldoptions,
					server->attributes[i].key, server->attributes[i].value);
			}
		}

		snprintf(query, sizeof(query),
			"<?xml version=\"1.0\"?>"
			"<update class=\"%s\" type=\"%s\" uri=\"%s\" "
			"username=\"%s\" password=\"%s\" mode=\"add\">"
			"%s"
			"</update>\n",
			classname, category, serveruri,
			(username ? username : ""), (password ? password : ""),
			options);
		free(serveruri);

		s = meta_query_internal(fd, query);
		if(s)
		{
			dom = minidom_parse(s);
			if((dom) && (dom->processed) && (dom->valid))
			{
				root = dom->el;
				if(root)
				{
					el = MD_query(root, "status");
					if(el)
					{
						if(el->value)
						{
							if(!strcmp(el->value, "accepted")) accepted = 1;
						}
					}
				}

				minidom_free(dom);
			}

			free(s);
		}
	}

	return accepted;
}

ServerEntry *meta_server_new(ggz_uri_t uri)
{
	ServerEntry *server;

	server = (ServerEntry*)malloc(sizeof(ServerEntry));

	server->uri = uri;
	server->attributes = NULL;

	return server;
}

void meta_server_attribute(ServerEntry *server,
	const char *key, const char *value)
{
	int i;

	if(!server) return;
	if(!key) return;
	if(!value) return;

	i = 0;
	if(server->attributes)
	{
		for(i = 0; server->attributes[i].key; i++);
	}

	server->attributes = (ServerAttribute*)realloc(server->attributes,
		(i + 2) * sizeof(ServerAttribute));
	server->attributes[i].key = strdup(key);
	server->attributes[i].value = strdup(value);
	server->attributes[i + 1].key = NULL;
	server->attributes[i + 1].value = NULL;
}

const char *meta_server_findattribute(ServerEntry *server, const char *key)
{
	int i;

	if(!server) return NULL;
	if(!key) return NULL;

	if(server->attributes)
	{
		for(i = 0; server->attributes[i].key; i++)
		{
			if(!strcmp(server->attributes[i].key, key))
				return server->attributes[i].value;
		}
	}

	return NULL;
}

void meta_server_free(ServerEntry *server)
{
	int i;

	if(!server) return;

	if(server->attributes)
	{
		for(i = 0; server->attributes[i].key; i++)
		{
			free(server->attributes[i].key);
			free(server->attributes[i].value);
		}
	}

	if(server->attributes) free(server->attributes);

	ggz_uri_free(server->uri);

	free(server);
}

ServerEntry **meta_query(const char *metaserveruri, const char *classname,
	const char *category, ServerEntry *restriction)
{
	int fd;
	ServerEntry **list = NULL;
	char query[1024];
	char *s;
	DOM *dom;
	ELE **el;
	int i, j;

	ggz_uri_t uri;
	char options[512];
	char oldoptions[512];
	ServerEntry *server;
	ELE **properties;
	char *uriattr;
	char *name;
	char *value;

	uri = ggz_uri_from_string(metaserveruri);
	if(!uri.port) uri.port = 15689;
	fd = meta_connect_internal(uri.host, uri.port);
	ggz_uri_free(uri);

	if(fd >= 0)
	{
		strcpy(options, "");
		if(restriction->attributes)
		{
			for(i = 0; restriction->attributes[i].key; i++)
			{
				snprintf(oldoptions, sizeof(oldoptions),
					"%s",
					options);
				snprintf(options, sizeof(options),
					"%s"
					"<option name=\"%s\">%s</option>",
					oldoptions,
					restriction->attributes[i].key,
					restriction->attributes[i].value);
			}
		}

		snprintf(query, sizeof(query),
			"<?xml version=\"1.0\"?>"
			"<query class=\"%s\" type=\"%s\">"
			"%s"
			"</query>\n",
			classname, category, options);

		s = meta_query_internal(fd, query);
		if(s)
		{
			dom = minidom_parse(s);
			if((dom) && (dom->processed) && (dom->valid) && (dom->el))
			{
				el = dom->el->el;
				for(i = 0; ((el) && (el[i])); i++)
				{
					uriattr = MD_att(el[i], "uri");
					if(!uriattr)
					{
						continue;
					}

					uri = ggz_uri_from_string(uriattr);

					server = meta_server_new(uri);

					properties = el[i]->el;
					for(j = 0; ((properties) && (properties[j])); j++)
					{
						name = MD_att(properties[j], "name");
						value = properties[j]->value;

						meta_server_attribute(server, name, value);
					}

					list = meta_list_server(list, server);
				}
			}
			minidom_free(dom);
		}
	}

	return list;
}

ServerEntry **meta_list_server(ServerEntry **list, ServerEntry *server)
{
	int count = 0;
	char *uristr, *uristr2;
	int skip;

	if(!server) return list;

	if(list)
	{
		uristr = ggz_uri_to_string(server->uri);
		skip = 0;

		for(count = 0; list[count]; count++)
		{
			uristr2 = ggz_uri_to_string(list[count]->uri);
			if(!strcmp(uristr, uristr2)) skip = 1;
			free(uristr2);
		}

		free(uristr);

		if(skip)
		{
			/* FIXME: leak! */
			/* old entry should be free'd, new one copied there instead */
			/* FIXME 2: semantic equivalence (e.g. :15689 is not important) */
			return list;
		}
	}

	list = (ServerEntry**)realloc(list, (count + 2) * sizeof(ServerEntry*));
	list[count] = server;
	list[count + 1] = NULL;

	/* FIXME: take ownership (as is now) or better make a copy? */

	return list;
}

void meta_list_free(ServerEntry **list)
{
	int i;

	if(!list) return;

	for(i = 0; list[i]; i++)
	{
		meta_server_free(list[i]);
	}

	free(list);
}

ServerEntry **meta_queryall(ServerEntry **servers, const char *classname,
	const char *category, ServerEntry *restriction)
{
	int i, j;
	ServerEntry **list = NULL;
	ServerEntry **result;
	char *uristr;

	if(!servers) return NULL;

	for(i = 0; servers[i]; i++)
	{
		uristr = ggz_uri_to_string(servers[i]->uri);
		result = meta_query(uristr, classname, category, restriction);
		free(uristr);
		if(result)
		{
			for(j = 0; result[j]; j++)
			{
				list = meta_list_server(list, result[j]);
			}
	
			free(result);
		}
	}

	return list;
}

ServerEntry **meta_network_sync(ServerEntry **servers)
{
	ServerEntry **list = NULL;
	ServerEntry *restriction;
	const char *classname = "ggz";
	const char *category = "meta";
	const char *version = "0.5";
	ggz_uri_t uri;
	int i;

	if(!servers) return NULL;

	restriction = meta_server_new(uri);
	meta_server_attribute(restriction, "version", version);

	list = meta_queryall(servers, classname, category, restriction);

	meta_server_free(restriction);

	/*meta_list_free(servers);
	servers = list;*/

	for(i = 0; servers[i]; i++)
	{
		list = meta_list_server(list, servers[i]);
	}
	free(servers);

	return list;
}

ServerEntry **meta_queryallggz(ServerEntry **servers, const char *protocol)
{
	ServerEntry *restriction;
	const char *classname = "ggz";
	const char *category = "connection";
	ggz_uri_t uri;

	restriction = meta_server_new(uri);
	meta_server_attribute(restriction, "protocol", protocol);

	return meta_queryall(servers, classname, category, restriction);
}

/* === Internal functions === */
/* ========================== */

static char *meta_query_internal(int fd, const char *text)
{
	char buffer[1024];
	char *ret;
	int ret2;

	write(fd, text, strlen(text) + 1);
	ret2 = read(fd, buffer, sizeof(buffer));
	close(fd);

	if(ret2 > 0)
	{
		buffer[ret2 - 1] = 0;
		ret = strdup(buffer);
	}
	else ret = NULL;

	return ret;
}

static int meta_connect_internal(const char *hostname, int port)
{
	int fd, ret;
	struct sockaddr_in sa;
	struct hostent *name;

	if(!hostname) return -1;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0) return -1;

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);

	name = gethostbyname(hostname);
	if(!name) return -1;
	memcpy(&sa.sin_addr, name->h_addr, name->h_length);
	ret = connect(fd, (const struct sockaddr*)&sa, sizeof(sa));
	if(ret) return -1;

	return fd;
}

