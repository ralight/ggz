#ifndef TIMER_H
#define TIMER_H

/* Maximum number of timer events (0..MAX_TIMER-1) */
#define MAX_TIMER 10

/* User-defined callback function (unneeded???) */
typedef void (*timercallback)(int);

/* Data type to indicate the next timer event */
struct nexttimer_t
{
	int timer;
	int milliseconds;
};
typedef struct nexttimer_t nexttimer;

/* Initialize the timer functions */
void timer_init(timercallback cb);

/* Set a timer - if milliseconds is zero, deactivate it */
void timer_set(int timer, int milliseconds);

/* Reports the run of the next timer */
nexttimer timer_next(void);

#endif

