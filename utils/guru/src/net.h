#ifndef NET_H
#define NET_H

#include "gurumod.h"

enum States
{
	NET_NOOP,
	NET_LOGIN,
	NET_ERROR,
	NET_GOTREADY,
	NET_INPUT
};

void net_connect(const char *host, int port);
void net_join(int room);
int net_status();
Guru *net_input();
void net_output(Guru *output);

#endif

