/* GGZWrap - Generic game client/server wrapper for the GGZ Gaming Zone
 * Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* Include files */
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

/* Global variables */
int fdin = -1;
int fdout = -1;
char *exec = NULL;

/* Execute a program with the given argument vector */
void simpleexec(const char *argvec)
{
	char *tok;
	char **l;
	int i;

	tok = strtok(strdup(argvec), " ");
	if(!tok) return;

	l = (char**)malloc(sizeof(char*) * 2);
	l[0] = (char*)malloc(sizeof(char*));
	l[1] = (char*)malloc(sizeof(char*));
	l[0] = tok;

	i = 1;
	while(tok)
	{
		tok = strtok(NULL, " ");
		l = (char**)realloc(l, i + 2);
		l[i + 1] = (char*)malloc(sizeof(char*));
		l[i] = tok;
		i++;
	}
	l[i] = NULL;
	
	execvp(l[0], l);
}

/* Launch a child program and reassign some fd's */
void startup(int fdin, int fdout, const char *exec)
{
	int fd[2];
	int res;
	char buffer[1024];

	if(socketpair(AF_UNIX, SOCK_STREAM, 0, fd) == -1)
	{
		fprintf(stderr, "Couldn't create a pipe.\n");
		exit(-1);
	}

	switch(fork())
	{
		case 0:
			/* Assign the file descriptors */
			dup2(fd[0], fdin);
			dup2(fd[0], fdout);

			/* Try to start the child program */
			simpleexec(exec);
			fprintf(stderr, "Execution failed.\n");
			exit(-1);
			break;
		case -1:
			fprintf(stderr, "Fork failed.\n");
			exit(-1);
			break;
		default:
			/* Non-blocking operation */
			fcntl(fd[1], F_SETFL, O_NONBLOCK);
			fcntl(3, F_SETFL, O_NONBLOCK);

			while(1)
			{
				/* Handle child program messages */
				res = read(fd[1], buffer, sizeof(buffer));
				if((res == -1) && (errno != EAGAIN))
				{
					fprintf(stderr, "Read error from child.\n");
					exit(-1);
				}
				else if(res > 0)
				{
					/*printf("(%i) %s\n", res, buffer);*/
					fprintf(stderr, ">> %i bytes child -> server\n", res);
					write(3, buffer, res);
				}

				/* Handle server messages */
				res = read(3, buffer, sizeof(buffer));
				if((res == -1) && (errno != EAGAIN))
				{
					fprintf(stderr, "Read error from server.\n");
					exit(-1);
				}
				else if(res > 0)
				{
					/*printf("[%i] %s\n", res, buffer);*/
					fprintf(stderr, ">> %i bytes server -> child\n", res);
					write(fd[1], buffer, res);
				}
			}
			break;
	}
}

/* Signal handler */
void sighandler(int foo)
{
	int status;

	wait(&status);
	exit(-1);
}

/* Main function */
int main(int argc, char *argv[])
{
	struct option options[] =
	{
		{"exec", required_argument, 0, 'e'},
		{"fdin", required_argument, 0, 'i'},
		{"fdout", required_argument, 0, 'o'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0}
	};
	int optindex;
	char opt;

	/* Parse all options */
	while(1)
	{
		opt = getopt_long(argc, argv, "he:i:o:", options, &optindex);
		if(opt == -1) break;
		switch(opt)
		{
			case 'e':
				exec = optarg;
				break;
			case 'i':
				fdin = atoi(optarg);
				break;
			case 'o':
				fdout = atoi(optarg);
				break;
			case 'h':
				printf("GGZWrap - generic GGZ game client/server wrapper\n");
				printf("Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net\n");
				printf("Published under GNU GPL conditions\n");
				printf("\n");
				printf("[-h | --help           ] - This help screen\n");
				printf("[-e | --exec  <program>] - Execute this program\n");
				printf("[-i | --fdin  <fd>     ] - Input file descriptor\n");
				printf("[-o | --fdout <fd>     ] - Output file descriptor\n");
				exit(0);
		}
	}

	/* Check for argument validity */
	if((fdin < 0) || (fdout < 0) || (!exec))
	{
		fprintf(stderr, "Wrong parameters, try %s --help.\n", argv[0]);
		exit(-1);
	}

	/* Install the signal handler */
	signal(SIGCHLD, sighandler);

	/* Try to launch the child program */
	startup(fdin, fdout, exec);

	return 0;
}

