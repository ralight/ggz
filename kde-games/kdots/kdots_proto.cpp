#include "kdots_proto.h"

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

