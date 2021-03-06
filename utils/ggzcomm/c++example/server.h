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
		void slotCommNotification(connectxOpcodes::Opcode messagetype, const msg& message);
		void slotCommError();
		void slotNetwork(int fd);
	private:
		void newConnection(int socket);
		connectx *m_cx;
		QSocketNotifier *m_notifier;
};

#endif

