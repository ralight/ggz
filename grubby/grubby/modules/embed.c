/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001, 2002 Josef Spillner, <dr_maux@users.sourceforge.net>
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
#include <ruby.h>
#endif
#ifdef EMBED_PERL
#include <EXTERN.h>
#include <perl.h>
#endif

#define EMBEDCONF "/grubby/modembed.rc"

static char **aliaslist = NULL;
static char **scriptlist = NULL;

#ifdef EMBED_PERL
static PerlInterpreter *my_perl;
#endif

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
			strcpy(scriptlist[num - 2], script);
			scriptlist[num - 1] = NULL;
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
	int i;
#ifdef EMBED_RUBY
	int status;
	VALUE answer, tmp;
#endif
#ifdef EMBED_PERL
	char *answerstring;
	int tmpval;
#endif

	if(!scriptlist) return NULL;
	if(!message->message) return NULL;

	for(i = 0; scriptlist[i]; i++)
	{
		script = scriptlist[i];

#ifdef EMBED_RUBY
		if(script)
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
		char *argv[] = {script, NULL};
		perl_parse(my_perl, NULL, 2, argv, (char**)NULL);
		/*set_sv("answer", message->message)*/
		perl_run(my_perl);

		answerstring = SvPV(get_sv("answer", FALSE), tmpval);
		if(answerstring)
		{
			message->message = answerstring;
			return message;
		}
#endif
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
}
*/

