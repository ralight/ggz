/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "module.h"
#include "gurumod.h"
#include "i18n.h"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ggzcore.h>

/* Defines */
#define GRUBBYCONF "/.ggz/grubby.rc"

/* Plugin call function */
typedef Guru* (*modulefunc)(Guru *message);

/* Global variables */
char **modulenamelist = NULL;
void **modulelist = NULL;
modulefunc *functionlist = NULL;
int modulecount = 0;
Gurucore *core;
int handler;

/* Load the module list and activate the initial modules */
Gurucore *guru_module_init()
{
	int ret;
	char *path;
	char **list;
	int count, i;
	char *module;
	char *home;

	/* Open configuration file */
	home = getenv("HOME");
	path = (char*)malloc(strlen(home) + strlen(GRUBBYCONF) + 1);
	strcpy(path, home);
	strcat(path, GRUBBYCONF);
	handler = ggzcore_confio_parse(path, GGZ_CONFIO_RDONLY);
	free(path);
	if(handler < 0) return NULL;

	/* Load main option into grubby's core */
	core = (Gurucore*)malloc(sizeof(Gurucore));
	core->host = ggzcore_confio_read_string(handler, "preferences", "host", "localhost");
	core->name = ggzcore_confio_read_string(handler, "preferences", "name", "grubby/unnamed");
	core->guestname = (char*)malloc(strlen(core->name) + 4);
	strcpy(core->guestname, core->name);
	strcat(core->guestname, "(G)");
	core->owner = ggzcore_confio_read_string(handler, "preferences", "owner", NULL);
	core->autojoin = ggzcore_confio_read_int(handler, "preferences", "autojoin", 0);
	core->logfile = ggzcore_confio_read_string(handler, "preferences", "logfile", NULL);

	/* Preload libraries shared among multiple plugins */
	module = ggzcore_confio_read_string(handler, "guru", "player", NULL);
	if(module)
	{
		printf("Loading core module PLAYER: %s... ", module);
		fflush(NULL);
		if(dlopen(module, RTLD_NOW | RTLD_GLOBAL)) printf("OK\n");
		else
		{
			printf("ERROR: Not a shared library\n");
			exit(-1);
		}
	}

	/* Load net plugin */
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
	|| ((core->net_output = dlsym(core->nethandle, "net_output")) == NULL)
	|| ((core->net_log = dlsym(core->nethandle, "net_logfile")) == NULL))
	{
		printf("ERROR: Couldn't find net functions\n");
		exit(-1);
	}
	printf("OK\n");

	/* Load i18n plugin */
	core->i18n_init = NULL;
	core->i18n_translate = NULL;
	module = ggzcore_confio_read_string(handler, "guru", "i18n", NULL);
	if(module)
	{
		printf("Loading core module I18N: %s... ", module);
		fflush(NULL);
		if((!module) || ((core->i18nhandle = dlopen(module, RTLD_NOW)) == NULL))
		{
			printf("ERROR: Not a shared library\n");
			exit;
		}
		if(((core->i18n_init = dlsym(core->i18nhandle, "guru_i18n_initialize")) == NULL)
		|| ((core->i18n_translate = dlsym(core->i18nhandle, "guru_i18n_translate")) == NULL)
		|| ((core->i18n_check = dlsym(core->i18nhandle, "guru_i18n_check")) == NULL))
		{
			printf("ERROR: Couldn't find i18n functions\n");
			exit(-1);
		}
		printf("OK\n");
	}

	/* Add all specified generic plugins */
	ret = ggzcore_confio_read_list(handler, "guru", "modules", &count, &list);
	if(ret >= 0)
	{
		for(i = 0; i < count; i++)
			guru_module_add(list[i]);
	}

	return core;
}

/* Insert a plugin dynamically */
int guru_module_add(const char *modulealias)
{
	void *handle, *init;
	modulefunc func;
	char *modulename;
	int i;

	/* Find appropriate shared library first */
	if(!modulealias) return 0;
	modulename = ggzcore_confio_read_string(handler, "modules", modulealias, NULL);
	printf("Loading module: %s... ", modulename);
	fflush(NULL);

	/* Avoid duplicates */
	for(i = 0; i < modulecount; i++)
		if((modulelist[i]) && (!strcmp(modulenamelist[i], modulealias)))
		{
			printf("ERROR: Already loaded!!\n");
			return 0;
		}

	/* Check plugin consistency */
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

	/* Initialize the plugin */
	((void*(*)(void))init)();

	/* Insert it into the plugin list for later reference */
	modulecount++;
	modulelist = (void**)realloc(modulelist, (modulecount + 1) * sizeof(void*));
	functionlist = (modulefunc*)realloc(functionlist, (modulecount + 1) * sizeof(modulefunc));
	modulenamelist = (char**)realloc(modulenamelist, (modulecount + 1) * sizeof(char*));
	modulelist[modulecount - 1] = handle;
	modulelist[modulecount] = NULL;
	functionlist[modulecount - 1] = (modulefunc)malloc(sizeof(modulefunc));
	functionlist[modulecount - 1] = func;
	functionlist[modulecount] = NULL;
	modulenamelist[modulecount - 1] = (char*)malloc(strlen(modulealias) + 1);
	strcpy(modulenamelist[modulecount - 1], modulealias);
	modulenamelist[modulecount] = NULL;

	printf("OK\n");
	return 1;
}

/* Deactivate a plugin and unload it */
/* FIXME: look at free, and dlclose() module */
int guru_module_remove(const char *modulealias)
{
	int i;

	for(i = 0; i < modulecount; i++)
	{
		if((modulenamelist[i]) && (!strcmp(modulenamelist[i], modulealias)))
		{
			modulelist[i] = NULL;
			/*free(modulenamelist[i]);*/ /* HUH? */
			modulenamelist[i] = NULL;
			/*free(functionlist[i]);*/ /* HUH? */
			functionlist[i] = NULL;
			return 1;
		}
	}

	return 0;
}

/* List all plugins currently loaded */
char *guru_modules_list()
{
	static char *list = NULL;
	const char *prepend;
	int i;

	/* Build up list dynamically */
	prepend = _("List of available modules:");
	if(list) free(list);
	list = (char*)malloc(strlen(prepend) + 1);
	strcpy(list, prepend);
	for(i = 0; i < modulecount; i++)
	{
		if(modulenamelist[i])
		{
			list = (char*)realloc(list, strlen(list) + strlen(modulenamelist[i]) + 2);
			strcat(list, " ");
			strcat(list, modulenamelist[i]);
		}
	}
	return list;
}

/* Evaluate administrativia (admins only) */
Guru *guru_module_internal(Guru *message)
{
	char *token;
	int i;
	int modules, modadd, modremove;
	char *mod;

	i = 0;
	modules = 0;
	modadd = 0;
	modremove = 0;
	mod = NULL;
	
	/* Find all commands */
	while((message->list) && (message->list[i]))
	{
		token = message->list[i];
		if((i == 0) && (!strcasecmp(token, core->name))) modules++;
		if((i == 0) && (!strcasecmp(token, core->guestname))) modules++;
		if((i == 1) && (!strcmp(token, "modules"))) modules++;
		if((i == 1) && (!strcmp(token, "insmod"))) modadd++;
		if((i == 1) && (!strcmp(token, "rmmod"))) modremove++;
		if((i == 2) && ((modadd) || (modremove))) mod = strdup(token);
		i++;
	}
	if(message->type == GURU_PRIVMSG) modules++;

	/* Process given commands */
	if(modules == 2)
	{
		free(message->message);
		message->message = guru_modules_list();
		message->type = GURU_PRIVMSG;
		return message;
	}
	if((modules == 1) && ((modadd) || (modremove)) && (mod))
	{
		free(message->message);
		message->type = GURU_PRIVMSG;
		if((core->owner) && (!strcmp(core->owner, message->player)))
		{
			if(modadd)
			{
				if(guru_module_add(mod)) message->message = "Module added.";
				else message->message = "Error: Could not add module.";
			}
			if(modremove)
			{
				if(guru_module_remove(mod)) message->message = "Module removed.";
				else message->message = "Error: Could not remove module.";
			}
		}
		else message->message = "Only my owner can change the module setup.";
		if(mod) free(mod);
		return message;
	}

	return NULL;
}

/* Main function: Iterate over all loaded plugins */
/* FIXME: memory issues, and make sleep() a plugin issue */
Guru *guru_module_work(Guru *message, int priority)
{
	int i, j;
	modulefunc func;
	Guru *ret;
	char *savemsg;
	char *moresave;

	/* Check admin commands first */
	ret = guru_module_internal(message);
	if(ret) return ret;

	/* Now try plugins, with decreasing priority */
	if(!modulelist) return NULL;
	savemsg = message->message;
	for(j = 10; j >= 0; j-=11)
	{
		for(i = 0; i < modulecount; i++)
		{
			if(!functionlist[i]) continue;
			if(savemsg) message->message = strdup(savemsg);
			moresave = message->message;
			if(j == 10) printf("Trying module no. %i with '%s'\n", i, message->message);
			func = functionlist[i];
			ret = (func)(message);
			if(savemsg) free(moresave); /* EH?! */
			if((ret) && (ret->message))
			{
				/*sleep(strlen(g.message) / 7);*/
				free(savemsg);
				return ret;
			}
		}
	}
	free(savemsg);
	return NULL;
}

