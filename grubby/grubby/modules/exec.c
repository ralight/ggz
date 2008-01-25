/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001, 2002 Josef Spillner, <dr_maux@users.sourceforge.net>
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
#include <ggz.h>
#include <fcntl.h>
#include <time.h>

#include "gurumod.h"

/* Configuration with list of external programs */
#define EXECCONF "/grubby/modexec.rc"
/* Don't wait more than this for external programs to return*/
#define MAXDELAY 8
/* Maximum size of one line */
#define MAXSIZE 1024

/* Plugin-global lists of program names */
char **aliaslist = NULL;
char **programlist = NULL;

/* Build up an option list for the simpleexec function */
static char *const *optlist(char *cmdline, char *options)
{
	static char **list = NULL;
	int i;
	char *token;
	int listsize;

	/* Free list first if already initialized*/
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

	/* Create list from words in the option string */
	list = (char**)malloc(2 * sizeof(char*));
	list[0] = (char*)malloc(strlen(cmdline) + 1);
	strcpy(list[0], cmdline);
	list[1] = NULL;
	listsize = 2;
	token = strtok(options, " ,.");
	while(token)
	{
		listsize++;
		list = (char**)realloc(list, listsize * sizeof(char*));
		list[listsize - 2] = (char*)malloc(strlen(token) + 1);
		strcpy(list[listsize - 2], token);
		list[listsize - 1] = NULL;
		token = strtok(NULL, " ,.");
	}

	return list;
}

/* Just execute a program without any checking */
static void simpleexec(const char *cmdline, const char *player)
{
	char *const *opts = optlist((char*)cmdline, (char*)player);

	execvp(cmdline, opts);
}

/* Try given program with given message */
/* FIXME: Check further for errors and interrupts */
static char *process(const char *program, Guru *message)
{
	int fd[2];
	int result;
	int pid, i;
	static char *readbuffer = NULL;
	static char *writebuffer = NULL;
	time_t start;

	/* Create socketpair first for communication */
	result = socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
	if(result == -1)
	{
		return NULL;
	}

	/* Prepare buffers */
	if(!readbuffer) readbuffer = (char*)malloc(MAXSIZE);
	if(!writebuffer) writebuffer = (char*)malloc(MAXSIZE);
	for(i = 0; i < MAXSIZE; i++)
		readbuffer[i] = 0;
	snprintf(writebuffer, MAXSIZE, "%s\n", message->message);
	start = time(NULL);

	/* Create child process and watch it */
	pid = fork();
	switch(pid)
	{
		case -1:
			break;
		case 0:
			dup2(fd[0], 0);
			dup2(fd[0], 1);
			simpleexec(program, message->player);
			exit(-1);
			break;
		default:
			fcntl(fd[1], F_SETFL, O_NONBLOCK);
			write(fd[1], writebuffer, strlen(writebuffer));
			/*printf("==> (%s)\n", message->message);*/
			i = read(fd[1], readbuffer, MAXSIZE);
			if(i != -1) /*printf("FIRST: %i\n", i)*/;
			else i = -2;
			while((waitpid(pid, NULL, WNOHANG) == 0) && (time(NULL) - start < MAXDELAY) && (i <= 1))
			{
				i = read(fd[1], readbuffer, MAXSIZE);
			/*if(i != -1) printf("NOW: %i\n", i);*/
			}
			/*printf("<== (%i)\n", i);*/
			if(i > 1)
			{
				//readbuffer[strlen(readbuffer) - 1] = 0;
				return readbuffer;
			}
			break;
	}
	return NULL;
}

/* Reveive message and pass it to all external programs */
Guru *gurumod_exec(Guru *message)
{
	char *answer;
	int i;
	
	answer = NULL;
	i = 0;
	while((programlist) && (programlist[i]))
	{	
		/*printf("EXEC: %s (%s)\n", programlist[i], message->message);*/
		answer = process(programlist[i], message);
		if(answer)
		{
			message->message = strdup(answer);
			/*printf(" --> GOT ANSWER: '%s'\n", answer);*/
			return message;
		}
		i++;
	}
	return NULL;
}

/* Initialize this plugin */
void gurumod_init(const char *datadir)
{
	int handle;
	char *path;
	int count;
	int ret;
	int i;
	char *program;
	int num;

	path = (char*)malloc(strlen(datadir) + strlen(EXECCONF) + 1);
	strcpy(path, datadir);
	strcat(path, EXECCONF);
	handle = ggz_conf_parse(path, GGZ_CONF_RDONLY);
	free(path);
	if(handle < 0) return;
	ret = ggz_conf_read_list(handle, "programs", "programs", &count, &aliaslist);
	printf("[ ");
	num = 1;
	for(i = 0; i < count; i++)
	{
		program = ggz_conf_read_string(handle, "programs", aliaslist[i], NULL);
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

