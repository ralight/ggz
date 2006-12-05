//////////////////////////////////////////////////////////////////////
// KConnectX
// Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

#ifndef KCONNECTX_PROTO_H
#define KCONNECTX_PROTO_H

// Qt includes
#include <qobject.h>

// GGZ-KDE-Games includes
#include <kggzmod/module.h>

class Q3SocketDevice;
class QDataStream;

// Generic KConnectX client protocol handler
class Proto : public QObject
{
	Q_OBJECT
	public:
		// Constructor
		Proto();
		// Destructor
		~Proto();

		enum ServerOpcodes
		{
			msgseat = 0,
			msgplayers = 1,
			msgmove = 2,
			msggameover = 3,
			reqmove = 4,
			rspmove = 5,
			sndsync = 6,
			msgoptions = 7,
			reqoptions = 8,
			msgchat = 9,
			rspchat = 10
		};

		enum ClientOpcodes
		{
			sndmove = 0,
			reqsync = 1,
			sndoptions = 2,
			reqnewgame = 3
		};

		enum ServerErrors
		{
			allok = 0,
			errstate = -1,
			errturn = -2,
			errbound = -3,
			errfull = -4
		};

		KGGZMod::Module *mod() const;

		void sendMove(int move);
		void sendOptions(char boardwidth, char boardheight, char connectlength);
		void sendRestart();

	signals:
		void signalOptionsRequest(char minboardwidth, char maxboardwidth,
			char minboardheight, char maxboardheight,
			char minconnectlength, char maxconnectlength);
		void signalOptions(char boardwidth, char boardheight, char connectlength);
		void signalMoveRequest();
		void signalMoveResponse(char movestatus, int move);
		void signalMove(int move);
		void signalOver(char winner);

	private slots:
		void slotEvent(KGGZMod::Event event);
		void slotError();
		void slotNetwork(int fd);

	private:
		KGGZMod::Module *m_kggzmod;
		Q3SocketDevice *m_dev;
		QDataStream *m_net;

		struct ConnectXData
		{
			Q_INT32 myseat;
			Q_INT32 type1;
			Q_INT32 type2;
			QString name1;
			QString name2;
			Q_INT32 move;
			Q_INT8 winner;
			Q_INT8 movestatus;
			Q_INT8 boardwidth;
			Q_INT8 boardheight;
			Q_INT8 connectlength;
			Q_INT8 minboardwidth;
			Q_INT8 maxboardwidth;
			Q_INT8 minboardheight;
			Q_INT8 maxboardheight;
			Q_INT8 minconnectlength;
			Q_INT8 maxconnectlength;
		};

		ConnectXData m_data;
};

#endif

