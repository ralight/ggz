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

class QAbstractSocket;
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
		void slotEvent(const KGGZMod::Event& event);
		void slotError();
		void slotNetwork(int fd);

	private:
		KGGZMod::Module *m_kggzmod;
		QAbstractSocket *m_dev;
		QDataStream *m_net;

		struct ConnectXData
		{
			qint32 myseat;
			qint32 type1;
			qint32 type2;
			QString name1;
			QString name2;
			qint32 move;
			qint8 winner;
			qint8 movestatus;
			qint8 boardwidth;
			qint8 boardheight;
			qint8 connectlength;
			qint8 minboardwidth;
			qint8 maxboardwidth;
			qint8 minboardheight;
			qint8 maxboardheight;
			qint8 minconnectlength;
			qint8 maxconnectlength;
		};

		ConnectXData m_data;
};

#endif

