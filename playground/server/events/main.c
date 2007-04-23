#include "timer.h"

#include <sys/time.h>
#include <time.h>
#include <stdio.h>

#define DEBUG 0

enum TimerTypes
{
	TIMER_FREQUENT,
	TIMER_OCCASIONAL,
	TIMER_SELDOM
};

static void loop(int milliseconds)
{
	struct timespec req;
	int ret;

	req.tv_sec = milliseconds / 1000;
	req.tv_nsec = (milliseconds % 1000) * 1000000;

#if DEBUG
	printf("Go sleep. [%li:%li]\n", req.tv_sec, req.tv_nsec);
#endif
	ret = nanosleep(&req, &req);
#if DEBUG
	printf("Returned from sleep. [%i]\n", ret);
#endif
}

static void timer_run(void)
{
	nexttimer next;

	next = timer_next();

	printf("(Next: %i @ %i)\n", next.timer, next.milliseconds);

	loop(next.milliseconds);
}

static void timer_callback(int timer)
{
}

int main(int argc, char *argv[])
{
	timer_init(&timer_callback);

	/* All timers in milliseconds! */
	timer_set(TIMER_FREQUENT, 1000);
	timer_set(TIMER_OCCASIONAL, 3000);
	timer_set(TIMER_SELDOM, 15000);

	while(1)
		timer_run();

	return 0;
}

