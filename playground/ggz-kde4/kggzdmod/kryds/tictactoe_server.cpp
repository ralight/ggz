/* Generated by GGZComm/ruby version 0.4 */
/* Protocol 'tictactoe', version '5' */
/* Do NOT edit this file! */

#include "tictactoe_server.h"
#include <kggznet/kggzraw.h>


tictactoe::tictactoe()
: QObject()
{
	requirelink = 0;
	fd = -1;
	raw = NULL;
}

void tictactoe::ggzcomm_sndmove()
{
	sndmove message;
	*raw >> message.move_c;
	emit signalNotification(tictactoeOpcodes::message_sndmove, message);
}

void tictactoe::ggzcomm_reqsync()
{
	reqsync message;
	emit signalNotification(tictactoeOpcodes::message_reqsync, message);
}

void tictactoe::ggzcomm_reqmove(const reqmove& message)
{
	*raw << tictactoeOpcodes::message_reqmove;
}

void tictactoe::ggzcomm_rspmove(const rspmove& message)
{
	*raw << tictactoeOpcodes::message_rspmove;
	*raw << message.status;
}

void tictactoe::ggzcomm_msgmove(const msgmove& message)
{
	*raw << tictactoeOpcodes::message_msgmove;
	*raw << message.player;
	*raw << message.move;
}

void tictactoe::ggzcomm_sndsync(const sndsync& message)
{
	*raw << tictactoeOpcodes::message_sndsync;
	*raw << message.turn;
	for(int i1 = 0; i1 < 9; i1++)
	{
		*raw << message.space[i1];
	}
}

void tictactoe::ggzcomm_msggameover(const msggameover& message)
{
	*raw << tictactoeOpcodes::message_msggameover;
	*raw << message.winner;
}

void tictactoe::ggzcomm_network_main()
{
	int opcode;

	*raw >> opcode;

	if(requirelink)
	{
		if(opcode != nextlink) emit signalError();
		requirelink = 0;
	}

	switch(opcode)
	{
		case tictactoeOpcodes::message_sndmove:
			ggzcomm_sndmove();
			break;
		case tictactoeOpcodes::message_reqsync:
			ggzcomm_reqsync();
			break;
	}
}

void tictactoe::ggzcomm_set_fd(int usefd)
{
	fd = usefd;
	raw = new KGGZRaw();
	raw->setNetwork(fd);
}

void tictactoe::ggzcomm_set_socket(KGGZRaw *useraw)
{
	raw = useraw;
}

