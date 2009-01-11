/* This test tool has been derived from ggz-cmd. */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "ggzcore_mainloop.h"

static GGZCoreMainLoop parse(int argc, char **argv)
{
	GGZCoreMainLoop mainloop = {
		.uri = NULL,
		.tls = 0,
		.async = 0,
		.reconnect = 0,
		.debug = 0,
		.loop = 0,
		.func = NULL,
		.mode = GGZCORE_MAINLOOP_CHATONLY
	};

	struct option opts[] = {
		{"tls", no_argument, &mainloop.tls, 1},
		{"reconnect", no_argument, &mainloop.reconnect, 1},
		{"async", no_argument, &mainloop.async, 1},
		{"debug", no_argument, &mainloop.debug, 1},
		{"loop", no_argument, &mainloop.loop, 1},
		{"playmode", required_argument, NULL, 'p'},
		{0, 0, 0, }
	};
	int index;
	int valid = 1;

	char *playmode = NULL;

	while(1) {
		int ret = getopt_long(argc, argv, "", opts, &index);
		if(ret == -1) {
			break;
		} else if(ret == 'p') {
			playmode = optarg;
		} else if(ret == 0) {
			/* mainloop configuration */
		} else {
			valid = 0;
		}
	}

	if(optind == argc - 1) {
		mainloop.uri = argv[optind];
	} else {
		fprintf(stderr, "ERROR: Missing URL.\n");
		valid = 0;
	}

	if(!playmode) {
		/* use defaults */
	} else if(!strcmp(playmode, "launch")) {
		mainloop.mode = GGZCORE_MAINLOOP_LAUNCH;
	} else if(!strcmp(playmode, "join")) {
		mainloop.mode = GGZCORE_MAINLOOP_JOIN;
	} else if(!strcmp(playmode, "spectate")) {
		mainloop.mode = GGZCORE_MAINLOOP_SPECTATE;
	} else {
		fprintf(stderr, "ERROR: Invalid playmode '%s'.\n", playmode);
		valid = 0;
	}

	if(!valid) {
		fprintf(stderr, "Syntax: ggzcoreconn [--tls] [--reconnect] [--async] [--loop] [--debug] [--playmode <mode>] <GGZ URI>\n");
		fprintf(stderr, "Play modes: launch, join, spectate\n");
		mainloop.uri = NULL;
	} else {
		printf("Connection: tls=%i async=%i reconnect=%i loop=%i uri=%s mode=%s\n",
			mainloop.tls, mainloop.async, mainloop.reconnect, mainloop.loop, mainloop.uri, playmode);
	}

	return mainloop;
}

static void mainloopfunc(GGZCoreMainLoopEvent id, const char *message, GGZServer *server)
{
	if(id == GGZCORE_MAINLOOP_READY) {
		printf("READY: %s\n", message);
	} else if(id == GGZCORE_MAINLOOP_WAIT) {
		fprintf(stderr, "WARNING: %s\n", message);
	} else if(id == GGZCORE_MAINLOOP_ABORT) {
		fprintf(stderr, "ERROR: %s\n", message);
	}
}

int main(int argc, char **argv)
{
	GGZCoreMainLoop mainloop = parse(argc, argv);
	if(!mainloop.uri)
		return -1;

	mainloop.func = mainloopfunc;

	return ggzcore_mainloop_start(mainloop);
}
