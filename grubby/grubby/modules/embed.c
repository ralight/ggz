/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 - 2005 Josef Spillner <josef@ggzgamingzone.org>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ggz.h>
#include "gurumod.h"
#include "i18n.h"
#include "config.h"

#ifdef EMBED_RUBY
/* FIXME: ruby's config.h shouldn't declare those */
#undef PACKAGE_NAME
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef PACKAGE_STRING
#undef PACKAGE_BUGREPORT
#include <ruby.h>
#endif
#ifdef EMBED_PERL
#include <EXTERN.h>
#include <perl.h>
#endif
#ifdef EMBED_PYTHON
#include <Python.h>
#endif
#ifdef EMBED_TCL
#include <tcl.h>
#endif
#ifdef EMBED_PHP
#include <php_embed.h>
#endif

#define EMBEDCONF "/grubby/modembed.rc"

static char **aliaslist = NULL;
static char **scriptlist = NULL;
static int *typelist = NULL;

#ifdef EMBED_PERL
static PerlInterpreter *my_perl;
#endif
#ifdef EMBED_PYTHON
PyObject *pxDict;
#endif
#ifdef EMBED_TCL
Tcl_Interp *inter;
#endif

#define TYPE_UNKNOWN 1
#define TYPE_RUBY 2
#define TYPE_PERL 3
#define TYPE_PYTHON 4
#define TYPE_TCL 5
#define TYPE_PHP 6

/* Determine mime type of a file */
static int mimetype(const char *file)
{
	FILE *f;
	char buffer[128];
	char *ret;
	int type;

	type = TYPE_UNKNOWN;
	f = fopen(file, "r");
	if(f)
	{
		ret = fgets(buffer, sizeof(buffer), f);
		if(ret)
		{
			if(strstr(buffer, "perl")) type = TYPE_PERL;
			if(strstr(buffer, "ruby")) type = TYPE_RUBY;
			if(strstr(buffer, "python")) type = TYPE_PYTHON;
			if(strstr(buffer, "tcl")) type = TYPE_TCL;
			if(strstr(buffer, "php")) type = TYPE_PHP;
		}
		fclose(f);
	}
	return type;
}

/* Empty init */
void gurumod_init(const char *datadir)
{
	int handle;
	char *path;
	int count;
	int ret;
	int i;
	char *script;
	int num;

#ifdef EMBED_RUBY
	ruby_init();
#endif
#ifdef EMBED_PERL
	my_perl = perl_alloc();
	perl_construct(my_perl);
#endif
#ifdef EMBED_PYTHON
	pxDict = NULL;
#if PY_MAJOR_VERSION > 2 || PY_MINOR_VERSION >= 4
	Py_InitializeEx(0);
#else
	Py_Initialize();
#endif
#endif
#ifdef EMBED_TCL
	/*Tcl_FindExecutable();*/
	inter = Tcl_CreateInterp();
	Tcl_Init(inter);
#endif
#ifdef EMBED_PHP
	char *xxx[2] = {"foo", NULL};
	PHP_EMBED_START_BLOCK(1, xxx);
	zend_eval_string("echo 'hello php-world';", NULL, "guru");
	PHP_EMBED_END_BLOCK();
#endif

	path = (char*)malloc(strlen(datadir) + strlen(EMBEDCONF) + 1);
	strcpy(path, datadir);
	strcat(path, EMBEDCONF);
	handle = ggz_conf_parse(path, GGZ_CONF_RDONLY);
	free(path);
	if(handle < 0) return;
	ret = ggz_conf_read_list(handle, "scripts", "scripts", &count, &aliaslist);
	printf("[ ");
	num = 1;
	for(i = 0; i < count; i++)
	{
		script = ggz_conf_read_string(handle, "scripts", aliaslist[i], NULL);
		if(script)
		{
			scriptlist = (char**)realloc(scriptlist, ++num * sizeof(char*));
			scriptlist[num - 2] = (char*)malloc(strlen(script) + 1);
			typelist = (int*)realloc(typelist, num * sizeof(int));
			strcpy(scriptlist[num - 2], script);
			scriptlist[num - 1] = NULL;
			typelist[num - 2] = mimetype(script);
			printf("|");
		}
		else printf(".");
	}
	printf(" ] ");

	if(ret < 0) scriptlist = NULL;
}

/* Execute embedded scripts */
Guru *gurumod_exec(Guru *message)
{
	char *script = NULL;
	int type;
	int i;
#ifdef EMBED_RUBY
	int status;
	VALUE answer, tmp;
#endif
#ifdef EMBED_PERL
	char *answerstring;
	char *statement;
#endif
#ifdef EMBED_PYTHON
	PyObject *pName, *pModule, *pValue;
	PyMethodDef methods[] = {{NULL, NULL}};
	FILE *f;
#endif

	if(!scriptlist) return NULL;
	if(!message->message) return NULL;

	for(i = 0; scriptlist[i]; i++)
	{
		script = scriptlist[i];
		type = typelist[i];

		if(script)
		{
#ifdef EMBED_RUBY
			if(type == TYPE_RUBY)
			{
				answer = rb_ary_new();
				rb_define_variable("$answer", &answer);
				rb_ary_push(answer, rb_str_new2(message->message));
				/*rb_global_variable(&answer);*/

				ruby_script("grubby-embedded");
				rb_load_file(script);

				switch(fork())
				{
					case -1:
						return NULL;
					case 0:
						ruby_run();
						exit(0);
						break;
					default:
						wait(&status);
				}

				if(!NIL_P(answer))
				{
					if(RARRAY(answer)->len > 0)
					{
						tmp = rb_ary_pop(answer);
						message->message = STR2CSTR(tmp);
						return message;
					}
				}
			}
#endif

#ifdef EMBED_PERL
			if(type == TYPE_PERL)
			{
				char *argv[] = {"modembed", script, NULL};
				perl_parse(my_perl, NULL, 2, argv, (char**)NULL);
				statement = (char*)malloc(strlen(message->message) + 100);
				sprintf(statement, "$answer = \"%s\"", message->message);
				eval_pv(statement, TRUE);
				perl_run(my_perl);
				free(statement);

				answerstring = SvPV(get_sv("answer", FALSE), PL_na);
				if(answerstring)
				{
					message->message = answerstring;
					return message;
				}
			}
#endif

#ifdef EMBED_PYTHON
			if(type == TYPE_PYTHON)
			{
				pName = PyString_FromString(script);

				pModule = Py_InitModule("grubby", methods);
				pValue = Py_BuildValue("s", message->message);
				PyModule_AddObject(pModule, "answer", pValue);
				pxDict = PyModule_GetDict(pModule);

				f = fopen(script, "r");
				PyRun_SimpleFile(f, script);
				fclose(f);

				pValue = PyDict_GetItemString(pxDict, "answer");
				message->message = PyString_AsString(pValue);
				return message;
			}
#endif

#ifdef EMBED_TCL
			if(type == TYPE_TCL)
			{
				Tcl_SetVar(inter, "answer", message->message, 0);

				Tcl_EvalFile(inter, script);

				message->message = strdup(Tcl_GetVar(inter, "answer", 0));
				return message;
			}
#endif
		}
	}

	return NULL;
}

/*
void gurumod_finish()
{
#ifdef EMBED_PERL
perl_destruct(perl);
perl_free(perl);
#endif
#ifdef EMBED_PYTHON
Py_Finalize();
#endif
#ifdef EMBED_TCL
Tcl_DeleteInterp(inter);
#endif
}
*/

