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

	printf("getaddrinfo...\n");
	if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
		return -1;
	}

	ressave = res;

	printf("check result set...\n");
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

	printf("done...\n");
	return(sockfd);
}

struct conn_t
{
	const char *host;
	int port;
	int fd;
	pthread_mutex_t lock;
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

	printf("FD(async): %i\n", args->fd);
}

static int es_threaded_ready(struct conn_t *args)
{
	int ret = 0;
	pthread_mutex_lock(&args->lock);
	if(args->fd != -1)
		ret = 1;
	pthread_mutex_unlock(&args->lock);
	return ret;
}

static int es_threaded_connect(const char *host, int port)
{
	pthread_t t;
	struct conn_t args;

	printf("-- assign\n");
	args.host = host;
	args.port = port;
	args.fd = -1;
	printf("-- mutex_init\n");
	pthread_mutex_init(&args.lock, NULL);

	printf("-- thread_create\n");
	pthread_create(&t, NULL, connectorthread, &args);

	printf("-- while...\n");
	while(!es_threaded_ready(&args))
	{
		printf("...\n");
		usleep(250000);
	}

	return args.fd;
}

int main(int argc, char *argv[])
{
	int fd;

	setbuf(stdout, NULL);
	fd = es_connect("localhost", 80);
	printf("FD(es_connect): %i\n", fd);
	fd = es_threaded_connect("localhost", 80);
	printf("FD(es_threaded_connect): %i\n", fd);
	return 0;
}

