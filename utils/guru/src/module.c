/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "module.h"
#include "gurumod.h"

#include <dlfcn.h>
#include <stdio.h>
#include <dirent.h>
#include <fnmatch.h>
#include <stdlib.h>

#define MODULEDIR "/usr/local/lib"

typedef Guru (*modulefunc)(Guru message);

void **modulelist;
modulefunc **functionlist;
int modulecount;

int guru_module_init()
{
	DIR *dp;
	struct dirent *ep;
	int ret;

	modulecount = 0;
	modulelist = NULL;
	functionlist = NULL;

	dp = opendir(MODULEDIR);
	if(dp)
	{
		while((ep = readdir(dp)) != NULL)
		{
			if(!fnmatch("*.so", ep->d_name, FNM_NOESCAPE))
			{
				ret = guru_module_add(ep->d_name);
			}
		}
		closedir(dp);
	}

	return 1;
}

int guru_module_add(const char *modulename)
{
	char modulefile[1024];
	void *handle, *init;
	modulefunc *func;

	printf("Loading module: %s... ", modulename);
	fflush(NULL);
	sprintf(modulefile, "%s/%s", MODULEDIR, modulename);

	if((handle = dlopen(modulefile, RTLD_NOW)) == NULL)
	{
		printf("ERROR: Not a shared library!\n");
		return 0;
	}
	init = dlsym(handle, "gurumod_init");
	if(!init)
	{
		printf("ERROR: Invalid module (init)!\n");
		return 0;
	}
	func = dlsym(handle, "gurumod_exec");
	if(!func)
	{
		printf("ERROR: Invalid module (exec)!\n");
		return 0;
	}

	((void*(*)(void))init)();

	modulecount++;
	modulelist = (void**)realloc(modulelist, (modulecount + 1) * sizeof(void*));
	functionlist = (modulefunc**)realloc(functionlist, modulecount + 1);
	modulelist[modulecount - 1] = handle;
	modulelist[modulecount] = NULL;
	functionlist[modulecount - 1] = func;
	functionlist[modulecount] = NULL;

	printf("OK\n");
	return 1;
}

int guru_module_remove(const char *modulename)
{
	return 1;
}

char *guru_module_work(const char *message, int priority)
{
	int i, j;
	Guru g;

	if(!modulelist) return NULL;
	for(j = 10; j >= 0; j--)
	{
		i = 0;
		while(modulelist[i])
		{
			g.message = (char*)message;
			g.priority = j;
			if(j == 10) printf("Trying module no. %i with '%s'\n", i, g.message);
			g = ((Guru(*)(Guru))functionlist[i])(g);
			if(g.message)
			{
				/*sleep(strlen(g.message) / 7);*/
				return g.message;
			}
			i++;
		}
	}
	return NULL;
}

