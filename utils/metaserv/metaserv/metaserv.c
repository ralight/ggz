/*
 * The GGZ Gaming Zone Metaserver Project
 * Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
 * Published under GNU GPL conditions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "minidom.h"
#include <getopt.h>
#include <stdarg.h>

#include "config.h"

/* The directory where metaservconf.xml resides */
#define METASERV_DIR PREFIX "/share/metaserv"

/* Optimized code is much faster, but not so readable */
#define METASERV_OPTIMIZED 1

/*
 URI:
 query://ggz/connection/0.0.5pre
 Answer:
 ggz://jzaun.com:5688

 XML:
 <?xml version="1.0"><query class="ggz" type="connection">0.0.5pre</query>
 Answer:
 <?xml version="1.0"><resultset><result preference="100"><uri>ggz://jzaun.com:5688</uri>..</result></resultset>
*/

static DOM *configuration = NULL;
static char *logfile = NULL;

static void log(const char *fmt, ...)
{
	FILE *f;
	time_t t;
	char *ct;
	va_list ap;

	if(!logfile) return;

	f = fopen(logfile, "a");
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
		fclose(f);
	}
}

static char *metaserv_lookup(const char *class, const char *category, const char *key, int xmlformat)
{
	const char *header = "<?xml version=\"1.0\"?><resultset referer=\"query\">";
	const char *footer = "</resultset>";
	static char *xmlret = NULL;
	char *ret, *r;
	int i, j, valid;
	ELE **ele;
	ATT **att;
	int preference, pref;
	char tmp[1024];

	if((!configuration)
	|| (!configuration->el)
	|| (!configuration->valid)
	|| (!configuration->processed)) return NULL;

	if((!class) || (!category) || (!key)) return NULL;

	/*att = configuration->el->at;*/
	valid = 0;
	preference = 0;
	ret = NULL;
	r = NULL;
	ele = NULL;

	srand(time(NULL));

	if(xmlret)
	{
		free(xmlret);
		xmlret = NULL;
	}
	if(xmlformat)
	{
		xmlret = (char*)malloc(strlen(header) + 1);
		strcpy(xmlret, header);
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
	if(!valid) return NULL;

	/*ele = configuration->el->el;*/
	i = 0;
	while((ele) && (ele[i]))
	{
		r = NULL;
		if(!strcmp(ele[i]->name, category))
		{
			att = ele[i]->at;
			j = 0;
			while((att) && (att[j]))
			{
				if((!strcmp(att[j]->name, "version"))
				&& (!strcmp(att[j]->value, key)))
				{
					r = ele[i]->value;
				}
				if((r) && (!strcmp(att[j]->name, "preference")))
				{
					pref = atoi(att[j]->value);
					if((pref == preference) && (rand() % 2)) ret = r;
					if(pref > preference)
					{
						preference = pref;
						ret = r;
					}
					if((xmlformat) && (r))
					{
						sprintf(tmp, "<result preference=\"%i\"><uri>%s</uri></result>", preference, r);
						xmlret = (char*)realloc(xmlret, strlen(xmlret) + strlen(tmp) + 1);
						strcat(xmlret, tmp);
					}
				}
				j++;
			}
		}
		i++;
	}

	if(xmlformat)
	{
		xmlret = (char*)realloc(xmlret, strlen(xmlret) + strlen(footer) + 1);
		strcat(xmlret, footer);
		ret = xmlret;
	}
	
	if(!ret) ret = r;
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
				j++;
			}
#else
			username2 = MD_att(ele->el[i], "username");
			password2 = MD_att(ele->el[i], "password");
			capability2 = MD_att(ele->el[i], capability);
			realm2 = MD_att(ele->el[i], "realm");
#endif
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

static char *meta_uri_host_internal(const char *uri)
{
	char *s;

	s = strdup(uri);
	s = strtok(s, ":");
	if(s)
	{
		s = strtok(NULL, ":");
		if(s) return s + 2;
	}
	return NULL;
}

static int meta_uri_port_internal(const char *uri)
{
	char *s;

	s = strdup(uri);
	s = strtok(s, ":");
	if(s)
	{
		s = strtok(NULL, ":");
		if(s)
		{
			s = strtok(NULL, ":");
			if(s) return atoi(s);
		}
	}
	return 0;
}

static int metaserv_notify(const char *username, const char *password, const char *uri, ELE *ele)
{
	int fd, ret;
	struct sockaddr_in sa;
	struct hostent *name;
	char *hostname;
	int port;
	const char *text = "Hello World.\n";

	log("Notify: peer=%s username=%s password=%s", uri, username, password);

	hostname = meta_uri_host_internal(uri);
	port = meta_uri_port_internal(uri);

	fd = socket(AF_INET, SOCK_STREAM, /*PROTO_TCP*/6);
	if(fd < 0)
	{
		log("Notify: failed (socket)");
		return 0;
	}

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);

	name = gethostbyname(hostname);
	if(!name)
	{
		log("Notify: failed (gethostbyname)");
		close(fd);
		return 0;
	}

	memcpy(&sa.sin_addr, name->h_addr, name->h_length);
	ret = connect(fd, (const struct sockaddr*)&sa, sizeof(sa));
	if(ret)
	{
		log("Notify: failed (connect)");
		close(fd);
		return 0;
	}

	/* Text: Use ELE! */

	ret = write(fd, text, strlen(text) + 1);
	if(ret < strlen(text) + 1)
	{
		log("Notify: failed (write)");
		close(fd);
		return 0;
	}

	log("Notify: succeeded");
	
	close(fd);
	return 1;
}

static void metaserv_peers(ELE *ele)
{
	int i, j;
	ELE *el2, *el;
	char *username, *password, *uri;

	log("Notify peers");

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
	static char *xmlret = NULL;
	char *ret;
	char tmp[1024];
	char *status;
	ELE *ele, *ele2;
	char *att2;
	int i;
#ifdef METASERV_OPTIMIZED
	int j;
#endif

	if((!configuration)
	|| (!configuration->el)
	|| (!configuration->valid)
	|| (!configuration->processed))
	{
		log("Update: Invalid XML");
		return NULL;
	}

	if((!class) || (!category) || (!username) || (!password) || (!uri) || (!att) || (!atnum) || (!mode))
	{
		log("Update: Missing arguments");
		return NULL;
	}

	log("Update: class=%s, category=%s, username=%s, password=%s, uri=%s", class, category, username, password, uri);

	ret = NULL;
	ele2 = NULL;
	att2 = NULL;

	status = "accepted";
	if(!metaserv_auth(username, password, "update", class))
	{
		status = "rejected";
	}
	else
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
#endif
			}
			i++;
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
				ele->value = strdup(uri);

				/* add updated data to the meta server */
				ele2->elnum++;
				ele2->el = (ELE**)realloc(ele2->el, (ele2->elnum + 1) * sizeof(ELE*));
				ele2->el[ele2->elnum - 1] = ele;
				ele2->el[ele2->elnum] = NULL;

				metaserv_peers(ele);

				/* dump new configuration */
				/*minidom_dump(configuration);*/
			}
			else if(!strcmp(mode, "modify"))
			{
			}
			else if(!strcmp(mode, "delete"))
			{
				/* remove XML element */
				free(ele2);
				ele2 = NULL;

				metaserv_peers(NULL);

				/* dump new configuration */
				/*minidom_dump(configuration);*/
			}
			else status = "wrong";
		}
		else
		{
			status = "wrong";
		}
	}

	log("Update: status=%s", status);

	if(xmlret)
	{
		free(xmlret);
		xmlret = NULL;
	}
	xmlret = (char*)malloc(strlen(header) + 1);
	strcpy(xmlret, header);

	sprintf(tmp, "<result><status>%s</status></result>", status);
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
	char *class, *category;
	char *username, *password;
	char *uri2, *name;
	char *mode;
	int i, atnum;
#ifdef METASERV_OPTIMIZED
	int j;
#endif

	ret = NULL;
	uri2 = NULL;
	mode = NULL;
	atnum = 0;

	log("XML: uri=%s", uri);

	query = minidom_parse(uri);

	if((!query) || (!query->valid) || (!query->processed) || (!query->el) || (!query->el->name))
	{
		log("XML: Invalid XML");
		minidom_free(query);
		return NULL;
	}

	if((!strcmp(query->el->name, "query")) && (query->el->value))
	{
#ifdef METASERV_OPTIMIZED
		class = NULL;
		category = NULL;
		i = 0;
		while((query->el->at) && (query->el->at[i]))
		{
			if(!strcmp(query->el->at[i]->name, "class")) class = query->el->at[i]->value;
			if(!strcmp(query->el->at[i]->name, "type")) category = query->el->at[i]->value;
			i++;
		}
#else
		class = MD_att(query->el, "class");
		category = MD_att(query->el, "type");
#endif
		ret = metaserv_lookup(class, category, query->el->value, 1);
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
			i++;
		}
		i = 0;
#else
		class = MD_att(query->el, "class");
		category = MD_att(query->el, "type");
		username = MD_att(query->el, "username");
		password = MD_att(query->el, "password");
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
					if(!strcmp(name, "uri"))
					{
						uri2 = query->el->el[i]->value;
					}
					else if(!strcmp(name, "mode"))
					{
						mode = query->el->el[i]->value;
					}
					else
					{
						atnum++;
						att = (ATT**)realloc(att, (atnum + 1) * sizeof(ATT*));
						att[atnum - 1] = (ATT*)malloc(sizeof(ATT));
						att[atnum - 1]->name = strdup(name);
						att[atnum - 1]->value = strdup(query->el->el[i]->value);
						att[atnum] = NULL;
						/*printf("Got: %s/%s\n", name, query->el->el[i]->value);*/
					}
				}
			}
			/*if(!strcmp(query->el->el[i]->name, "uri")) uri2 = query->el->el[i]->value;*/
			i++;
		}
		ret = metaserv_update(class, category, username, password, uri2, att, atnum, mode);
	}
	else
	{
		log("XML: Unknown type of operation: name=%s", query->el->name);
	}

	minidom_free(query);

	return ret;
}

static char *metamagic(const char *rawuri)
{
	char *ret;
	char *token;
	char *category, *class;
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
	ret = NULL;

	/* query://ggz/connection/0.0.5pre */
	token = strtok(uri, ":/");
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
		if(token) ret = metaserv_lookup(class, category, token, 0);
		free(category);
		free(class);
	}
	while(token) token = strtok(NULL, ":/");
	free(uri);

	return ret;
}

static void metaserv_init()
{
	log("Initialization");
	configuration = minidom_load(METASERV_DIR "/metaservconf.xml");
	/*minidom_dump(configuration);*/
	/*fflush(NULL);*/
}

static void metaserv_shutdown()
{
	log("Shutdown");
	minidom_free(configuration);
}

static int metaserv_work()
{
	char buffer[1024];
	char *result;

	log("Enter main loop");
	while(1)
	{
		result = fgets(buffer, sizeof(buffer), stdin);
		if(result)
		{
			buffer[strlen(buffer) - 1] = 0;
			log("Request: buffer=%s", buffer);
			result = metamagic(buffer);

			if(result)
			{
				printf("%s\n", result);
				log("Result: result=%s", result);
			}
			else
			{
				printf("\n");
				log("No result");
			}
			fflush(NULL);
		}
		else
		{
			log("Disconnection.");
			exit(0);
		}
	}

	return 1;
}

int main(int argc, char *argv[])
{
	int ret;
	int optindex;
	int opt;
	struct option options[] =
	{
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"logfile", required_argument, 0, 'l'},
		{0, 0, 0, 0}
	};

	while(1)
	{
		opt = getopt_long(argc, argv, "hvl:", options, &optindex);
		if(opt == -1) break;
		switch(opt)
		{
			case 'h':
				printf("The GGZ Gaming Zone Meta Server\n");
				printf("Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net\n");
				printf("Published under GNU GPL conditions\n\n");
				printf("Available options:\n");
				printf("[-h | --help]:    Show this help\n");
				printf("[-v | --version]: Display version number\n");
				printf("[-l | --logfile]: Log events into this file\n");
				exit(0);
				break;
			case 'v':
				printf("0.1\n");
				exit(0);
				break;
			case 'l':
				logfile = optarg;
				break;
			default:
				printf("Unknown option, try --help.\n");
				exit(-1);
		}
	}

	metaserv_init();
	ret = metaserv_work();
	metaserv_shutdown();

	return 0;
}

