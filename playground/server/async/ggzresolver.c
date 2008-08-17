#include <ggz.h>
#include <stdio.h>
#include <unistd.h>

#define USE_ASYNCNS 1

static int didresolve = 0;

static void callback(const char *address, int socket)
{
	printf("- [resolver: %s][socket: %i]\n",
		address, socket);
	didresolve = 1;
}

void do_something()
{
	while (!didresolve) {
		printf("- how long will it take?\n");
		usleep(1000);

#if USE_ASYNCNS
		int fd, ret;
		fd_set rfd;

		fd = ggz_resolver_fd();
		FD_ZERO(&rfd);
		FD_SET(fd, &rfd);
		ret = select(fd + 1, &rfd, NULL, NULL, 0);
		if (ret > 0) {
			printf("- asyncns query resolved something\n");
			ggz_resolver_work();
		}
#endif
	}
}

int main(int argc, char *argv[])
{
	int ret;
	char *host;

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
	didresolve = 0;

	printf(">> resolve %s\n");
	ggz_resolvename(host);
	do_something();
	printf(">> oh good! name resolved!\n");

	didresolve = 0;

	printf(">> connect to %s:80\n", host);
	ret = ggz_make_socket(GGZ_SOCK_CLIENT, 80, host);
	printf(">> ggz_make_socket returned %i\n", ret);
	if (ret == -3) {
		do_something();
		printf(">> oh good! connected!\n");
	} else {
		printf(">> already connected!\n");
	}
	return 0;
}

