/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ggzcore.h>
#include "exec.h"

char **programlist = NULL;

char *const *optlist(char *cmdline, char *options)
{
	static char **list = NULL;
	int i;
	char *token;
	int listsize;

	if(list)
	{
		i = 0;
		while(list[i])
		{
			free(list[i]);
			i++;
		}
		free(list);
	}

	list = (char**)malloc(2 * sizeof(char*));
	list[0] = (char*)malloc(strlen(cmdline) + 1);
	strcpy(list[0], cmdline);
	list[1] = NULL;
	listsize = 2;
	token = strtok(options, " ,().");
	while(token)
	{
		listsize++;
		list = (char**)realloc(list, listsize * sizeof(char*));
		list[listsize - 2] = (char*)malloc(strlen(token) + 1);
		strcpy(list[listsize - 2], token);
		list[listsize - 1] = NULL;
		token = strtok(NULL, " ,().");
	}

	return list;
}

void simpleexec(const char *cmdline, const char *options)
{
	char *const *opts = optlist(strdup(cmdline), strdup(options));

	execvp(cmdline, opts);
}

char *process(const char *program, Guru *message)
{
	int readfd[2], writefd[2];
	int result1, result2;
	int pid, i;
	static char *buffer = NULL;

	result1 = socketpair(AF_UNIX, SOCK_STREAM, 0, readfd);
	result2 = socketpair(AF_UNIX, SOCK_STREAM, 0, writefd);
	if((result1 == -1) || (result2 == -1))
	{
		return NULL;
	}

	pid = fork();
	switch(pid)
	{
		case -1:
			break;
		case 0:
			dup2(readfd[0], 0);
			dup2(writefd[0], 1);
			simpleexec(program, message->message);
			//guru_admin("Exec failed!");
			exit(-1);
			break;
		default:
			//printf("parent...\n");
			if(!buffer) buffer = (char*)malloc(1024);
			sprintf(buffer, "%s\n", message->message);
			write(readfd[1], buffer, strlen(buffer));
			for(i = 0; i < 1024; i++)
				buffer[i] = 0;
			i = read(writefd[1], buffer, 1024);
			//printf("got answer...\n");
			if(i > 1)
			{
				buffer[strlen(buffer) - 1] = 0;
				return buffer;
			}
			break;
	}
	return NULL;
}

Guru *gurumod_exec(Guru *message)
{
	char *answer;
	int i;
	
	answer = NULL;
	i = 0;
	while((programlist) && (programlist[i]))
	{	
		printf("EXEC: %s\n", programlist[i]);
		answer = process(programlist[i], message);
		if(answer)
		{
			free(message->message);
			message->message = strdup(answer);
			printf(" --> GOT ANSWER: %s\n", answer);
			return message;
		}
		i++;
	}
	return message;
}

void gurumod_init()
{
	int handle;
	char path[1024];
	int count;
	int ret;
int i;

	sprintf(path, "%s/.ggz/guru/modexec.rc", getenv("HOME"));
	handle = ggzcore_confio_parse(path, GGZ_CONFIO_RDONLY);
	if(handle < 0) return;
	ret = ggzcore_confio_read_list(handle, "programs", "programs", &count, &programlist);
printf("ret is: %i\n", ret);
printf("Modules: %i\n", count);
for(i = 0; i < count; i++)
	printf("%s\n", programlist[i]);

	if(ret < 0) programlist = NULL;
}

