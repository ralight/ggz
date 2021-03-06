/*
Compares es_connect() to es_threaded_connect().
*/

#include <sys/types.h>
#include <sys/param.h>

#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>

#include <pthread.h>

/* If set to 1, use select() instead of lock & variable check */
#define INTEGRATION_FD 1

static int es_connect(const char *host, int port)
{
	int sockfd;
	int n;
	struct addrinfo hints, *res, *ressave;
	char serv[30];

	snprintf(serv, sizeof(serv), "%d", (unsigned int)port);

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
		printf("{getaddrinfo failed!}");
		return -1;
	}

	ressave = res;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0)
			continue;
		printf("connect>>\n");
		if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;
		printf("<<connect\n");

		close(sockfd);
		sockfd = -1;

	} while ( (res = res->ai_next) != NULL);

	freeaddrinfo(ressave);

	return(sockfd);
}

struct conn_t
{
	const char *host;
	int port;
	int fd;
	pthread_mutex_t lock;
	int notifyfd;
};

static void *connectorthread(void *arg)
{
	struct conn_t *args;
	int fd;
	
	args = (struct conn_t*)arg;

	fd = es_connect(args->host, args->port);

	pthread_mutex_lock(&args->lock);
	args->fd = fd;
	pthread_mutex_unlock(&args->lock);
	printf("[thread] connection fd: %i\n", args->fd);

#if INTEGRATION_FD
	printf("[thread] notify listeners\n");
	write(args->notifyfd, "!", 1);
	close(args->notifyfd);
#endif
}

static int es_threaded_ready(struct conn_t *args, int pollfd)
{
	int ret = 0;
#if INTEGRATION_FD
	fd_set set;
	struct timeval zeroval;

	FD_ZERO(&set);
	FD_SET(pollfd, &set);
	zeroval.tv_sec = 0;
	zeroval.tv_usec = 0;
	ret = select(pollfd + 1, &set, NULL, NULL, &zeroval);
	if(ret > 0)
	{
		printf("[mainloop-check] Got notification through listener=%i\n", ret);
		if(FD_ISSET(pollfd, &set))
		{
			char buf;
			read(pollfd, &buf, 1);
			printf("[mainloop-check] Message: [%i]\n", (char)buf);
		}
		close(pollfd);
		ret = 1;
	}
	else
	{
		ret = 0;
	}
#else
	pthread_mutex_lock(&args->lock);
	if(args->fd != -1)
		ret = 1;
	pthread_mutex_unlock(&args->lock);
#endif
	return ret;
}

static int es_threaded_connect(const char *host, int port)
{
	pthread_t t;
	struct conn_t args;
	int ret;
	int pollfd = -1;

#if INTEGRATION_FD
	int fd_pair[2];

	ret = socketpair(PF_LOCAL, SOCK_STREAM, 0, fd_pair);
	if(ret != 0)
	{
		printf("{socketpair failed}");
		return -1;
	}
	args.notifyfd = fd_pair[1];

	printf("[mainloop] listener = %i, notifier = %i\n",
		fd_pair[0], fd_pair[1]);
#endif

	args.host = host;
	args.port = port;
	args.fd = -1;
	ret = pthread_mutex_init(&args.lock, NULL);
	if(ret != 0)
	{
		printf("{pthread_mutex_init failed}");
		return -1;
	}

	ret = pthread_create(&t, NULL, connectorthread, &args);
	if(ret != 0)
	{
		printf("{pthread_create failed}");
		return -1;
	}

#if INTEGRATION_FD
	pollfd = fd_pair[0];
#endif

	while(!es_threaded_ready(&args, pollfd))
	{
		printf("[mainloop] ...\n");
		usleep(250000);
	}

	return args.fd;
}

int main(int argc, char *argv[])
{
	int fd;

	setbuf(stdout, NULL);

	printf(">> es_connect (blocking)\n");
	fd = es_connect("localhost", 80);
	printf("<< FD(es_connect): %i\n", fd);
	close(fd);

	printf(">> es_threaded_connect (asynchronous)\n");
	fd = es_threaded_connect("localhost", 80);
	printf("<< FD(es_threaded_connect): %i\n", fd);
	close(fd);

	return 0;
}

