#include "ktictactuxproto.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>

KTicTacTuxProto::KTicTacTuxProto()
{
}

KTicTacTuxProto::~KTicTacTuxProto()
{
}

void KTicTacTuxProto::connect()
{
	char fd_name[64];
	struct sockaddr_un addr;

printf("connect\n");
	sprintf(fd_name, "/tmp/TicTacToe.%d", getpid());
	if((fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0) exit(-1);
	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, fd_name);
	if(::connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) exit(-1);
printf("done\n");
}

void KTicTacTuxProto::init()
{
	for(int i = 0; i < 9; i++)
		board[i % 3][i / 3] = none;
	state = stateinit;
}

int KTicTacTuxProto::getSeat()
{
	es_read_int(fd, &num);
}

int KTicTacTuxProto::getPlayers()
{
	for(int i = 0; i < 2; i++)
	{
		es_read_int(fd, &seats[i]);
		if(seats[i] != -1) es_read_string(fd, (char*)&names[i], 17);
	}
}

int KTicTacTuxProto::getMoveStatus()
{
	char status;

	es_read_char(fd, &status);

	if(status == 0) board[move % 3][move / 3] = seats[num];
}

int KTicTacTuxProto::getOpponentMove()
{
	int move;

	es_read_int(fd, &move);
	board[move % 3][move / 3] = seats[(num + 1) % 2];
}

int KTicTacTuxProto::getSync()
{
	// TODO!!!
}

int KTicTacTuxProto::getGameOver()
{
	es_read_char(fd, &winner);
}

int KTicTacTuxProto::sendOptions()
{
	es_write_int(fd, 0);
}

int KTicTacTuxProto::sendMyMove()
{
	es_write_int(fd, sndmove);
	es_write_int(fd, move);
}
