#include <ggz.h>
#include <stdio.h>
#include <string.h>

#define ggz_foreachsplit(s, sep, var, skip) \
	{ \
	char *var = NULL; \
	int i; \
	for(i = 0; (var = ggz_strsplit(s, sep, i, skip)); ggz_free(var), i++)

#define ggz_foreachend() \
	}

char *ggz_strsplit(const char *s, const char *sep, int pos, int skipempty)
{
	int i, j;
	int prev = 0;
	int match = 0;
	int size;
	char *x;

	for(i = 0; s[i]; i++)
	{
		for(j = 0; sep[j]; j++)
		{
			if((s[i] == sep[j]) || ((!s[i + 1]) && (!sep[j + 1])))
			{
				size = i - prev;
				if(s[i] != sep[j]) size++;
				if((size > 0) || (!skipempty))
				{
					if(match == pos)
					{
						x = ggz_malloc(size + 1);
						strncpy(x, s + prev, size);
						x[size] = '\0';
						return x;
					}
				}
				else
				{
					match--;
				}
				match++;
				prev = i + 1;
			}
		}
	}

	return NULL;
}

int main()
{
	const char *var = "Hello World, what's up?";

	printf("Run 1: simple split\n");

	ggz_foreachsplit(var, " ", splitter, 1)
	{
		printf("-> %s\n", splitter);
	}
	ggz_foreachend();

	printf("Run 2: multi-separator split\n");

	ggz_foreachsplit(var, " ,'?", splitter, 0)
	{
		printf("-> %s\n", splitter);
	}
	ggz_foreachend();

	printf("Run 3: multi-separator but skipping empty tokens\n");

	ggz_foreachsplit(var, " ,'?", splitter, 1)
	{
		printf("-> %s\n", splitter);
	}
	ggz_foreachend();

	return 0;
}

