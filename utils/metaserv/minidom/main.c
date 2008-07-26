/*
 * The GGZ Gaming Zone Metaserver Project
 * Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
 * Published under GNU GPL conditions.
 */

#include <stdio.h>
#include "minidom.h"

/* Main functions: This is for convenience only. */
int main(int argc, char *argv[])
{
	DOM *dom;
	ELE *ele;
	char *file;

	if(argc == 2)
	{
		file = argv[1];
	}
	else
	{
		fprintf(stderr, "Syntax: minidom [file]\n");
		return 1;
	}

	dom = minidom_load(file);
	minidom_dump(dom);

	if(dom)
	{
		printf("Is the DOM valid? %i\n", dom->valid);

		printf("Query resultset/result[0]/host:\n");
		ele = MD_query(MD_query(dom->el, "result"), "host");
		if(ele) printf("Found: %s\n", ele->value);

		minidom_free(dom);

		return 0;
	}
	else
	{
		fprintf(stderr, "Error loading file '%s'.\n", file);

		return 1;
	}
}

