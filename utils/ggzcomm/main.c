/* Include files */
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ggzcommgen.h"

/* Main function */
int main(int argc, char *argv[])
{
	char *opt_protocol = NULL;
	char *opt_task = NULL;
	char *opt_format = NULL;
	char *opt_library = NULL;
	char *opt_mode = NULL;
	struct option options[] =
	{
		{"protocol", required_argument, 0, 'p'},
		{"task", required_argument, 0, 't'},
		{"format", required_argument, 0, 'f'},
		{"version", no_argument, 0, 'v'},
		{"help", no_argument, 0, 'h'},
		{"library", required_argument, 0, 'l'},
		{"mode", required_argument, 0, 'm'},
		{0, 0, 0, 0}
	};
	int optindex;
	int opt;

	/* Parse all options */
	while(1)
	{
		opt = getopt_long(argc, argv, "hvf:t:p:l:m:", options, &optindex);
		if(opt == -1) break;
		switch(opt)
		{
			case 'l':
				opt_library = optarg;
				break;
			case 'p':
				opt_protocol = optarg;
				break;
			case 't':
				opt_task = optarg;
				break;
			case 'f':
				opt_format = optarg;
				break;
			case 'm':
				opt_mode = optarg;
				break;
			case 'v':
				printf("GGZCommGen version 0.1\n");
				fflush(NULL);
				exit(0);
				break;
			case 'h':
				printf("GGZCommGen - GGZ Gaming Zone protocol generator\n");
				printf("Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net\n");
				printf("Published under GNU GPL conditions\n");
				printf("\n");
				printf("[-h | --help     ] - This help screen\n");
				printf("[-v | --version  ] - Display program version\n");
				printf("[-f | --format   ] - Output format: c, c++\n");
				printf("[-p | --protocol ] - Protocol definition file\n");
				printf("[-t | --task     ] - Task of the generated files: server, client\n");
				printf("[-l | --library  ] - Networking library: libggz (default), qt\n");
				printf("[-m | --mode     ] - Generation mode: simple (default), logic, full\n");
				fflush(NULL);
				exit(0);
		}
	}

	/* Check for missing arguments */
	if(!opt_protocol)
	{
		fprintf(stderr, "Missing argument: protocol\n");
		exit(-1);
	}
	if(!opt_format)
	{
		fprintf(stderr, "Missing argument: format\n");
		exit(-1);
	}
	if(!opt_task)
	{
		fprintf(stderr, "Missing argument: task\n");
		exit(-1);
	}
	if(!opt_library)
	{
		opt_library = "libggz";
	}
	if(!opt_mode)
	{
		opt_mode = "simple";
	}

	/* Check for argument validity */
	if(strcmp(opt_format, "c") && strcmp(opt_format, "c++"))
	{
		fprintf(stderr, "Wrong argument: format is currently only 'c' or 'c++'\n");
		exit(-1);
	}
	if(strcmp(opt_task, "client") && strcmp(opt_task, "server"))
	{
		fprintf(stderr, "Wrong argument: task can only be 'server' or 'client'\n");
		exit(-1);
	}
	if(strcmp(opt_library, "libggz") && strcmp(opt_library, "qt"))
	{
		fprintf(stderr, "Wrong argument: library is currently only 'libggz' or 'qt'\n");
		exit(-1);
	}
	if(strcmp(opt_mode, "simple") && strcmp(opt_mode, "logic") && strcmp(opt_mode, "full"))
	{
		fprintf(stderr, "Wrong argument: mode can only be 'simple' or 'logic' or 'full'\n");
		exit(-1);
	}

	/* Check for argument logics */
	if(!strcmp(opt_library, "qt") && !strcmp(opt_format, "c"))
	{
		fprintf(stderr, "Silly argument: library 'qt' doesn't work with format 'c'\n");
		exit(-1);
	}

	/* Do all the work */
	ggzcommgen_generate(opt_protocol, opt_task, opt_format, opt_library, opt_mode);

	return 0;
}

