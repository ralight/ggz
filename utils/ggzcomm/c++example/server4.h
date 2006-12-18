#ifndef SERVER_H
#define SERVER_H

#include "connectx_server.h"

class QSocketNotifier;
class QTcpServer;

class Server : public QObject
{
	Q_OBJECT
	public:
		Server();
	private slots:
		void slotCommNotification(connectxOpcodes::Opcode messagetype, const msg& message);
		void slotCommError();
		void slotConnection();
		void slotNetwork();
	private:
		connectx *m_cx;
		QTcpServer *m_s;
};

#endif

