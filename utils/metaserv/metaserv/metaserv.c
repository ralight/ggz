/*
 * The GGZ Gaming Zone Metaserver Project
 * Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
 * Published under GNU GPL conditions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "minidom.h"

#define METASERVDIR "/usr/local/share/metaserv"

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

DOM *configuration = NULL;

char *metaserv_lookup(const char *class, const char *category, const char *key, int xmlformat)
{
	const char *header = "<?xml version=\"1.0\"?><resultset>";
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

	att = configuration->el->at;
	valid = 0;
	preference = 0;
	ret = NULL;
	r = NULL;

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

	i = 0;
	while((att) && (att[i]))
	{
		if((!strcmp(att[i]->name, "class"))
		&& (!strcmp(att[i]->value, class)))
		{
			valid = 1;
		}
		i++;
	}
	if(!valid) return NULL;

	ele = configuration->el->el;
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

char *metaserv_xml(const char *uri)
{
	DOM *query;
	char *ret;
	char *class, *category;
	int i;

	ret = NULL;
	class = NULL;
	category = NULL;
	query = minidom_parse(uri);
	if((!query->valid) || (!query->processed) || (!query->el)
	|| (!query->el->name) || (strcmp(query->el->name, "query")))
	{
		minidom_free(query);
		return NULL;
	}

	if((query) && (query->el) && (query->el->value))
	{
		i = 0;
		while((query->el->at) && (query->el->at[i]))
		{
			if(!strcmp(query->el->at[i]->name, "class")) class = query->el->at[i]->value;
			if(!strcmp(query->el->at[i]->name, "type")) category = query->el->at[i]->value;
			i++;
		}
		ret = metaserv_lookup(class, category, query->el->value, 1);
	}

	minidom_free(query);
	return ret;
}

char *metamagic(char *uri)
{
	char *ret;
	char *token;
	char *category, *class;

	if(!uri) return NULL;

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

void metaserv_init()
{
	configuration = minidom_load(METASERVDIR "/metaservconf.xml");
	/*minidom_dump(configuration);
	fflush(NULL);*/
}

void metaserv_shutdown()
{
	minidom_free(configuration);
}

int main(int argc, char *argv[])
{
	char buffer[1024];
	char *result;

	metaserv_init();

	fgets(buffer, sizeof(buffer), stdin);
	result = metamagic(strdup(buffer));
	if(result) printf("%s\n", result);
	else printf("\n");
	fflush(NULL);

	metaserv_shutdown();
	return 0;
}

