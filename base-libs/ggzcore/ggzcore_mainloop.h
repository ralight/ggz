#ifndef GGZCORE_MAINLOOP_H
#define GGZCORE_MAINLOOP_H

#include <ggzcore.h>

typedef enum {
	GGZCORE_MAINLOOP_READY,
	GGZCORE_MAINLOOP_WAIT,
	GGZCORE_MAINLOOP_ABORT
} GGZCoreMainLoopEvent;

typedef enum {
	GGZCORE_MAINLOOP_CHATONLY,
	GGZCORE_MAINLOOP_LAUNCH,
	GGZCORE_MAINLOOP_JOIN,
	GGZCORE_MAINLOOP_SPECTATE
} GGZCoreMainLoopTableMode;

typedef void (*GGZCoreMainLoopFunc)(GGZCoreMainLoopEvent id, const char *message, GGZServer *server);

typedef struct {
	char *uri;
	int tls;
	int async;
	int reconnect;
	int debug;
	int loop;
	GGZCoreMainLoopFunc func;
	GGZCoreMainLoopTableMode mode;

	GGZServer *server;
	int status;
	char *room;
	char *table;
} GGZCoreMainLoop;

int ggzcore_mainloop_start(GGZCoreMainLoop mainloop);

#endif

