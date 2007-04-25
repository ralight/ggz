#ifdef __linux__
#include <linux/version.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)
#define HAVE_EPOLL 1
#endif
#endif

#include <stdio.h>
#include <unistd.h>

#ifdef HAVE_EPOLL
#include <sys/epoll.h>
#define MAX_EVENTS 10
#endif

int main(int argc, char *argv[])
{
#ifdef HAVE_EPOLL
	int epoll_fd, ret;
	struct epoll_event events[MAX_EVENTS];
	struct epoll_event event;
	int i;

	epoll_fd = epoll_create(1024);
	if(epoll_fd == -1)
	{
		fprintf(stderr, "epoll_create *error*\n");
		return -1;
	}

	event.events = EPOLLIN;

	ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
	if(ret == -1)
	{
		fprintf(stderr, "epoll_ctl *error*\n");
		return -1;
	}

	while(1)
	{
		ret = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if(ret == -1)
		{
			fprintf(stderr, "epoll_wait *error*\n");
			return -1;
		}

		printf("File descriptors activated: %i\n", ret);
		for(i = 0; i < MAX_EVENTS; i++)
		{
			if(events[i].events & EPOLLIN)
			{
				printf("- %i\n", events[i].data.fd);
			}
		}
	}

	close(epoll_fd);
#endif

	return 0;
}

