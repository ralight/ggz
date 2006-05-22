/*
 * The GGZ Gaming Zone Metaserver Project
 * Copyright (C) 2001 - 2006 Josef Spillner <josef@ggzgamingzone.org>
 * Published under GNU GPL conditions.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_SOCKET_H
#  include <socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#  include <netdb.h>
#endif
#ifdef HAVE_WINSOCK_H
#  include <winsock.h>
#endif
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>

#include "minidom.h"
#include "uri.h"
#include "meta.h"

#include "helper.h"

#include "config.h"

/* Version number */
#define METASERV_VERSION "0.5"

/* The directory where metaservconf.xml resides */
#define METASERV_DIR PREFIX "/share/ggzmetaserv"

/* Optimized code is much faster, but not so readable */
#define METASERV_OPTIMIZED 1

/* Server port */
#define METASERV_PORT 15689

/* Global variables */
static DOM *configuration = NULL;
char *logfile = NULL;
static char *cachefile = NULL;
static int daemonmode = 0;
static int daemonport = 0;
static int verbosity = 0;
static pthread_mutex_t mutex;

static void metaserv_cache()
{
	char tmp[1024];

	if(cachefile)
	{
		logline("Caching configuration to %s", cachefile);
		snprintf(tmp, sizeof(tmp), "%s-%li.tmp", cachefile, time(NULL));
		unlink(tmp);
		minidom_dumpfile(configuration, tmp);
		rename(tmp, cachefile);
		chmod(cachefile, S_IRUSR | S_IWUSR);
		/*minidom_dumpfile(configuration, cachefile);*/
	}
}

static char *metaserv_list()
{
	const char *header = "<?xml version=\"1.0\"?><resultset referer=\"list\">";
	const char *footer = "</resultset>";
	char *xmlret = NULL;
	char *class;

	char *ret;
	int i, j;

	xmlret = (char*)malloc(strlen(header) + 1);
	strcpy(xmlret, header);

	j = 0;
	while((configuration->el->el) && (configuration->el->el[j]))
	{
#ifdef METASERV_OPTIMIZED
		i = 0;
		class = NULL;
		while((configuration->el->el[j]->at) && (configuration->el->el[j]->at[i]))
		{
			if(!strcmp(configuration->el->el[j]->at[i]->name, "class"))
			{
				class = configuration->el->el[j]->at[i]->value;
				break;
			}
			i++;
		}
#else
		class = MD_att(configuration->el->el[j], "class");
#endif
		if(!class)
		{
			j++;
			continue;
		}

		xmlret = (char*)realloc(xmlret, strlen(xmlret) + strlen(class) + 17);
		strcat(xmlret, "<class>");
		strcat(xmlret, class);
		strcat(xmlret, "</class>");
		/*strcat(xmlret, "\n");*/

		j++;
	}

	xmlret = (char*)realloc(xmlret, strlen(xmlret) + strlen(footer) + 1);
	strcat(xmlret, footer);
	ret = xmlret;

	return ret;
}

static char *metaserv_lookup_class(ELE **ele, const char *category, const char *delta, ELE **key, int xmlformat, const char *class)
{
	char *xmlret = NULL;
	char *uriret = NULL;
	char *ret, *r;
	int i, j;
	ATT **att;
	char *tmpatt;
	char tmp[1024];
	char *lastupdate;
	int length;
	int skip;
	char classstr[128];

	ret = NULL;
	r = NULL;

	if(xmlformat)
	{
		xmlret = (char*)malloc(1);
		strcpy(xmlret, "");
	}
	else
	{
		uriret = (char*)malloc(1);
		strcpy(uriret, "");
	}

	i = 0;
	while((ele) && (ele[i]))
	{
		r = NULL;
		if(!strcmp(ele[i]->name, category))
		{
			/* Filter out entries which are too new */
			if(delta)
			{
				lastupdate = MD_att(ele[i], "ggzmeta:update");
				if((lastupdate) && (atoi(lastupdate) < atoi(delta)))
				{
					i++;
					continue;
				}
			}

			/* If search key is available, filter out all non-matching entries */
			if(key)
			{
				skip = 0;
				for(j = 0; key[j]; j++)
				{
					tmpatt = MD_att(key[j], "name");
					if(tmpatt) tmpatt = MD_att(ele[i], tmpatt);
					if(tmpatt)
					{
						if(strcmp(tmpatt, key[j]->value)) skip = 1;
					}
				}
				if(skip)
				{
					i++;
					continue;
				}
			}

			r = ele[i]->value;
			if(!xmlformat)
			{
				uriret = (char*)realloc(uriret, strlen(uriret) + strlen(r) + 2);
				strcat(uriret, r);
				strcat(uriret, "\n");
			}
			else
			{
				strcpy(classstr, "");
				if(class)
				{
					snprintf(classstr, sizeof(classstr), " class=\"%s\"", class);
				}
				snprintf(tmp, sizeof(tmp), "<result uri=\"%s\"%s>", r, classstr);
				xmlret = (char*)realloc(xmlret, strlen(xmlret) + strlen(tmp) + 1);
				strcat(xmlret, tmp);

				j = 0;
				att = ele[i]->at;
				while((att) && (att[j]))
				{
					if(!strncmp(att[j]->name, "ggzmeta:", 8))
					{
						j++;
						continue;
					}
					if(att[j]->value)
					{
						length = strlen(xmlret) + strlen(att[j]->name);
						length += strlen(att[j]->value) + 25;
						xmlret = (char*)realloc(xmlret, length);
						snprintf(tmp, sizeof(tmp), "<option name=\"%s\">%s</option>",
							att[j]->name, att[j]->value);
						strcat(xmlret, tmp);
					}
					else logline("ALERT: key=%s without value", att[j]->name);
					j++;
				}

				snprintf(tmp, sizeof(tmp), "</result>");
				xmlret = (char*)realloc(xmlret, strlen(xmlret) + strlen(tmp) + 1);
				strcat(xmlret, tmp);
			}
		}
		i++;
	}

	if(xmlformat)
	{
		ret = xmlret;
	}
	else
	{
		ret = uriret;
	}

	return ret;
}

static char *metaserv_lookup(const char *class, const char *category, const char *delta, ELE **key, int xmlformat)
{
	const char *header = "<?xml version=\"1.0\"?><resultset referer=\"query\">";
	const char *footer = "</resultset>";
	char *xmlret = NULL;
	char *ret, *r;
	int i, j, valid;
	ELE **ele;
	char *attr;

	if((!class) || (!category)) return NULL;

	valid = 0;
	ret = NULL;
	r = NULL;
	ele = NULL;

	srand(time(NULL));

	if(xmlformat)
	{
		ret = (char*)malloc(strlen(header) + 1);
		strcpy(ret, header);
	}

	j = 0;
	while((configuration->el->el) && (configuration->el->el[j]) && (!valid))
	{
#ifdef METASERV_OPTIMIZED
		i = 0;
		while((configuration->el->el[j]->at) && (configuration->el->el[j]->at[i]))
		{
			if((!strcmp(configuration->el->el[j]->at[i]->name, "class"))
			&& (!strcmp(configuration->el->el[j]->at[i]->value, class)))
			{
				valid = 1;
				ele = configuration->el->el[j]->el;
			}
			i++;
		}
#else
		valid = !strcmp(MD_att(configuration->el->el[j], "class"), class);
		if(valid) ele = configuration->el->el[j]->el;
#endif
		j++;
	}
	if(!valid)
	{
		if(strcmp(class, "*")) return NULL;
	}

	if(!strcmp(class, "*"))
	{
		j = 0;
		while((configuration->el->el) && (configuration->el->el[j]))
		{
			attr = MD_att(configuration->el->el[j], "class");
			if(attr)
			{
				ele = configuration->el->el[j]->el;
				xmlret = metaserv_lookup_class(ele, category, delta, key, xmlformat, attr);
				ret = (char*)realloc(ret, strlen(ret) + strlen(xmlret) + 1);
				strcat(ret, xmlret);
				free(xmlret);
			}
			j++;
		}
	}
	else
	{
		xmlret = metaserv_lookup_class(ele, category, delta, key, xmlformat, NULL);
		if(xmlformat)
		{
				ret = (char*)realloc(ret, strlen(ret) + strlen(xmlret) + 1);
				strcat(ret, xmlret);
				free(xmlret);
		}
		else ret = xmlret;
	}

	if(xmlformat)
	{
		ret = (char*)realloc(ret, strlen(ret) + strlen(footer) + 1);
		strcat(ret, footer);
	}

	return ret;
}

static int metaserv_auth(const char *username, const char *password, const char *capability, const char *realm)
{
	ELE *ele;
#ifdef METASERV_OPTIMIZED
	ATT **att;
	int j;
#endif
	int i;
	char *username2, *password2, *capability2, *realm2;

	/*ele = configuration->el;*/
	ele = MD_query(configuration->el, "configuration");
	i = 0;
	while((ele) && (ele->el) && (ele->el[i]))
	{
		if(!strcmp(ele->el[i]->name, "authorization"))
		{
#ifdef METASERV_OPTIMIZED
			att = ele->el[i]->at;
			username2 = NULL;
			password2 = NULL;
			capability2 = NULL;
			realm2 = NULL;
			j = 0;
			while((att) && (att[j]))
			{
				if(!strcmp(att[j]->name, "username")) username2 = att[j]->value;
				if(!strcmp(att[j]->name, "password")) password2 = att[j]->value;
				if(!strcmp(att[j]->name, capability)) capability2 = att[j]->value;
				if(!strcmp(att[j]->name, "realm")) realm2 = att[j]->value;

				if((!capability2) && (!strcmp(capability, "update")))
					if(!strcmp(att[j]->name, "create")) capability2 = att[j]->value;
				j++;
			}
#else
			username2 = MD_att(ele->el[i], "username");
			password2 = MD_att(ele->el[i], "password");
			capability2 = MD_att(ele->el[i], capability);
			if((!capability2) && (!strcmp(capability, "update")))
				capability2 = MD_att(ele->el[i], "create");
			realm2 = MD_att(ele->el[i], "realm");
#endif
			if(!strcmp(realm2, "*")) realm = realm2;

			if((username2) && (password2) && (capability2) && (realm2))
			{
				if((!strcmp(username, username2))
				&& (!strcmp(password, password2))
				&& (!strcmp("1", capability2))
				&& (!strcmp(realm, realm2)))
				{
					return 1;
				}
			}
		}
		i++;
	}

	return 0;
}

static int metaserv_notify(const char *username, const char *password, const char *uristring, ELE *ele)
{
	int fd, ret;
	struct sockaddr_in sa;
	struct hostent *name;
	char *hostname;
	int port;
	const char *text = "Hello World.\n";
	ssize_t bytes;
	URI uri;

	logline("Notify: peer=%s username=%s password=%s", uristring, username, password);

	uri = uri_from_string(uristring);

	/* FIXME: is never freed; move notification to libmeta */
	hostname = uri.host;
	port = uri.port;

	fd = socket(AF_INET, SOCK_STREAM, 6);
	if(fd < 0)
	{
		logline("Notify: failed (socket)");
		return 0;
	}

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);

	name = gethostbyname(hostname);
	if(!name)
	{
		logline("Notify: failed (gethostbyname)");
		close(fd);
		return 0;
	}

	memcpy(&sa.sin_addr, name->h_addr, name->h_length);
	ret = connect(fd, (const struct sockaddr*)&sa, sizeof(sa));
	if(ret)
	{
		logline("Notify: failed (connect)");
		close(fd);
		return 0;
	}

	/* Text: Use ELE! */

	bytes = write(fd, text, strlen(text) + 1);
	if(bytes < (int)(strlen(text) + 1))
	{
		logline("Notify: failed (write)");
		close(fd);
		return 0;
	}

	logline("Notify: succeeded");

	close(fd);
	return 1;
}

static void metaserv_peers(ELE *ele)
{
	int i, j;
	ELE *el2, *el;
	char *username, *password, *uri;

	logline("Update: notify peers");

#ifdef METASERV_OPTIMIZED
	el2 = NULL;
	i = 0;
	while((configuration->el) && (configuration->el->el) && (configuration->el->el[i]))
	{
		if(!strcmp(configuration->el->el[i]->name, "configuration")) el2 = configuration->el->el[i];
		i++;
	}
#else
	el2 = MD_query(configuration->el, "configuration");
#endif

	i = 0;
	while((el2) && (el2->el) && (el2->el[i]))
	{
		el = el2->el[i];
		uri = el->value;

		/*printf("Got peer: %s\n", uri);*/
#ifdef METASERV_OPTIMIZED
		username = NULL;
		password = NULL;
		j = 0;
		while((el->at) && (el->at[j]))
		{
			if(!strcmp(el->at[j]->name, "username")) username = el->at[j]->value;
			if(!strcmp(el->at[j]->name, "password")) password = el->at[j]->value;
			j++;
		}
#else
		username = MD_att(el, "username");
		password = MD_att(el, "password");
#endif
		if((uri) && (username) && (password))
		{
			/*printf("==> notify: %s:%s@%s\n", username, password, uri);*/
			metaserv_notify(username, password, uri, ele);
		}

		i++;
	}
}

static char *metaserv_update(const char *class, const char *category, const char *username, const char *password, const char *uri, ATT **att, int atnum, const char *mode)
{
	const char *header = "<?xml version=\"1.0\"?><resultset referer=\"update\">";
	const char *footer = "</resultset>";
	char *xmlret = NULL;
	char *ret;
	char tmp[1024];
	char *status;
	ELE *ele, *ele2, *ele3;
	char *att2;
	ATT **att3;
	int i;
#ifdef METASERV_OPTIMIZED
	int j;
#endif

	logline("Update: class=%s, category=%s, username=%s, password=%s, uri=%s, mode=%s",
		class, category, username, password, uri, mode);

	ret = NULL;
	ele2 = NULL;
	att2 = NULL;

	status = "accepted";

	if((!class) || (!category) || (!username) || (!password) || (!uri) || (!att) || (!atnum) || (!mode))
	{
		logline("Update: Missing arguments");
		status = "wrong";
	}

	if(!metaserv_auth(username, password, "update", class))
	{
		status = "rejected";
	}
	else if(!daemonmode)
	{
		status = "readonly";
	}
	else if(!strcmp(status, "accepted"))
	{
		i = 0;
		while((configuration->el->el) && (configuration->el->el[i]) && (!att2))
		{
			if(!strcmp(configuration->el->el[i]->name, "entrylist"))
			{
				ele2 = configuration->el->el[i];
#ifdef METASERV_OPTIMIZED
				att2 = NULL;
				j = 0;
				while((ele2->at) && (ele2->at[j]))
				{
					if(!strcmp(ele2->at[j]->name, "class") && (!strcmp(ele2->at[j]->value, class)))
						att2 = ele2->at[j]->value;
					j++;
				}
#else
				att2 = MD_att(ele2, "class");
				if(strcmp(ele2->at[j]->value, class)) att2 = NULL;
#endif
			}
			i++;
		}

		pthread_mutex_lock(&mutex);

		if(!att2)
		{
			if(metaserv_auth(username, password, "create", class))
			{
				// CREATE new class!
				// <entrylist class=''>

				/* Create attributes first */
				att3 = (ATT**)malloc(sizeof(ATT*) * 2);
				att3[0] = (ATT*)malloc(sizeof(ATT));
				att3[0]->name = strdup("class");
				att3[0]->value = xml_strdup(class, 1, 0);
				att3[1] = NULL;

				/* create new XML element */
				ele = (ELE*)malloc(sizeof(ELE));
				ele->name = strdup("entrylist");
				ele->el = NULL;
				ele->parent = configuration->el;
				ele->at = att3;
				ele->elnum = 0;
				ele->atnum = 1;
				ele->value = NULL;

				/* add updated data to the meta server */
				ele2 = configuration->el;
				ele2->elnum++;
				ele2->el = (ELE**)realloc(ele2->el, (ele2->elnum + 1) * sizeof(ELE*));
				ele2->el[ele2->elnum - 1] = ele;
				ele2->el[ele2->elnum] = NULL;

				/* now we've got the needed attribute */
				att2 = (char*)class;
				ele2 = ele;
			}
		}

		if(att2)
		{
			if(!strcmp(mode, "add"))
			{
				/* create XML element from update data */
				ele = (ELE*)malloc(sizeof(ELE));
				ele->name = strdup(category);
				ele->el = NULL;
				ele->parent = ele2;
				ele->at = att;
				ele->elnum = 0;
				ele->atnum = atnum;
				ele->value = xml_strdup(uri, 0, 1);

				/* add updated data to the meta server */
				ele2->elnum++;
				ele2->el = (ELE**)realloc(ele2->el, (ele2->elnum + 1) * sizeof(ELE*));
				ele2->el[ele2->elnum - 1] = ele;
				ele2->el[ele2->elnum] = NULL;

				metaserv_peers(ele);

				/* dump new configuration */
				metaserv_cache();
				/*minidom_dump(configuration);*/
			}
			else if(!strcmp(mode, "modify"))
			{
			}
			else if(!strcmp(mode, "delete"))
			{
				/* remove XML element */
				i = 0;
				while((ele2->el) && (ele2->el[i]))
				{
					ele3 = ele2->el[i];
					if(!strcmp(ele3->name, category))
					{
						if((ele3->value) && (!strcmp(ele3->value, uri)))
						{
							free(ele3);
							/*ele3 = NULL;*/
							ele2->el[i] = NULL;
							for(j = i + 1; ele2->el[j]; j++)
							{
								ele2->el[j - 1] = ele2->el[j];
								ele2->el[j] = NULL;
							}

							metaserv_peers(NULL);

							/* dump new configuration */
							metaserv_cache();
							/*minidom_dump(configuration);*/
							break;
						}
					}
					i++;
				}
			}
			else status = "wrong";
		}
		else
		{
			status = "wrong";
		}

		pthread_mutex_unlock(&mutex);
	}

	logline("Update: status=%s", status);

	xmlret = (char*)malloc(strlen(header) + 1);
	strcpy(xmlret, header);

	snprintf(tmp, sizeof(tmp), "<status>%s</status>", status);
	xmlret = (char*)realloc(xmlret, strlen(xmlret) + strlen(tmp) + 1);
	strcat(xmlret, tmp);

	xmlret = (char*)realloc(xmlret, strlen(xmlret) + strlen(footer) + 1);
	strcat(xmlret, footer);
	ret = xmlret;

	/*metaserv_peers();*/

	return ret;
}

static char *metaserv_xml(const char *uri)
{
	DOM *query;
	ATT **att;
	char *ret;
	char *class, *category, *delta;
	char *username, *password;
	char *uri2, *name;
	char *mode;
	int i, atnum;
#ifdef METASERV_OPTIMIZED
	int j;
#endif
	char tmp[128];

	ret = NULL;
	uri2 = NULL;
	mode = NULL;
	atnum = 0;

	if(verbosity == 2)
		logline("XML: input=%s", uri);

	query = minidom_parse(uri);

	if((!query) || (!query->valid) || (!query->processed) || (!query->el) || (!query->el->name))
	{
		logline("XML: Invalid XML");
		minidom_free(query);
		return NULL;
	}

	if(!strcmp(query->el->name, "query"))
	{
#ifdef METASERV_OPTIMIZED
		class = NULL;
		category = NULL;
		delta = NULL;
		i = 0;
		while((query->el->at) && (query->el->at[i]))
		{
			if(!strcmp(query->el->at[i]->name, "class")) class = query->el->at[i]->value;
			if(!strcmp(query->el->at[i]->name, "type")) category = query->el->at[i]->value;
			if(!strcmp(query->el->at[i]->name, "delta")) delta = query->el->at[i]->value;
			i++;
		}
#else
		class = MD_att(query->el, "class");
		category = MD_att(query->el, "type");
		delta = MD_att(query->el, "delta");
#endif
		ret = metaserv_lookup(class, category, delta, query->el->el, 1);
	}
	else if(!strcmp(query->el->name, "list"))
	{
		ret = metaserv_list();
	}
	else if(!strcmp(query->el->name, "update"))
	{
#ifdef METASERV_OPTIMIZED
		class = NULL;
		category = NULL;
		username = NULL;
		password = NULL;
		i = 0;
		while((query->el->at) && (query->el->at[i]))
		{
			if(!strcmp(query->el->at[i]->name, "class")) class = query->el->at[i]->value;
			if(!strcmp(query->el->at[i]->name, "type")) category = query->el->at[i]->value;
			if(!strcmp(query->el->at[i]->name, "username")) username = query->el->at[i]->value;
			if(!strcmp(query->el->at[i]->name, "password")) password = query->el->at[i]->value;
			if(!strcmp(query->el->at[i]->name, "uri")) uri2 = query->el->at[i]->value;
			if(!strcmp(query->el->at[i]->name, "mode")) mode = query->el->at[i]->value;
			i++;
		}
		i = 0;
#else
		class = MD_att(query->el, "class");
		category = MD_att(query->el, "type");
		username = MD_att(query->el, "username");
		password = MD_att(query->el, "password");
		uri2 = MD_att(query->el, "uri");
		mode = MD_att(query->el, "mode");
#endif
		att = NULL;
		i = 0;
		while((query->el->el) && (query->el->el[i]) && (query->el->el[i]->value))
		{
			if(!strcmp(query->el->el[i]->name, "option"))
			{
#ifdef METASERV_OPTIMIZED
				name = NULL;
				j = 0;
				while((query->el->el[i]->at) && (query->el->el[i]->at[j]))
				{
					if(!strcmp(query->el->el[i]->at[j]->name, "name")) name = query->el->el[i]->at[j]->value;
					j++;
				}
#else
				name = MD_att(query->el->el[i], "name");
#endif
				if(name)
				{
					atnum++;
					att = (ATT**)realloc(att, (atnum + 1) * sizeof(ATT*));
					att[atnum - 1] = (ATT*)malloc(sizeof(ATT));
					/*if(!strcmp(name, "version"))
						att[atnum - 1]->name = strdup("ggzmeta:version");
					else
						att[atnum - 1]->name = xml_strdup(name, 0, 0);*/
					att[atnum - 1]->name = xml_strdup(name, 1, 0);
					att[atnum - 1]->value = xml_strdup(query->el->el[i]->value, 1, 0);
					att[atnum] = NULL;
					/*printf("Got: %s/%s\n", name, query->el->el[i]->value);*/
				}
			}
			/*if(!strcmp(query->el->el[i]->name, "uri")) uri2 = query->el->el[i]->value;*/
			i++;
		}

		atnum++;
		att = (ATT**)realloc(att, (atnum + 1) * sizeof(ATT*));
		att[atnum - 1] = (ATT*)malloc(sizeof(ATT));
		att[atnum - 1]->name = strdup("ggzmeta:update");
		snprintf(tmp, sizeof(tmp), "%li", time(NULL) + 7200); /* 2 hours tolerance */
		att[atnum - 1]->value = strdup(tmp);
		att[atnum] = NULL;

		ret = metaserv_update(class, category, username, password, uri2, att, atnum, mode);
	}
	else
	{
		logline("XML: Unknown type of operation: name=%s", query->el->name);
	}

	minidom_free(query);

	return ret;
}

static char *metaserv_uri(char *requri)
{
	char *ret;
	char *token;
	char *category, *class;

	ret = NULL;

	/* query://ggz/connection/0.0.5pre */
	token = strtok(requri, ":/");
	/* query ggz connection 0.0.5pre */
	if(!token) return NULL;
	if(!strcmp(token, "query"))
	{
		token = strtok(NULL, ":/");
		/* ggz connection 0.0.5pre */
		if(!token) return NULL;
		class = strdup(token);
		token = strtok(NULL, ":/");
		/* connection 0.0.5pre */
		if(!token) return NULL;
		category = strdup(token);
		token = strtok(NULL, ":/");
		/* 0.0.5pre */
		if(token) ret = metaserv_lookup(class, category, NULL, NULL, 0);
		else ret = metaserv_lookup(class, category, NULL, NULL, 0);
		free(category);
		free(class);
	}
	else
	{
		if(verbosity)
			logline("Unknown query type in %s", requri);
	}

	while(token) token = strtok(NULL, ":/");

	return ret;
}

static char *metamagic(const char *rawuri)
{
	char *ret;
	char *uri;

	if(!rawuri) return NULL;

	uri = strdup(rawuri);

	if((strlen(uri) > 5) && (!strncmp(uri, "<?xml ", 5)))
	{
		ret = metaserv_xml(uri);
		free(uri);
		return ret;
	}

	if(uri[strlen(uri) - 1] == '\n') uri[strlen(uri) - 1] = 0;
	if(uri[strlen(uri) - 1] == '\r') uri[strlen(uri) - 1] = 0;

	ret = metaserv_uri(uri);

	free(uri);

	return ret;
}

static void metaserv_init(const char *configfile)
{
	logline("Initialization");
	if(cachefile)
	{
		configuration = minidom_load(cachefile);
		if(configuration) logline("Using cachefile: %s", cachefile);
		else logline("Cache not present, will be created");
	}
	if(!configuration)
	{
		if(!configfile) configfile = METASERV_DIR "/metaservconf.xml";
		logline("Using configuration: %s", configfile);
		configuration = minidom_load(configfile);
		if((!configuration)
		|| (!configuration->el)
		|| (!configuration->valid)
		|| (!configuration->processed))
		{
			logline("Configuration invalid!");
			exit(-1);
		}
	}
}

static void metaserv_shutdown()
{
	logline("Shutdown");
	minidom_free(configuration);
}

static int metaserv_work(int fd, int session)
{
	char buffer[1024];
	char *result;
	int ret;
	FILE *stream;

	logline("[%i] Enter main loop", session);
	while(1)
	{
		ret = read(fd, buffer, sizeof(buffer));
		if(ret > 0)
		{
			buffer[ret - 1] = 0;
			if(verbosity == 2)
				logline("[%i] Request: buffer=%s", session, buffer);
			else
				logline("[%i] Got request", session);
			result = metamagic(buffer);

			stream = fdopen(fd, "w");
			if(stream)
			{
				if(result)
				{
					fprintf(stream, "%s\n", result);
					if(verbosity == 2)
						logline("[%i] Result: result=%s", session, result);
					else
						logline("[%i] Sent result", session);
				}
				else
				{
					fprintf(stream, "\n");
					logline("[%i] No result", session);
				}
				fflush(stream);
				/*fclose(stream);*/
			}
			else logline("[%i] Broken pipe", session);

			free(result);
		}
		else return 0;
	}

	return 1;
}

static void *metaserv_worker(void *arg)
{
	int fd;
	struct sockaddr_in peername;
	socklen_t namelen;
	int ret;
	struct hostent host, *hp;
	int herr;
	const char *peer;
	static long session = 0;
	int tmpsession;
	char tmp[1024];

	fd = *(int*)arg;
	free(arg);

	namelen = sizeof(struct sockaddr_in);
	getpeername(fd, (struct sockaddr*)&peername, &namelen);
	ret = gethostbyaddr_r(&peername.sin_addr, sizeof(struct in_addr), AF_INET,
		&host, tmp, sizeof(tmp), &hp, &herr);
	if(!ret) peer = host.h_name;
	else peer = "(unknown)";

	tmpsession = session++;
	logline("Accepted connection from %s as [%i]", peer, tmpsession);
	metaserv_work(fd, tmpsession);
	logline("Disconnection from %s as [%i]", peer, tmpsession);
	close(fd);

	return NULL;
}

static void metaserv_daemon()
{
	int sock;
	struct sockaddr_in name;
	int on;
	int ret;
	int fd;
	pthread_t *thread;
	int *arg;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock < 0) return;

	name.sin_family = AF_INET;
	name.sin_port = htons(daemonport);
	name.sin_addr.s_addr = htonl(INADDR_ANY);

	on = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*)&on, sizeof(on));

	ret = bind(sock, (struct sockaddr*)&name, sizeof(name));
	if(ret < 0)
	{
		fprintf(stderr, "Couldn't bind to port, maybe already running?\n");
		return;
	}

	ret = listen(sock, 1);
	if(ret < 0) return;

	pthread_mutex_init(&mutex, NULL);

	signal(SIGPIPE, SIG_IGN);

	while(1)
	{
		fd = accept(sock, NULL, NULL);
		if(fd < 0) continue;

		arg = (int*)malloc(sizeof(int));
		*arg = fd;
		thread = (pthread_t*)malloc(sizeof(pthread_t));
		ret = pthread_create(thread, NULL, metaserv_worker, arg);
		if(ret) close(fd);
		else pthread_detach(*thread);
	}
}

int main(int argc, char *argv[])
{
	int ret;
	char *config = NULL;
	int optindex;
	int opt;
	struct option options[] =
	{
		{"configuration", required_argument, 0, 'c'},
		{"cache", required_argument, 0, 'C'},
		{"daemon", no_argument, 0, 'd'},
		{"debug", no_argument, 0, 'D'},
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"logfile", required_argument, 0, 'l'},
		{"port", required_argument, 0, 'p'},
		{"verbose", no_argument, 0, 'V'},
		{0, 0, 0, 0}
	};

	while(1)
	{
		opt = getopt_long(argc, argv, "c:C:dDhl:p:vV", options, &optindex);
		if(opt == -1) break;
		switch(opt)
		{
			case 'c':
				config = optarg;
				break;
			case 'C':
				cachefile = optarg;
				break;
			case 'd':
				daemonmode = 1;
				break;
			case 'D':
				verbosity = 2;
				break;
			case 'h':
				printf("The GGZ Gaming Zone Meta Server\n");
				printf("Version %s (GGZ %s)\n", METASERV_VERSION, VERSION);
				printf("Copyright (C) 2001 - 2006 Josef Spillner <josef@ggzgamingzone.org>\n");
				printf("Published under GNU GPL conditions\n\n");
				printf("Available options:\n");
				printf("[-c | --configuration]: Use this configuration file\n");
				printf("[-C | --cache        ]: Use this cache file\n");
				printf("[-d | --daemon       ]: Run in daemon mode\n");
				printf("[-D | --debug        ]: Log all transmissions\n");
				printf("[-h | --help         ]: Show this help\n");
				printf("[-p | --port         ]: Use this port instead of %i\n", METASERV_PORT);
				printf("[-v | --version      ]: Display version number\n");
				printf("[-V | --verbose      ]: More verbose logging\n");
				printf("[-l | --logfile      ]: Log events into this file\n");
				exit(0);
				break;
			case 'v':
				printf("%s\n", METASERV_VERSION);
				exit(0);
				break;
			case 'V':
				if(verbosity == 0) verbosity = 1;
				break;
			case 'l':
				logfile = optarg;
				break;
			case 'p':
				daemonport = atoi(optarg);
				break;
			default:
				printf("Unknown option, try --help.\n");
				exit(-1);
		}
	}

	if(!daemonport) daemonport = METASERV_PORT;

	if((verbosity) && (!logfile)) logfile = "stdout";

	metaserv_init(config);

	if(!daemonmode)
	{
		ret = metaserv_work(STDIN_FILENO, 0);
		logline("Disconnection");
	}
	else metaserv_daemon();

	metaserv_shutdown();

	return 0;
}

