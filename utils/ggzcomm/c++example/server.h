#ifndef SERVER_H
#define SERVER_H

#include "connectx_server.h"

#include <qserversocket.h>

class QSocketNotifier;

class Server : public QServerSocket
{
	Q_OBJECT
	public:
		Server();
	private slots:
		void slotNetwork(int fd);
		void slotCommNotification(connectxOpcodes::Opcode messagetype, const msg& message);
		void slotCommError();
	private:
		void newConnection(int socket);
		QSocketNotifier *m_notifier;
		connectx *m_cx;
};

#endif

