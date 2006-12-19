/*******************************************************************
*
* Agrue - A GGZ Real User Emulator
* Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
* Idea based on the original Agrue, code based on Grubby
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#ifndef NET_H
#define NET_H

struct agrue_t
{
	void *server;
	const char *name;
	const char *password;
	int frequency;
	int mobility;
	int activity;
};
typedef struct agrue_t Agrue;

void net_connect(const char *host, int port, Agrue *agrue);
void net_work(Agrue *agrue);

#endif

