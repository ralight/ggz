#define SERVER_MODE 0

#ifdef __linux__
#include <linux/version.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)
#define HAVE_EPOLL 1
#endif
#endif

#include <stdio.h>
#include <unistd.h>

#ifdef SERVER_MODE
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#endif

#include <sys/ioctl.h>
#include <stdlib.h>

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
	ssize_t bytes;
	char *peekbuf;
	int testsock;
	int pending;
#if SERVER_MODE
	int serversock;
	struct sockaddr_in name;
	unsigned int namesize;
	int port;
	const int dumbopt;
#endif

	epoll_fd = epoll_create(1024);
	if(epoll_fd == -1)
	{
		fprintf(stderr, "epoll_create *error*\n");
		return -1;
	}

#if SERVER_MODE
	serversock = socket(AF_INET, SOCK_STREAM, 0);
	ret = setsockopt(serversock, SOL_SOCKET, SO_REUSEADDR, (void*)&dumbopt, sizeof(dumbopt));
	printf("Serversock: %i [option returns %i]\n", serversock, ret);

	port = 11111;
	memset(&name, 0, sizeof(name));
	name.sin_family = AF_INET;
	name.sin_port = htons(port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(serversock, (struct sockaddr*)&name, sizeof(name));
	if(ret)
	{
		fprintf(stderr, "bind *error*\n");
		fprintf(stderr, "=> error %i: %s\n", errno, strerror(errno));
		return -1;
	}

	listen(serversock, 1);

	namesize = sizeof(name);
	testsock = accept(serversock, (struct sockaddr*)&name, &namesize);
#else
	testsock = STDIN_FILENO;
#endif
	printf("Testsock: %i\n", testsock);

	event.events = EPOLLIN;
	event.data.fd = testsock;

	memset(events, 0, sizeof(struct epoll_event) * MAX_EVENTS);

	ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, testsock, &event);
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
				printf("- fd %i @ %i\n", events[i].data.fd, i);

				printf("Peek size...\n");
				ioctl(events[i].data.fd, FIONREAD, &pending);
				printf("=> size: %i\n", pending);

				if(pending == 0)
				{
					printf("Seems to have disconnected!\n");
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
					continue;
				}

				printf("Read all the data...\n");
				do
				{
					peekbuf = malloc(pending);
					bytes = read(events[i].data.fd, peekbuf, pending);
					printf("=> %i out of max %i bytes\n", bytes, pending);
					free(peekbuf);
					pending -= bytes;
					if(bytes == -1)
					{
						fprintf(stderr, "recv() *error*\n");
						return -1;
					}
				} while (pending > 0);
			}
		}
	}

	close(epoll_fd);
#endif

	return 0;
}

