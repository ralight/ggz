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
#include "gurumod.h"

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

	list = (char**)malloc(2);
	list[0] = (char*)malloc(strlen(cmdline) + 1);
	strcpy(list[0], cmdline);
	list[1] = NULL;
	listsize = 2;
	token = strtok(options, " ,().");
	while(token)
	{
		listsize++;
		list = (char**)realloc(list, listsize);
		list[listsize - 2] = (char*)malloc(strlen(token) + 1);
		strcpy(list[listsize - 2], token);
		list[listsize - 1] = NULL;
		token = strtok(NULL, " ,().");
	}

	return list;
}

void simpleexec(const char *cmdline, const char *options)
{
	char *const *opts = optlist((char*)cmdline, (char*)options);

	execvp(cmdline, opts);
}

void process(const char *program, Guru *message)
{
	int readfd[2], writefd[2];
	int result1, result2;
	int pid, i;
	static char *buffer = NULL;

	result1 = socketpair(AF_UNIX, SOCK_STREAM, 0, readfd);
	result2 = socketpair(AF_UNIX, SOCK_STREAM, 0, writefd);
	if((result1 == -1) || (result2 == -1))
	{
		message->message = "Socketpair() failed.";
		message->type = GURU_ADMIN;
		return;
	}

	pid = fork();
	switch(pid)
	{
		case -1:
			message->message = "Couldn't fork!";
			message->type = GURU_ADMIN;
			break;
		case 0:
			dup2(readfd[0], 0);
			dup2(writefd[0], 1);
			simpleexec(program, message->message);
			//guru_admin("Exec failed!");
			exit(-1);
			break;
		default:
			if(!buffer) buffer = (char*)malloc(1024);
			sprintf(buffer, "%s\n", message->message);
			write(readfd[1], buffer, strlen(buffer));
			for(i = 0; i < 1024; i++)
				buffer[i] = 0;
			read(writefd[1], buffer, 1024);
			buffer[strlen(buffer) - 1] = 0;
			message->message = buffer;
			break;
	}
}

Guru gurumod_exec(Guru message)
{
	process("../data/rdf.pl", &message);
	return message;
}

void gurumod_init()
{
}

