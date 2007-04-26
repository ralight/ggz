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

static void loop(float seconds)
{
	struct timespec req;
	int ret;

	req.tv_sec = (int)seconds;
	req.tv_nsec = seconds - req.tv_sec;

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

	printf("(Next: %i @ %3.2f)\n", next.timer, next.seconds);

	loop(next.seconds);
}


/*static void timer_callback(int timer)
{
}*/

int main(int argc, char *argv[])
{
	timer_init(/*&timer_callback*/NULL);

	/* All timers in fractional seconds now! */
	timer_set(TIMER_FREQUENT, 2.300);
	timer_set(TIMER_OCCASIONAL, 6.000);
	timer_set(TIMER_SELDOM, 15.000);

	while(1)
		timer_run();

	return 0;
}

