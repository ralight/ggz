#include "kdots_proto.h"

#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdio.h>

KDotsProto::KDotsProto()
{
	state = stateinit;
}

KDotsProto::~KDotsProto()
{
}

void KDotsProto::getSeat()
{
	es_read_int(fd, &num);
}

void KDotsProto::getPlayers()
{
	int seattmp;

	for(int i = 0; i < 2; i++)
	{
		es_read_int(fd, &seattmp);
		if(seattmp != -1) es_read_string(fd, (char*)&players[i], 17);
	}
}

void KDotsProto::connect()
{
	char fd_name[64];
	struct sockaddr_un addr;
 
printf("connect\n");
	sprintf(fd_name, "/tmp/Dots.%d", getpid());
	if((fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0) exit(-1);
	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, fd_name);
	if(::connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) exit(-1);
printf("done\n");
}

void KDotsProto::sendMove(int x, int y, int direction)
{
	es_write_int(fd, direction);
	es_write_int(fd, x);
	es_write_int(fd, y);

	m_lastx = x;
	m_lasty = y;
	m_lastdir = direction;
printf("--debug-- assignment: %i/%i/%i\n", m_lastx, m_lasty, m_lastdir);
}

void KDotsProto::sendOptions(int wantwidth, int wantheight)
{
	width = wantwidth;
	height = wantheight;
	es_write_int(fd, sndoptions);
	es_write_char(fd, width);
	es_write_char(fd, height);
}

void KDotsProto::getOptions()
{
	es_read_char(fd, &width);
	es_read_char(fd, &height);
}

void KDotsProto::getMove()
{
	char status;
	char s, x, y;

	/*movex = -1;
	movey = -1;*/

	es_read_char(fd, &status);
	es_read_char(fd, &s);

	for(int i = 0; i < s; i++)
	{
		es_read_char(fd, &x);
		es_read_char(fd, &y);
	}

	/*if(status >= 0)
	{
		movex = x;
		movey = y;
	}*/
}

void KDotsProto::getOppMove(int direction)
{
	char s, x, y, nx, ny;

	if(state == stateopponent) state = statewait;

	es_read_char(fd, &nx);
	es_read_char(fd, &ny);
	es_read_char(fd, &s);

	for(int i = 0; i < s; i++)
	{
		es_read_char(fd, &x);
		es_read_char(fd, &y);
	}

	movex = nx;
	movey = ny;
}

