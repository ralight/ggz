#include <stdio.h>
#include "minidom.h"

/* Main functions: This is for convenience only. */
int main(int argc, char *argv[])
{
	DOM *dom;
	ELE *ele;
	char *file;

	file = "example.xml";
	if(argc == 2) file = argv[1];
	dom = minidom_load(file);
	minidom_dump(dom);

	printf("Query resultset/result[0]/host:\n");
	ele = MD_query(MD_query(dom->el, "result"), "host");
	if(ele) printf("Found: %s\n", ele->value);

	minidom_free(dom);

	return 0;
}

