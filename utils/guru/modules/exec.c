/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "gurumod.h"

void simpleexec(const char *cmdline)
{
	char *const opts[] = {(char*)cmdline, NULL};

	execvp(cmdline, opts);
}

void process(const char *program, Guru *message)
{
	static char *buffer = NULL;
	int readfd[2], writefd[2];
	int result1, result2;
	int pid;

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
			simpleexec(program);
			//guru_admin("Exec failed!");
			exit(-1);
			break;
		default:
			if(!buffer)
			{
				buffer = (char*)malloc(strlen(message->message) + 2);
				sprintf(buffer, "%s\n", message->message);
			}
			write(readfd[1], buffer, strlen(buffer));
			read(writefd[1], buffer, 1024);
			buffer[strlen(buffer) - 1] = 0;
			message->message = buffer;
			break;
	}
}

Guru gurumod_exec(Guru message)
{
	process("../data/funky.pl", &message);
	return message;
}

void gurumod_init()
{
}

