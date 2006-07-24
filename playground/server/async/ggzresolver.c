#include <ggz.h>
#include <stdio.h>
#include <unistd.h>

static int didresolve = 0;

static void callback(const char *address, int socket)
{
	printf("[resolver: %s][socket: %i]\n",
		address, socket);
	didresolve = 1;
}

void do_something()
{
	while(!didresolve) {
		printf("how long will it take?\n");
		usleep(1000);
	}
}

int main(int argc, char *argv[])
{
	int ret;

	ggz_set_network_notify_func(callback);
	didresolve = 0;

	ggz_resolvename("localhost");
	do_something();
	printf("oh good! name resolved!\n");

	didresolve = 0;

	ret = ggz_make_socket(GGZ_SOCK_CLIENT, 80, "localhost");
	printf("ggz_make_socket returned %i\n", ret);
	if(ret == -3) {
		do_something();
		printf("oh good! connected!\n");
	} else {
		printf("already connected!\n");
	}
	return 0;
}

