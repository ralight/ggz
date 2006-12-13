#include "connectx_client.h"
#include "client.h"

#include <qapplication.h>
#include <qsocket.h>

Client::Client()
: QObject()
{
	m_s = new QSocket(this);
	m_s->connectToHost("localhost", 10000);

	connect(m_s, SIGNAL(connected()), SLOT(slotConnected()));
	connect(m_s, SIGNAL(error(int)), SLOT(slotError(int)));
}

void Client::slotConnected()
{
	qDebug("connection-at %i", m_s->socket());

	connectx *cx = new connectx();
	cx->ggzcomm_set_fd(m_s->socket());

	cx->ggzcomm_network_main();
}

void Client::slotError(int code)
{
	qDebug("error!");

	qApp->quit();
}

int main(int argc, char **argv)
{
	Client *c = new Client();

	QApplication a(argc, argv, false);
	return a.exec();

	return 0;
}

