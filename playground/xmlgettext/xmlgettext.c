#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct entry_t
{
	char *entry;
	int *occurencelines;
	int occurences;
};
typedef struct entry_t entry;

void output_header(void)
{
	printf("# SOME DESCRIPTIVE TITLE.\n");
	printf("# Copyright (C) YEAR Free Software Foundation, Inc.\n");
	printf("# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.\n");
	printf("#\n");
	printf("#, fuzzy\n");
	printf("msgid \"\"\n");
	printf("msgstr \"\"\n");
	printf("\"Project-Id-Version: PACKAGE VERSION\\n\"\n");
	printf("\"POT-Creation-Date: 2002-03-24 17:05+0100\\n\"\n");
	printf("\"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n\"\n");
	printf("\"Last-Translator: FULL NAME <EMAIL@ADDRESS>\\n\"\n");
	printf("\"Language-Team: LANGUAGE <LL@li.org>\\n\"\n");
	printf("\"MIME-Version: 1.0\\n\"\n");
	printf("\"Content-Type: text/plain; charset=CHARSET\\n\"\n");
	printf("\"Content-Transfer-Encoding: 8bit\\n\"\n");
	printf("\n");
}

void output_entries(entry **list)
{
	int i = 0;
	int j;

	while(list[i])
	{
		printf("# (%i) line", i);
		if(list[i]->occurences > 1) printf("s");
		for(j = 0; j < list[i]->occurences; j++)
		{
			if(j > 0) printf(",");
			printf(" %i", list[i]->occurencelines[j]);
		}
		printf("\n");
		printf("msgid \"%s\"\n", list[i]->entry);
		printf("msgstr \"\"\n");
		printf("\n");

		i++;
	}
}

char *get_input(void)
{
	char xbuf[256];
	int size;
	int inputsize = 0;
	char *input = NULL;

	while(fgets(xbuf, sizeof(xbuf), stdin))
	{
		size = strlen(xbuf);
		inputsize += size;
		input = (char*)realloc(input, inputsize);
		memcpy(input + inputsize - size, xbuf, size);
	}
	if(input) input[inputsize] = 0;

	return input;
}

int main(int argc, char *argv[])
{
	char *input;
	int inputsize;
	char *tmp = NULL;
	int tmpsize = 0;
	int i, j;
	int mode = 1;
	int line = 1;
	entry **entrylist = NULL;
	int entries = 0;
	entry *e;

	input = get_input();
	if(!input) return -1;

	output_header();

	inputsize = strlen(input);
	for(i = 0; i < inputsize; i++)
	{
		if(input[i] == '\n') line++;
		if(input[i] == '<')
		{
			mode = 0;
			if(tmp)
			{
				if(!(tmpsize % 256)) tmp = (char*)realloc(tmp, tmpsize + 256);
				tmp[tmpsize++] = 0;

				e = NULL;
				for(j = 0; j < entries; j++)
					if((entrylist[j]) && (entrylist[j]->entry))
						if(!strcmp(entrylist[j]->entry, tmp)) e = entrylist[j];
				if(!e)
				{
					e = (entry*)malloc(sizeof(entry));
					e->occurences = 0;
					e->occurencelines = NULL;
					e->entry = strdup(tmp);

					entrylist = (entry**)realloc(entrylist, (entries + 2) * sizeof(entry));
					entrylist[entries] = e;
					entrylist[entries + 1] = NULL;
					entries++;
				}
				e->occurencelines = (int*)realloc(e->occurencelines, (e->occurences + 1) * sizeof(int));
				e->occurencelines[e->occurences] = line;
				e->occurences++;

				free(tmp);
				tmp = NULL;
				tmpsize = 0;
			}
		}
		if(mode == 1)
		{
			if(input[i] == '\n') input[i] = ' ';
			if(!(tmpsize % 256)) tmp = (char*)realloc(tmp, tmpsize + 256);
			tmp[tmpsize++] = input[i];
		}
		if(input[i] == '>') mode = 1;
	}

	if(input) free(input);
	if(entrylist)
	{
		output_entries(entrylist);

		for(i = 0; i < entries; i++)
		{
			if(entrylist[i])
			{
				if(entrylist[i]->entry) free(entrylist[i]->entry);
				if(entrylist[i]->occurencelines) free(entrylist[i]->occurencelines);
				free(entrylist[i]);
			}
		}
		free(entrylist);
	}
	return 0;
}


