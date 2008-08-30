#include <ggz.h>
#include <stdio.h>
#include <unistd.h>

static int didresolve = 0;

static void callback(const char *address, int socket)
{
	printf("* [resolver: %s][socket: %i]\n",
		address, socket);
}

void do_something()
{
	while (!didresolve) {
		printf("...\n");
		usleep(250000);

		int fd, ret;
		fd_set rfd;
		GGZAsyncEvent event;
		struct timeval zeroval;

		fd = ggz_async_fd();
		FD_ZERO(&rfd);
		FD_SET(fd, &rfd);
		zeroval.tv_sec = 0;
		zeroval.tv_usec = 0;
		ret = select(fd + 1, &rfd, NULL, NULL, &zeroval);
		if (ret > 0) {
			printf("- asyncns query resolved something\n");
			event = ggz_async_event();
			switch(event.type) {
				case GGZ_ASYNC_NOOP:
					printf("= noop\n");
					break;
				case GGZ_ASYNC_RESOLVER:
					printf("= resolved to %s\n", event.address);
					break;
				case GGZ_ASYNC_CONNECT:
					printf("= connect to %s [%i]\n", event.address, event.socket);
					break;
			}
			didresolve = 1;
		}
	}
}

int main(int argc, char *argv[])
{
	int ret;
	char *host;
	GGZResolveType type;
	const char *resolvedhost;

	if(argc == 1)
	{
		host = "localhost";
	}
	else
	{
		host = argv[1];
	}

	setbuf(stdout, NULL);

	ggz_set_network_notify_func(callback);

	printf(">> resolve synchronously %s\n", host);
	resolvedhost = ggz_resolvename(host, GGZ_RESOLVE_SYNC);
	if(resolvedhost)
	{
		printf(">> resolved in blocking mode to %s\n", resolvedhost);
		ggz_free(resolvedhost);
	}
	else
	{
		printf("ERROR: should never get here\n");
	}
	printf("\n");

	printf(">> resolve asynchronously %s, with fallback\n", host);
	didresolve = 0;
	resolvedhost = ggz_resolvename(host, GGZ_RESOLVE_TRYASYNC);
	if(resolvedhost)
	{
		printf(">> resolved in blocking mode to %s\n", resolvedhost);
		ggz_free(resolvedhost);
	}
	else
	{
		do_something();
		printf(">> oh good! name resolved asynchronously!\n");
	}
	printf("\n");

	printf(">> resolve asynchronously %s, without fallback\n", host);
	didresolve = 0;
	resolvedhost = ggz_resolvename(host, GGZ_RESOLVE_ASYNC);
	if(resolvedhost)
	{
		printf(">> did not resolve the name, left at %s\n", resolvedhost);
		ggz_free(resolvedhost);
	}
	else
	{
		do_something();
		printf(">> oh good! name resolved asynchronously!\n");
	}
	printf("\n");

	printf(">> connect to %s:80 synchronously\n", host);
	ret = ggz_make_socket(GGZ_SOCK_CLIENT, 80, host);
	printf(">> ggz_make_socket returned %i\n", ret);
	if (ret < 0) {
		printf(">> connection failed!!\n");
	} else {
		printf(">> connected!\n");
	}
	printf("\n");

	printf(">> connect to %s:80 asynchronously\n", host);
	didresolve = 0;
	ret = ggz_make_socket(GGZ_SOCK_CLIENT_ASYNC, 80, host);
	printf(">> ggz_make_socket returned %i\n", ret);
	if (ret == GGZ_SOCKET_PENDING) {
		do_something();
		printf(">> oh good! connected asynchronously!\n");
	} else if (ret < 0) {
		printf(">> connection failed!!\n");
	} else {
		printf(">> connected synchronously, oh bad!\n");
	}
	printf("\n");

	return 0;
}

