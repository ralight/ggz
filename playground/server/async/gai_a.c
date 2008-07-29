#define LOOKUP_ASYNC 1 /* SYNC otherwise */
#define LOOKUP_THREAD 1 /* SIGNAL otherwise */

#define TRIGGER_BUG 1 /* glibc gai_a bug before 2.3.7 */

#define _GNU_SOURCE

#include <stdio.h>
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#if TRIGGER_BUG
# if !LOOKUP_THREAD || !LOOKUP_ASYNC
#  error Bug will not be triggered even though requested
# endif
#else
# warning Bug will not be triggered
#endif

#if !TRIGGER_BUG
static struct gaicb *global_req = NULL;
#endif

static void print_address(struct addrinfo *res)
{
	char hostname[64];
	int ret;

	/* Display a single result */
	ret = getnameinfo(res->ai_addr, res->ai_addrlen,
		hostname, sizeof(hostname), NULL, 0, NI_NUMERICHOST);
	if(ret)
	{
		printf("Error (gni %i): %s\n", ret, gai_strerror(ret));
		return;
	}
	printf(" -> [%s]\n", hostname);
}

static void print_addresses(struct addrinfo *res)
{
	struct addrinfo *ptr;

	printf("Results:\n");

	/* Iterate over result list */
	for(ptr = res; ptr; ptr = ptr->ai_next)
	{
		print_address(ptr);
	}

	/* Free list */
	freeaddrinfo(res);
}

static void check_error(struct gaicb *req)
{
	int ret = gai_error(req);

	if(ret)
	{
#if LOOKUP_ASYNC
		if(ret != EAI_INPROGRESS)
		{
			printf("Error: %s\n", gai_strerror(ret));
		}
#else
		printf("Error: %s\n", gai_strerror(ret));
#endif
	}
}

#if LOOKUP_THREAD
static void found_thread(sigval_t arg)
{
	printf("callback in new thread!\n");

	struct gaicb *req;

	if(arg.sival_ptr)
	{
#if TRIGGER_BUG
		req = (struct gaicb*)arg.sival_ptr;
#else
		req = global_req;
#endif
		check_error(req);
		print_addresses(req->ar_result);

		/*free(arg.sival_ptr);*/
	}
}
#endif

#if !LOOKUP_THREAD
static void found_alarm(int signum)
{
	signal(SIGALRM, SIG_DFL);

	printf("callback in signal!\n");

	struct gaicb *req;

#if !TRIGGER_BUG
	req = global_req;
	check_error(req);
	print_addresses(req->ar_result);
#else
# error Dunno how to get req here?!
#endif
}
#endif

int main(int argc, char *argv[])
{
	const char *host;
	int ret;

	struct sigevent sigev;
	struct gaicb *req;

	if(argc == 1)
	{
		host = "localhost";
	}
	else
	{
		host = argv[1];
	}

#if !LOOKUP_THREAD
	signal(SIGALRM, found_alarm);
#endif
	setbuf(stdout, NULL);

	struct addrinfo *addrinforesult = (struct addrinfo*)malloc(sizeof(struct addrinfo));

	/* Create request */
	req = (struct gaicb*)malloc(sizeof(struct gaicb));
	req->ar_name = strdup(host);
	req->ar_service = NULL;
	req->ar_request = NULL;
	req->ar_result = addrinforesult;

	/* FIXME: the host duplication and the allocation of addrinforesult
	 * and its assignment to ar_result were added recently but do not help. */

#if LOOKUP_THREAD
	sigev.sigev_notify = SIGEV_THREAD;
	sigev.sigev_value.sival_ptr = NULL;
	sigev.sigev_notify_function = found_thread;
	sigev.sigev_notify_attributes = NULL;
#else
	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_value.sival_ptr = req;
	sigev.sigev_signo = SIGALRM;
#endif

#if !TRIGGER_BUG
	global_req = req;
#endif

	printf("GAI test: mode=[%s] method=[%s]\n",
		(LOOKUP_ASYNC ? "async" : "sync"),
		(LOOKUP_THREAD ? "thread" : "signal"));

	/* Perform lookup */
	printf("entering getaddr_a call!\n");
#if LOOKUP_ASYNC
	ret = getaddrinfo_a(GAI_NOWAIT, &req, 1, &sigev);
#else
	ret = getaddrinfo_a(GAI_WAIT, &req, 1, &sigev);
#endif
	printf("returned from getaddr_a call!\n");

	if(ret)
	{
		/* An immediate (enqueueing) error occured */
		printf("Error: %s\n", gai_strerror(ret));
	}

#if !LOOKUP_ASYNC
	check_error(req);
	print_addresses(req->ar_result);
#endif

#if LOOKUP_ASYNC
	printf("go sleep...\n");
	sleep(3);
#endif

	free(req);

	return 0;
}

