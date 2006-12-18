#ifndef CLIENT_H
#define CLIENT_H

#include "connectx_client.h"

#include <qobject.h>

class QTcpSocket;

class Client : public QObject
{
	Q_OBJECT
	public:
		Client();
	private slots:
		void slotConnected();
		void slotError(int);
		void slotNetwork();
		void slotCommNotification(connectxOpcodes::Opcode messagetype, const msg& message);
		void slotCommError();
	private:
		QTcpSocket *m_s;
		connectx *m_cx;
};

#endif

