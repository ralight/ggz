/* Include files */

#include "meta.h"
#include "minidom.h"
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

/* Globals */

char **metaservers;

/* Prototypes */

char *meta_query_internal(int fd, const char *text);
int meta_connect_internal(const char *hostname, int port);
char *meta_uri_host_internal(const char *uri);
int meta_uri_port_internal(const char *uri);

/* Public functions */

void meta_init()
{
}

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
	int i;

	fd = meta_connect_internal("localhost", 15689);
	if(fd >= 0)
	{
		sprintf(query, "<?xml version=\"1.0\"?><query class=\"ggz\" type=\"connection\">%s</query>\n", version);
		s = meta_query_internal(fd, query);
		if(s)
		{
			printf(">> %s\n", s);
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
						list = (ServerGGZ**)realloc(list, (listcount + 2) * sizeof(ServerGGZ));
						list[listcount] = (ServerGGZ*)malloc(sizeof(ServerGGZ));
						list[listcount]->host = meta_uri_host_internal(uri);
						list[listcount]->port = meta_uri_port_internal(uri);
						list[listcount]->version = strdup(version);
						list[listcount]->speed = 0;
						list[listcount]->location = strdup("unknown");
						list[listcount]->preference = 100;
						list[listcount]->id = listcount + 1;
						list[listcount + 1] = NULL;
						listcount++;
					}
					i++;
				}
			}
			minidom_free(dom);
		}
	}

	return list;
}

void meta_sync()
{
}

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

/* Internal functions */

char *meta_query_internal(int fd, const char *text)
{
	char buffer[1024];
	char *ret;

	write(fd, text, strlen(text) + 1);
	read(fd, buffer, sizeof(buffer));
	close(fd);

	ret = strdup(buffer);
	return ret;
}

int meta_connect_internal(const char *hostname, int port)
{
	int fd, ret;
	struct sockaddr_in sa;
	struct hostent *name;

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

char *meta_uri_host_internal(const char *uri)
{
	char *s;

	s = strdup(uri);
	s = strtok(s, ":");
	s = strtok(NULL, ":");
	return s + 2;
}

int meta_uri_port_internal(const char *uri)
{
	char *s;

	s = strdup(uri);
	s = strtok(s, ":");
	s = strtok(NULL, ":");
	s = strtok(NULL, ":");
	return atoi(s);
}

