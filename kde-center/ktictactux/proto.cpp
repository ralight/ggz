/* Generated by GGZComm/ruby version 0.4 */
/* Protocol 'tictactoe', version '5' */
/* Do NOT edit this file! */

// FIXME: call packet->flush() after sending!

#include "proto.h"
#include <kggznet/kggzpacket.h>
#include <qdatastream.h>


tictactoe::tictactoe()
: QObject()
{
	requirelink = 0;
	fd = -1;
	packet = NULL;
}

void tictactoe::ggzcomm_sndmove(const sndmove& message)
{
	*packet->outputstream() << tictactoeOpcodes::message_sndmove;
	*packet->outputstream() << message.move_c;
}

void tictactoe::ggzcomm_reqsync(const reqsync& message)
{
	*packet->outputstream() << tictactoeOpcodes::message_reqsync;
}

void tictactoe::ggzcomm_reqmove()
{
	reqmove message;
	emit signalNotification(tictactoeOpcodes::message_reqmove, message);
}

void tictactoe::ggzcomm_rspmove()
{
	rspmove message;
	*packet->inputstream() >> message.status;
	emit signalNotification(tictactoeOpcodes::message_rspmove, message);
}

void tictactoe::ggzcomm_msgmove()
{
	msgmove message;
	*packet->inputstream() >> message.player;
	*packet->inputstream() >> message.move;
	emit signalNotification(tictactoeOpcodes::message_msgmove, message);
}

void tictactoe::ggzcomm_sndsync()
{
	sndsync message;
	*packet->inputstream() >> message.turn;
	for(int i1 = 0; i1 < 9; i1++)
	{
		*packet->inputstream() >> message.space[i1];
	}
	emit signalNotification(tictactoeOpcodes::message_sndsync, message);
}

void tictactoe::ggzcomm_msggameover()
{
	msggameover message;
	*packet->inputstream() >> message.winner;
	emit signalNotification(tictactoeOpcodes::message_msggameover, message);
}

void tictactoe::ggzcomm_network_main()
{
	packet->slotNetwork(fd);
}

void tictactoe::ggzcomm_ready()
{
	int opcode;

	*packet->outputstream() >> opcode;

	if(requirelink)
	{
		if(opcode != nextlink) emit signalError();
		requirelink = 0;
	}

	switch(opcode)
	{
		case tictactoeOpcodes::message_reqmove:
			ggzcomm_reqmove();
			break;
		case tictactoeOpcodes::message_rspmove:
			ggzcomm_rspmove();
			break;
		case tictactoeOpcodes::message_msgmove:
			ggzcomm_msgmove();
			break;
		case tictactoeOpcodes::message_sndsync:
			ggzcomm_sndsync();
			break;
		case tictactoeOpcodes::message_msggameover:
			ggzcomm_msggameover();
			break;
	}
}

void tictactoe::ggzcomm_set_fd(int usefd)
{
	fd = usefd;
	packet = new KGGZPacket();
}

void tictactoe::ggzcomm_set_socket(KGGZPacket *usepacket)
{
	packet = usepacket;
	connect(packet, SIGNAL(signalError()), SIGNAL(signalError()));
	connect(packet, SIGNAL(signalPacket()), SLOT(ggzcomm_ready()));
}

