/*******************************************************************
*
* Agrue - A GGZ Real User Emulator
* Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
* Idea based on the original Agrue, code based on Grubby
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "config.h"
#include "net.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define AGRUE_VERSION "0.1"

#define MAX_AGRUES 10000

static pid_t pids[MAX_AGRUES];
static int instances;

static void termhandler(int signum)
{
	int i;

	printf("Agrue: Killing all instances\n");

	for(i = 0; i < instances; i++)
	{
		if(pids[i] != -1)
			kill(pids[i], SIGKILL);
	}

	printf("Agrue: Exit\n");
}

int main(int argc, char *argv[])
{
	char *opthost = NULL;
	int optinstances = 1;
	int optfrequency = 0, optmobility = 0, optactivity = 10;
	int i;
	pid_t pid;
	char name[32];
	int status;
	int instancesleft;

	struct option options[] =
	{
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{"host", required_argument, 0, 'H'},
		{"instances", required_argument, 0, 'i'},
		{"frequency", required_argument, 0, 'f'},
		{"mobility", required_argument, 0, 'm'},
		{"activity", required_argument, 0, 'a'},
		{0, 0, 0, 0}
	};
	int optindex;
	int opt;

	while(1)
	{
		opt = getopt_long(argc, argv, "hvH:i:f:m:a:", options, &optindex);
		if(opt == -1) break;
		switch(opt)
		{
			case 'h':
				printf("Agrue - A GGZ Real User Emulator\n");
				printf("Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>\n");
				printf("Published under GNU GPL conditions\n\n");
				printf("Recognized options:\n");
				printf("[-H | --host     ]: Connect to this host\n");
				printf("[-i | --instances]: Number of Agrue instances to launch (1-%i)\n",
					MAX_AGRUES);
				printf("[-f | --frequency]: Frequency of reconnection (0-100)\n");
				printf("[-m | --mobility ]: Frequency of switching rooms (0-100)\n");
				printf("[-a | --activity ]: Frequency of chatting (0-100)\n");
				printf("[-v | --version  ]: Display version number\n");
				printf("[-h | --help     ]: Show this help screen\n");
				exit(0);
				break;
			case 'H':
				opthost = optarg;
				break;
			case 'v':
				printf("Agrue version %s\n", AGRUE_VERSION);
				exit(0);
				break;
			case 'i':
				optinstances = atoi(optarg);
				break;
			case 'f':
				optfrequency = atoi(optarg);
				break;
			case 'm':
				optmobility = atoi(optarg);
				break;
			case 'a':
				optactivity = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Error: Unknown command line option, try --help.\n");
				exit(-1);
				break;
		}
	}

	if(!opthost)
	{
		fprintf(stderr, "Error: No host given.\n");
		exit(-1);
	}

	if((optinstances < 1) || (optinstances > MAX_AGRUES))
	{
		fprintf(stderr, "Error: Invalid number of instances (1-%i).\n", MAX_AGRUES);
		exit(-1);
	}

	if((optfrequency < 0) || (optfrequency > 100))
	{
		fprintf(stderr, "Error: Invalid frequency (1-100).\n");
		exit(-1);
	}

	if((optmobility < 0) || (optmobility > 100))
	{
		fprintf(stderr, "Error: Invalid mobility (1-100).\n");
		exit(-1);
	}

	if((optactivity < 0) || (optactivity > 100))
	{
		fprintf(stderr, "Error: Invalid activity (1-100).\n");
		exit(-1);
	}

	printf("Agrue: Server is %s\n", opthost);
	printf("Agrue: Set instances to %i\n", optinstances);
	printf("Agrue: Set frequency to %i\n", optfrequency);
	printf("Agrue: Set mobility to %i\n", optmobility);
	printf("Agrue: Set activity to %i\n", optactivity);

	/*signal(SIGTERM, termhandler);
	signal(SIGINT, termhandler);*/

	srand(time(NULL));
	setbuf(stdout, NULL);

	instances = optinstances;
	instancesleft = instances;

	for(i = 0; i < optinstances; i++)
	{
		pid = fork();
		if(pid == 0)
		{
			srand(getpid() * time(NULL));

			snprintf(name, sizeof(name), "agrue-%i", i);

			Agrue *agrue = (Agrue*)malloc(sizeof(Agrue));
			agrue->server = NULL;
			agrue->name = strdup(name);
			agrue->password = NULL;
			agrue->frequency = optfrequency;
			agrue->mobility = optmobility;
			agrue->activity = optactivity;
			agrue->finished = 0;

			printf("(main) [%s] created...\n", agrue->name);

			net_connect(opthost, 5688, agrue);
			net_work(agrue);

			fprintf(stderr, "(main) [%s] Error: Something happened, agrue killed!\n", agrue->name);
			exit(-1);
		}
		else if(pid == -1)
		{
			fprintf(stderr, "(main) Error: Not enough resources for more agrues!\n");
			exit(-1);
		}
		else
		{
			pids[i] = pid;
		}
	}

	while(instancesleft > 0)
	{
		pid = wait(&status);
		if(pid == -1)
		{
			fprintf(stderr, "(main) Error: Cannot wait for agrues, kill them all!\n");
			//termhandler();
			exit(-1);
		}

		for(i = 0; i < instances; i++)
		{
			if(pids[i] == pid)
			{
				pids[i] = -1;
				instancesleft -= 1;
				break;
			}
		}
		printf("(main) Child %i killed with status %i; %i agrues left\n", pid, status, instancesleft);
	}

	printf("Agrue: finished.\n");

	return 0;
}

