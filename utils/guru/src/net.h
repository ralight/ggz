#ifndef NET_H
#define NET_H

enum States
{
	NET_NOOP,
	NET_LOGIN,
	NET_ERROR,
	NET_GOTREADY
};

void net_connect(const char *host, int port);
void net_join(int room);
int net_status();

#endif

