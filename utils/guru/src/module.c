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
#include <stdlib.h>
#include <string.h>
#include <ggzcore.h>

typedef Guru* (*modulefunc)(Guru *message);

void **modulelist = NULL;
modulefunc *functionlist = NULL;
int modulecount = 0;

Gurucore *guru_module_init()
{
	int handler, ret;
	char path[1024];
	char **list;
	int count, i;
	char *module;
	Gurucore *core;

	sprintf(path, "%s/.ggz/guru.rc", getenv("HOME"));
	handler = ggzcore_confio_parse(path, GGZ_CONFIO_RDONLY);
	if(handler < 0) return NULL;

	core = (Gurucore*)malloc(sizeof(Gurucore));

	module = ggzcore_confio_read_string(handler, "guru", "net", NULL);
	printf("Loading core module NET: %s... ", module);
	fflush(NULL);

	if((!module) || ((core->nethandle = dlopen(module, RTLD_NOW)) == NULL))
	{
		printf("ERROR: Not a shared library\n");
		exit(-1);
	}
	if(((core->net_connect = dlsym(core->nethandle, "net_connect")) == NULL)
	|| ((core->net_join = dlsym(core->nethandle, "net_join")) == NULL)
	|| ((core->net_status = dlsym(core->nethandle, "net_status")) == NULL)
	|| ((core->net_input = dlsym(core->nethandle, "net_input")) == NULL)
	|| ((core->net_output = dlsym(core->nethandle, "net_output")) == NULL))
	{
		printf("ERROR: Couldn't find net functions\n");
		exit(-1);
	}
	printf("OK\n");

	ret = ggzcore_confio_read_list(handler, "guru", "modules", &count, &list);
	if(ret < 0) return NULL;

	for(i = 0; i < count; i++)
	{
		module = ggzcore_confio_read_string(handler, "modules", list[i], NULL);
		guru_module_add(module);
	}

	return core;
}

int guru_module_add(const char *modulename)
{
	void *handle, *init;
	modulefunc func;

	if(!modulename) return 0;
	printf("Loading module: %s... ", modulename);
	fflush(NULL);

	if((handle = dlopen(modulename, RTLD_NOW)) == NULL)
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
	functionlist = (modulefunc*)realloc(functionlist, (modulecount + 1) * sizeof(modulefunc));
	modulelist[modulecount - 1] = handle;
	modulelist[modulecount] = NULL;
	functionlist[modulecount - 1] = (modulefunc)malloc(sizeof(modulefunc));
	functionlist[modulecount - 1] = func;
	functionlist[modulecount] = NULL;

	printf("OK\n");
	return 1;
}

int guru_module_remove(const char *modulename)
{
	return 1;
}

Guru *guru_module_work(Guru *message, int priority)
{
	int i, j;
	modulefunc func;
	Guru *ret;
	char *savemsg;

	if(!modulelist) return NULL;
	savemsg = message->message;
	for(j = 10; j >= 0; j--)
	{
		i = 0;
		while(modulelist[i])
		{
			message->message = strdup(savemsg);
			if(j == 10) printf("Trying module no. %i with '%s'\n", i, message->message);
			func = functionlist[i];
			ret = (func)(message);
			if(ret->message)
			{
				printf("Debug: got %s\n", ret->message);
				/*sleep(strlen(g.message) / 7);*/
				return ret;
			}
			i++;
		}
	}
	return NULL;
}

