#ifndef SERVER_H
#define SERVER_H

#include <qserversocket.h>

class Server : public QServerSocket
{
	Q_OBJECT
	public:
		Server();
	private:
		void newConnection(int socket);
};

#endif

