#include "timer.h"

#include <sys/time.h>
#include <time.h>
#include <stdio.h>

#define DEBUG 0

static float timers[MAX_TIMERS];
static struct timeval timerstarts[MAX_TIMERS];
static timercallback callback = NULL;

void timer_init(timercallback cb)
{
	int i;

	for(i = 0; i < MAX_TIMERS; i++)
		timers[i] = 0.0;

	callback = cb;
}

void timer_set(int timer, float seconds)
{
	if((timer < 0) || (timer >= MAX_TIMERS))
		return;
	if(seconds < 0.0)
		return;

	timers[timer] = seconds;
	gettimeofday(&timerstarts[timer], NULL);
}

nexttimer timer_next(void)
{
	float seconds = 0.0;
	int timer = -1;
	int i;
	struct timeval now, timerstart;
	float remaining, done;
	nexttimer next;

	gettimeofday(&now, NULL);

	for(i = 0; i < MAX_TIMERS; i++)
	{
		if(timers[i] != 0.0)
		{
			timerstart = timerstarts[i];
			done = (float)(now.tv_sec - timerstart.tv_sec);
			done += (float)(now.tv_usec - timerstart.tv_usec) / 1000000.0;
			remaining = timers[i] - done;
#if DEBUG
			printf("=> %i: %3.2f seconds, %3.2f to go\n", i, timers[i], remaining);
#endif

			if((seconds == 0.0) || (remaining < seconds))
			{
				seconds = remaining;
				timer = i;
			}
		}
	}

	/* If this happens, we missed an event */
	if(seconds < 0.0) seconds = 0;

	timerstarts[timer].tv_sec += (int)timers[timer];
	timerstarts[timer].tv_usec += (int)(timers[timer] - (int)timers[timer]) * 1000000;

	next.seconds = seconds;
	next.timer = timer;

	return next;
}

