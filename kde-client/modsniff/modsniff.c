#include <ggzcore.h>

#include <stdio.h>
#include <stdlib.h>

#include "modsniff.h"

int handle = -1;
char **list = NULL;
int listcount = 0;
char *modulefile = NULL;

/* Prototypes */
char *modsniff_moduletest(const char *directory);
char *modsniff_modulefile(void);

char *modsniff_moduletest(const char *directory)
{
	char *path;
	FILE *f;

	path = (char*)malloc(strlen(directory) + 13);
	sprintf(path, "%s/ggz.modules", directory);
	f = fopen(path, "r");
	if(f)
	{
		fclose(f);
		return path;
	}
	return NULL;
}

char *modsniff_modulefile(void)
{
	char *path;

	path = modsniff_moduletest("/usr/local/etc");
	if(!path) path = modsniff_moduletest("/usr/etc");
	if(!path) path = modsniff_moduletest("/etc");

	return path;
}

int modsniff_init(void)
{
	modulefile = modsniff_modulefile();
	if(!modulefile) return -1;
	
	handle = ggzcore_confio_parse(modulefile, GGZ_CONFIO_RDONLY);
	if(handle == -1)
	{
		printf("ERROR: Couldn't open configuration file: %s.\n", modulefile);
		return -1;
	}
	printf("Initialized with %s\n", modulefile);

	return 0;
}

char **modsniff_list(void)
{
	int ret;

	if(handle == -1) return NULL;

	ret = ggzcore_confio_read_list(handle, "Games", "*GameList*", &listcount, &list);
	if(ret == -1)
	{
		printf("ERROR: Couldn't read games list.\n");
		return NULL;
	}

	return list;
}

char **modsniff_merge(char **orig)
{
	int i, j, k;
	
	if(handle == -1) return NULL;
	if(!list) return NULL;
	
	j = 0;
	while((orig) && (orig[j]))
	{
		for(i = 0; i < listcount; i++)
			if((orig[j]) && (!strcmp(orig[j], list[i])))
			{
				k = j;
				free(list[i]);
				list[i] = list[listcount - 1];
				list[listcount - 1] = NULL;
				listcount--;
				break;
			}
		j++;
	}
	
	return list;
}

int modsniff_end(void)
{
	int i;
		
	if(handle == -1) return -1;

	for(i = 0; i < listcount; i++)
		if(list[i]) free(list[i]);
	if(list) free(list);

	if(modulefile) free(modulefile);
	
	return 0;
}

