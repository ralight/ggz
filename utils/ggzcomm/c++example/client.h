#ifndef CLIENT_H
#define CLIENT_H

#include <qobject.h>

class QSocket;

class Client : public QObject
{
	Q_OBJECT
	public:
		Client();
	private slots:
		void slotConnected();
		void slotError(int);
	private:
		QSocket *m_s;
};

#endif

