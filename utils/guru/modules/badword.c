/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gurumod.h"
#include <ggzcore.h>

char **badwordlist;

void gurumod_init()
{
	char path[1024];
	int handle;
	int ret, i;
	int count;

	sprintf(path, "%s/.ggz/guru/modbadword.rc", getenv("HOME"));
	handle = ggzcore_confio_parse(path, GGZ_CONFIO_RDONLY);
	if(handle < 0) return;
	ret = ggzcore_confio_read_list(handle, "badwords", "badwords", &count, &badwordlist);

	printf("[ ");
	for(i = 0; i < count; i++)
		printf("|");
	printf(" ] ");
	if(ret < 0) badwordlist = NULL;
}

Guru *gurumod_exec(Guru *message)
{
	int i, j;
	
	j = 0;
	while((message->list) && (message->list[j]))
	{
		i = 0;
		while((badwordlist) && (badwordlist[i]))
		{
			if(!strcasecmp(message->list[j], badwordlist[i]))
			{
				message->message = "Watch your language please!";
				return message;
			}
			i++;
		}
		j++;
	}
	return NULL;
}


