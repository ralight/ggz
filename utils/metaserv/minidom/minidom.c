#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "minidom.h"

/* Remove all line breaks, spaces, tabs, and check/remove XML header */
char *minidom_cleanstream(const char *stream)
{
	static char *cs = NULL;
	int i, j;
	int inside;

	if(!stream) return NULL;
	if(cs)
	{
		free(cs);
		cs = NULL;
	}
	cs = (char*)malloc(strlen(stream) + 1);

	j = 0;
	inside = 0;
	for(i = 0; i < strlen(stream); i++)
	{
		if((stream[i] == '\t') || (stream[i] == '\n')) continue;
		if((!inside) && (stream[i] == ' ')) continue;
		if(stream[i] == '<')
		{
			inside = 1;
			if(stream[i + 1] == '?') inside = 2;
		}
		if(inside != 2)
		{
			cs[j] = stream[i];
			j++;
		}
		if(stream[i] == '>') inside = 0;
	}
	cs[j] = 0;

	/*printf("DEBUG: cleanstream: return %s\n", cs);*/
	return cs;
}

/* Return position of c or -1 if not found */
int strpos(const char *s, char c)
{
	int i;

	if(!s) return -1;
	for(i = 0; i < strlen(s); i++)
		if(s[i] == c) return i;
	return -1;
}

/* Add a complete tag to an element, return the new child */
ELE *minidom_makechild(ELE *parent, char *tag)
{
	char *token;
	int i;
	ELE *ele;
	ATT *att;
	int pos;

	if(!tag) return parent;

	ele = (ELE*)malloc(sizeof(ELE));
	ele->parent = parent;
	ele->name = NULL;
	ele->value = NULL;
	ele->at = NULL;
	ele->el = NULL;
	ele->elnum = 0;
	ele->atnum = 0;

	if(parent)
	{
		parent->elnum++;
		parent->el = (ELE**)realloc(parent->el, (parent->elnum + 1) * sizeof(ELE*));
		parent->el[parent->elnum - 1] = ele;
		parent->el[parent->elnum] = NULL;
	}

	token = strtok(tag, " ");
	i = 0;
	while(token)
	{
		if(i == 0)
		{
			/* name */
			/*printf(" ** %s\n", token);*/
			ele->name = (char*)malloc(strlen(token) + 1);
			strcpy(ele->name, token);
		}
		else
		{
			/* attribute */
			/*printf(" ++ %s\n", token);*/
			att = (ATT*)malloc(sizeof(ATT));
			pos = strpos(token, '=');
			if(pos == -1) att->name = strdup(token);
			else
			{
				att->name = (char*)malloc(pos + 1);
				att->value = (char*)malloc(strlen(token) - pos + 1);
				memcpy(att->name, token, pos);
				memcpy(att->value, token + pos + 1, strlen(token) - pos);
			}
			ele->atnum++;
			ele->at = realloc(ele->at, ele->atnum + 1);
			ele->at[ele->atnum - 1] = att;
			ele->at[ele->atnum] = NULL;
		}
		token = strtok(NULL, " ");
		i++;
	}

	return ele;
}

/* Parses a stream to add its contents to a DOM */
DOM *minidom_parse(DOM *dom, const char *stream)
{
	char *cs;
	int i, mark, lastmark;
	char *token;
	int error = 0;
	ELE *ele, *cp;							/* root node and current pointer */

	if(!stream) return dom;
	cs = minidom_cleanstream(stream);

	/*ele = (ELE*)malloc(sizeof(ELE));*/		/* memory loss! */
	/*ele->parent = NULL;
	ele->name = NULL;
	ele->value = NULL;
	ele->el = NULL;
	ele->at = NULL;
	ele->elnum = 0;*/
	ele = NULL;
	cp = NULL;
	token = NULL;
	mark = -1;
	error = 0;
	lastmark = 0;
	for(i = 0; i < strlen(cs); i++)
	{
		if(cs[i] == '<')
		{
			if(mark == -1) mark = i + 1;
			else error = 1;
			if(i != lastmark)
			{
				if(token) free(token);
				token = (char*)malloc(i - lastmark + 1);
				memcpy(token, cs + lastmark, i - lastmark);
				token[i - lastmark] = 0;
				/*printf("  --> content: %s\n", token);*/
				cp->value = (char*)malloc(strlen(token) + 1);
				strcpy(cp->value, token);
			}
		}
		if(cs[i] == '>')
		{
			if(mark != -1)
			{
				if(token) free(token);
				token = (char*)malloc(i - mark + 1);
				memcpy(token, cs + mark, i - mark);
				token[i - mark] = 0;
				/*printf("--> token: %s\n", token);*/
				if((token[0] == '/') && (cp)) cp = cp->parent;
				else
				{
					/*printf("INSERT AT: %i\n", cp);*/
					cp = minidom_makechild(cp, token);
					if((cp) && (!ele)) ele = cp;
					/*if(cp) cp = cp->parent;*/ /* QUICK HACK?! */
				}
				mark = -1;
				lastmark = i + 1;
			}
			else error = 1;
		}
	}

	dom->valid = !error;
	dom->processed = 1;
	dom->el = ele;

	return dom;
}

/* Loads an XML file and return the DOM */
DOM *minidom_load(const char *file)
{
	DOM *dom;
	FILE *f;
	char buf[1024];
	char *buffer;

	dom = NULL;
	f = fopen(file, "r");
	if(!f) return NULL;
	dom = (DOM*)malloc(sizeof(DOM));
	dom->processed = 0;
	dom->valid = 0;
	dom->el = NULL;
	buffer = (char*)malloc(1);
	strcpy(buffer, "");
	while(fgets(buf, sizeof(buf), f))
	{
		buffer = (char*)realloc(buffer, strlen(buffer) + strlen(buf));
		buf[strlen(buf) - 1] = 0;
		strcat(buffer, buf);
	}
	fclose(f);

	/*printf("DEBUG: load: parse %s\n", buffer);*/
	minidom_parse(dom, buffer);
	free(buffer);

	return dom;
}

void minidom_internal_dump(ELE *ele);		/* forward decl */

/* Dump out the DOM in XML format */
/* FIXME: return a char* and print this if dump is needed */
void minidom_dump(DOM *dom)
{
	if(!dom) return;
	if(!dom->processed)
	{
		printf("ERROR: DOM is incomplete!\n");
		return;
	}
	if(!dom->valid)
	{
		printf("ERROR: DOM is invalid!\n");
		return;
	}
	if(!dom->el)
	{
		printf("ERROR: DOM is empty!\n");	/* is this really an error? */
		return;
	}
	minidom_internal_dump(dom->el);
}

void minidom_internal_dump(ELE *ele)
{
	int i;
	static int indent = 0;

	if(!ele) return;
	indent++;
	for(i = 0; i < indent * 2; i++)
		printf("==");
	printf(" (%i) %s: %s\n", indent, ele->name, ele->value);

	i = 0;
	while((ele->at) && (ele->at[i]))
	{
		printf("%s = %s\n", ele->at[i]->name, ele->at[i]->value);
		i++;
	}

	i = 0;
	while((ele->el) && (ele->el[i]))
	{
		minidom_internal_dump(ele->el[i]);
		i++;
	}
	indent--;
}

/* Clean up after all operations */
void minidom_free(DOM *dom)
{
	free(dom);
	dom = 0;
}

/* Main functions: This is for convenience only. */
int main(int argc, char *argv[])
{
	DOM *dom;

	dom = minidom_load("example.xml");
	minidom_dump(dom);

	return 0;
}

