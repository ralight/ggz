/* Generated by GGZComm/ruby version 0.4 */
/* Protocol 'tictactoe', version '5' */
/* Do NOT edit this file! */

#ifndef GGZCOMM_TICTACTOE_H
#define GGZCOMM_TICTACTOE_H

#include <qobject.h>

class KGGZRaw;

class tictactoeOpcodes
{
	public:
	enum Opcode
	{
		message_sndmove = 0,
		message_reqsync = 1,
		message_reqmove = 4,
		message_rspmove = 5,
		message_msgmove = 2,
		message_sndsync = 6,
		message_msggameover = 3
	};
};

class msg
{
	public:
	msg(tictactoeOpcodes::Opcode type){m_type = type;}
	tictactoeOpcodes::Opcode type() const{return m_type;}
	private:
	tictactoeOpcodes::Opcode m_type;
};

class sndmove : public msg
{
	public:
	sndmove() : msg(tictactoeOpcodes::message_sndmove){}
	int move_c;
};

class reqsync : public msg
{
	public:
	reqsync() : msg(tictactoeOpcodes::message_reqsync){}

};

class reqmove : public msg
{
	public:
	reqmove() : msg(tictactoeOpcodes::message_reqmove){}

};

class rspmove : public msg
{
	public:
	rspmove() : msg(tictactoeOpcodes::message_rspmove){}
	qint8 status;
};

class msgmove : public msg
{
	public:
	msgmove() : msg(tictactoeOpcodes::message_msgmove){}
	int player;
	int move;
};

class sndsync : public msg
{
	public:
	sndsync() : msg(tictactoeOpcodes::message_sndsync){}
	qint8 turn;
	qint8 space[9];
	int i1;
};

class msggameover : public msg
{
	public:
	msggameover() : msg(tictactoeOpcodes::message_msggameover){}
	qint8 winner;
};

class tictactoe : public QObject
{
	Q_OBJECT
	public:
	tictactoe();

	void ggzcomm_network_main();

	void ggzcomm_reqmove(const reqmove& message);
	void ggzcomm_rspmove(const rspmove& message);
	void ggzcomm_msgmove(const msgmove& message);
	void ggzcomm_sndsync(const sndsync& message);
	void ggzcomm_msggameover(const msggameover& message);

	void ggzcomm_set_fd(int usefd);
	void ggzcomm_set_socket(KGGZRaw *useraw);

	signals:
	void signalNotification(tictactoeOpcodes::Opcode messagetype, const msg& message);
	void signalError();

	private:
	void handle(bool ret);
	void ggzcomm_sndmove();
	void ggzcomm_reqsync();

	int fd;
	int ret;
	int requirelink;
	int nextlink;
	KGGZRaw *raw;
};

#endif

