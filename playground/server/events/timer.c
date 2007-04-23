#include "timer.h"

#include <sys/time.h>
#include <time.h>
#include <stdio.h>

#define DEBUG 0

static int timers[MAX_TIMER];
static struct timeval timerstarts[MAX_TIMER];
static timercallback callback = NULL;

void timer_init(timercallback cb)
{
	int i;

	for(i = 0; i < MAX_TIMER; i++)
		timers[i] = 0;

	callback = cb;
}

void timer_set(int timer, int milliseconds)
{
	if((timer < 0) || (timer >= MAX_TIMER))
		return;
	if(milliseconds < 0)
		return;

	timers[timer] = milliseconds;
	gettimeofday(&timerstarts[timer], NULL);
}

nexttimer timer_next(void)
{
	int milliseconds = 0;
	int timer = -1;
	int i;
	struct timeval now, timerstart;
	int remaining, done;
	nexttimer next;

	gettimeofday(&now, NULL);

	for(i = 0; i < MAX_TIMER; i++)
	{
		if(timers[i] != 0)
		{
			timerstart = timerstarts[i];
			done = (now.tv_sec - timerstart.tv_sec) * 1000;
			done += (now.tv_usec - timerstart.tv_usec) / 1000;
			remaining = timers[i] - done;
#if DEBUG
			printf("=> %i: %i milliseconds, %i to go\n", i, timers[i], remaining);
#endif

			if((milliseconds == 0) || (remaining < milliseconds))
			{
				milliseconds = remaining;
				timer = i;
			}
		}
	}

	/* If this happens, we missed an event */
	if(milliseconds < 0) milliseconds = 0;

	timerstarts[timer].tv_sec += timers[timer] / 1000;
	timerstarts[timer].tv_usec += (timers[timer] % 1000) * 1000000;

	next.milliseconds = milliseconds;
	next.timer = timer;

	return next;
}

