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
#include <sys/wait.h>
#include <unistd.h>
#include <ggzcore.h>
#include <fcntl.h>
#include <time.h>
#include "exec.h"

#define MAXDELAY 8

char **aliaslist = NULL;
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
	/*char *const *opts = optlist(strdup(cmdline), strdup(options));*/
	char *const *opts = optlist((char*)cmdline, NULL);

	execvp(cmdline, opts);
}

char *process(const char *program, Guru *message)
{
	int fd[2];
	int result;
	int pid, i;
	static char *readbuffer = NULL;
	static char *writebuffer = NULL;
	time_t start;

	result = socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
	if(result == -1)
	{
		return NULL;
	}

	/* prepare buffers */
	if(!readbuffer) readbuffer = (char*)malloc(1024);
	if(!writebuffer) writebuffer = (char*)malloc(1024);
	for(i = 0; i < 1024; i++)
		readbuffer[i] = 0;
	sprintf(writebuffer, "%s\n", message->message);
	start = time(NULL);

	pid = fork();
	switch(pid)
	{
		case -1:
			break;
		case 0:
			dup2(fd[0], 0);
			dup2(fd[0], 1);
			simpleexec(program, message->message);
			exit(-1);
			break;
		default:
			fcntl(fd[1], F_SETFL, O_NONBLOCK);
			write(fd[1], writebuffer, strlen(writebuffer));
			printf("==> (%s)\n", message->message);
			/*fcntl(fd[1], F_SETFL, O_NONBLOCK);*/
			i = read(fd[1], readbuffer, 1024);
if(i != -1) printf("FIRST: %i\n", i);
else i = -2;
			while((waitpid(pid, NULL, WNOHANG) == 0) && (time(NULL) - start < MAXDELAY) && (i <= 1))
			{
				i = read(fd[1], readbuffer, 1024);
if(i != -1) printf("NOW: %i\n", i);
			}
			printf("<== (%i)\n", i);
			if(i > 1)
			{
				//readbuffer[strlen(readbuffer) - 1] = 0;
				return readbuffer;
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
		printf("EXEC: %s (%s)\n", programlist[i], message->message);
		answer = process(programlist[i], message);
		if(answer)
		{
			message->message = strdup(answer);
			printf(" --> GOT ANSWER: '%s'\n", answer);
			return message;
		}
		i++;
	}
	return NULL;
}

void gurumod_init()
{
	int handle;
	char path[1024];
	int count;
	int ret;
	int i;
	char *program;
	int num;

	sprintf(path, "%s/.ggz/guru/modexec.rc", getenv("HOME"));
	handle = ggzcore_confio_parse(path, GGZ_CONFIO_RDONLY);
	if(handle < 0) return;
	ret = ggzcore_confio_read_list(handle, "programs", "programs", &count, &aliaslist);
	printf("[ ");
	num = 1;
	for(i = 0; i < count; i++)
	{
		program = ggzcore_confio_read_string(handle, "programs", aliaslist[i], NULL);
		if(program)
		{
			programlist = (char**)realloc(programlist, ++num * sizeof(char*));
			programlist[num - 2] = (char*)malloc(strlen(program) + 1);
			strcpy(programlist[num - 2], program);
			programlist[num - 1] = NULL;
			printf("|");
		}
		else printf(".");
	}
	printf(" ] ");

	if(ret < 0) programlist = NULL;
}

