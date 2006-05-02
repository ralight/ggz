/*
 * TelGGZ - The GGZ Gaming Zone Telnet Wrapper
 * Copyright (C) 2001 - 2003 Josef Spillner, dr_maux@users.sourceforge.net

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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

/* Definitions */
#define PROTO_TCP 6

/* Global variables */
char **metaservers;
int metaservercount;

/* Prototypes */
static char *meta_query_internal(int fd, const char *text);
static int meta_connect_internal(const char *hostname, int port);

/* Public functions */

/*
void meta_init()
{
	FILE *f;
	char buf[1024];

	metaservers = NULL;
	metaservercount = 0;

	strcpy(buf, getenv("HOME"));
	strcat(buf, "/.ggz/metaserver.cache");

	f = fopen(buf, "r");
	if(f)
	{
		while(fgets(buf, sizeof(buf), f))
		{
			buf[strlen(buf) - 1] = 0;
			if(strlen(buf))
			{
				metaservers = (char**)realloc(metaservers, (metaservercount + 1) * sizeof(char*));
				metaservers[metaservercount] = strdup(buf);
				metaservercount++;
			}
		}
		fclose(f);
	}
}
*/

/*
ServerGGZ **meta_query(const char *version)
{
	int fd;
	ServerGGZ **list = NULL;
	int listcount = 0;
	char query[1024];
	char *s;
	DOM *dom;
	ELE **el, *tmp;
	char *uri;
	int i, j;
	int finished;
	char *host;
	int port;
	ELE *speedstr, *locationstr;
	char *preferencestr;
	int speed, preference;
	char *location;

	finished = 0;
	j = 0;
	while((!finished) && (j < metaservercount))
	{
		host = meta_uri_host_internal(metaservers[j]);
		port = meta_uri_port_internal(metaservers[j]);
		fd = meta_connect_internal(host, port);
		if(fd >= 0)
		{
			sprintf(query, "<?xml version=\"1.0\"?><query class=\"ggz\" type=\"connection\">%s</query>\n", version);
			s = meta_query_internal(fd, query);
			if(s)
			{
				dom = minidom_parse(s);
				if((dom) && (dom->processed) && (dom->valid) && (dom->el))
				{
					el = dom->el->el;
					i = 0;
					while((el) && (el[i]))
					{
						tmp = MD_query(el[i], "uri");
						speedstr = MD_query(el[i], "speed");
						locationstr = MD_query(el[i], "location");
						preferencestr = MD_att(el[i], "preference");
						if(!speedstr) speed = 0;
						else speed = atoi(speedstr->value);
						if(!preferencestr) preference = 99;
						else preference = atoi(preferencestr);
						if(!locationstr) location = "unknown";
						else location = locationstr->value;
						if(tmp)
						{
							uri = tmp->value;
							list = (ServerGGZ**)realloc(list, (listcount + 2) * sizeof(ServerGGZ));
							list[listcount] = (ServerGGZ*)malloc(sizeof(ServerGGZ));
							list[listcount]->host = meta_uri_host_internal(uri);
							list[listcount]->port = meta_uri_port_internal(uri);
							list[listcount]->version = strdup(version);
							list[listcount]->speed = speed;
							list[listcount]->location = strdup(location);
							list[listcount]->preference = preference;
							list[listcount]->id = listcount + 1;
							list[listcount + 1] = NULL;
							listcount++;
						}
						i++;
					}
					finished = 1;
				}
				minidom_free(dom);
			}
		}
		j++;
	}

	return list;
}
*/

/*
void meta_sync()
{
	FILE *f;
	int fd;
	char query[1024];
	char *s;
	DOM *dom;
	ELE **el, *tmp;
	char *uri;
	int i, j, k;
	int finished;
	char *host;
	int port;

	finished = 0;
	j = 0;
	while((!finished) && (j < metaservercount))
	{
		host = meta_uri_host_internal(metaservers[j]);
		port = meta_uri_port_internal(metaservers[j]);
		fd = meta_connect_internal(host, port);
		if(fd >= 0)
		{
			sprintf(query, "<?xml version=\"1.0\"?><query class=\"ggz\" type=\"meta\">0.1</query>\n");
			s = meta_query_internal(fd, query);
			if(s)
			{
				dom = minidom_parse(s);
				if((dom) && (dom->processed) && (dom->valid))
				{
					el = dom->el->el;
					i = 0;
					while((el) && (el[i]))
					{
						tmp = MD_query(el[i], "uri");
						if(tmp)
						{
							uri = tmp->value;
							for(k = 0; k < metaservercount; k++)
								if(!strcmp(uri, metaservers[k]))
								{
									uri = NULL;
									break;
								}
							if(uri)
							{
								metaservers = (char**)realloc(metaservers, (metaservercount + 1) * sizeof(char*));
								metaservers[metaservercount] = strdup(uri);
								metaservercount++;
							}
						}
						i++;
					}
					finished = 1;
				}
				minidom_free(dom);
			}
		}
		j++;
	}

	strcpy(query, getenv("HOME"));
	strcat(query, "/.ggz/metaserver.cache");

	f = fopen(query, "w");
	if(f)
	{
		for(k = 0; k < metaservercount; k++)
		{
			fprintf(f, "%s\n", metaservers[k]);
		}
		fclose(f);
	}
}
*/

/*
void meta_free(ServerGGZ **server)
{
	int i;

	if(server)
	{
		i = 0;
		while(server[i])
		{
			if(server[i]->location) free(server[i]->location);
			if(server[i]->host) free(server[i]->host);
			if(server[i]->version) free(server[i]->version);
			free(server[i]);
			i++;
		}
		free(server);
	}
}
*/

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
	URI uri;
	char *serveruri;
	int accepted;
	int i;

	accepted = 0;

	uri = uri_from_string(metaserveruri);
	if(!uri.port) uri.port = 15689;
	fd = meta_connect_internal(uri.host, uri.port);
	uri_free(uri);

	if(fd >= 0)
	{
		serveruri = uri_to_string(server->uri);
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

/* ------------------------------------ */
/* Server entry data handling functions */
/* ------------------------------------ */

ServerEntry *meta_server_new(URI uri)
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

	free(server);
}

/* ------------------ */
/* Internal functions */
/* ------------------ */

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

	fd = socket(AF_INET, SOCK_STREAM, PROTO_TCP);
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

