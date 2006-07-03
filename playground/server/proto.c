#include <stdio.h>
#define _XOPEN_SOURCE 500
#include <pthread.h>

#include "data.h"

static void *watcher(void *arg)
{
		coregen_watcher_init();

		coregen_set_f_bar(1);
		coregen_set_f_baz(2);
		coregen_get_f_bar();

		while(1);
}

static void *action(void *arg)
{
		coregen_action_init();

		/* this is evil */
		coregen_set_f_bar(1);
		coregen_get_f_bar();

		while(1);
}

static void initthreads()
{
	pthread_t t_watcher, t_action;

	coregen_init();
	pthread_create(&t_watcher, NULL, &watcher, NULL);
	pthread_create(&t_action, NULL, &action, NULL);

	printf("control thread [%lu]\n", pthread_self());

	while(1);
}

int main(int argc, char *argv[])
{
		initthreads();
		return 0;
}

