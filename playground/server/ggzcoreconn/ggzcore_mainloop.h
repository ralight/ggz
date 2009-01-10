#ifndef GGZCORE_MAINLOOP_H
#define GGZCORE_MAINLOOP_H

#include <ggzcore.h>

typedef enum {
	GGZCORE_MAINLOOP_READY,
	GGZCORE_MAINLOOP_WAIT,
	GGZCORE_MAINLOOP_ABORT
} GGZCoreMainLoopEvent;

typedef void (*GGZCoreMainLoopFunc)(GGZCoreMainLoopEvent id, const char *message, GGZServer *server);

typedef struct {
	char *uri;
	int tls;
	int async;
	int reconnect;
	int debug;
	int loop;
	GGZCoreMainLoopFunc func;

	GGZServer *server;
	int status;
} GGZCoreMainLoop;

int ggzcore_mainloop_start(GGZCoreMainLoop mainloop);

#endif

