/* This test tool has been derived from ggz-cmd. */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

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
		.func = NULL
	};

	struct option opts[] = {
		{"tls", 0, &mainloop.tls, 1},
		{"reconnect", 0, &mainloop.reconnect, 1},
		{"async", 0, &mainloop.async, 1},
		{"debug", 0, &mainloop.debug, 1},
		{"loop", 0, &mainloop.loop, 1},
		{0, 0, 0, }
	};
	int index;
	int valid = 1;

	while(1) {
		int ret = getopt_long(argc, argv, "", opts, &index);
		if(ret == -1) {
			break;
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

	if(!valid) {
		fprintf(stderr, "Syntax: ggzcoreconn [--tls] [--reconnect] [--async] [--loop] [--debug] <GGZ URI>\n");
		mainloop.uri = NULL;
	} else {
		printf("Connection: tls=%i async=%i reconnect=%i loop=%i uri=%s\n",
			mainloop.tls, mainloop.async, mainloop.reconnect, mainloop.loop, mainloop.uri);
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
