/* GGZ includes */
#include <ggz.h>

/* System includes */
#include <stdio.h>
#include <stdlib.h>

/* Modsniff includes */
#include "modsniff.h"

/* Configuration */
#include "config.h"

static int handle = -1;
static GGZModuleEntry *list = NULL;
static int listcount = 0;
static char *modulefile = NULL;

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

	path = modsniff_moduletest(GGZMODULECONFDIR);
	if(!path) path = modsniff_moduletest(PREFIX "/etc");
	if(!path) path = modsniff_moduletest("/usr/local/etc");
	if(!path) path = modsniff_moduletest("/etc");

	return path;
}

int modsniff_init(void)
{
	modulefile = modsniff_modulefile();
	if(!modulefile) return -1;
	
	handle = ggz_conf_parse(modulefile, GGZ_CONF_RDONLY);
	if(handle == -1)
	{
		printf("ERROR: Couldn't open configuration file: %s.\n", modulefile);
		return -1;
	}

	return 0;
}

GGZModuleEntry *modsniff_list(void)
{
	int ret;
	int i, j;
	char **modulelist, **felist;
	int modulecount, fecount;
	char *fe;

	if(handle == -1) return NULL;

	list = NULL;
	listcount = 0;

	ret = ggz_conf_read_list(handle, "Games", "*Engines*", &modulecount, &modulelist);
	if(ret == -1)
	{
		printf("ERROR: Couldn't read games list.\n");
		return NULL;
	}

	for(i = 0; i < modulecount; i++)
	{
		ret = ggz_conf_read_list(handle, "Games", modulelist[i], &fecount, &felist);
		if(ret == -1) continue;
		for(j = 0; j < fecount; j++)
		{
			fe = ggz_conf_read_string(handle, felist[j], "Frontend", "unknown");
			listcount++;
			list = (GGZModuleEntry*)realloc(list, sizeof(GGZModuleEntry) * (listcount + 1));
			list[listcount].name = NULL;
			list[listcount].frontend = NULL;
			list[listcount - 1].name = strdup(modulelist[i]);
			list[listcount - 1].frontend = strdup(fe);
		}
		for(j = 0; j < fecount; j++)
			ggz_free(felist[j]);
		ggz_free(felist);
	}

	if(modulelist)
	{
		for(i = 0; i < modulecount; i++)
			if(modulelist[i]) ggz_free(modulelist[i]);
		ggz_free(modulelist);
	}
	
	return list;
}

GGZModuleEntry *modsniff_merge(GGZModuleEntry *orig)
{
	int i, j, k;
	
	if(handle == -1) return NULL;
	if(!list) return NULL;

	j = 0;
	while((orig) && (orig[j].name))
	{
		for(i = 0; i < listcount; i++)
			if((orig[i].name) && (!strcmp(orig[j].name, list[i].name) && (!strcmp(orig[j].frontend, list[i].frontend))))
			{
				k = j;
				free(list[i].name);
				free(list[i].frontend);
				list[i] = list[listcount - 1];
				list[listcount - 1].name = NULL;
				list[listcount - 1].frontend = NULL;
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

	if(list)
	{
		for(i = 0; i < listcount; i++)
			if(list[i].name)
			{
				free(list[i].name);
				free(list[i].frontend);
			}
		free(list);
	}

	if(modulefile) free(modulefile);
	
	return 0;
}

