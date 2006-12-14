#include "client.h"

#include <qapplication.h>
#include <qsocket.h>

Client::Client()
: QObject()
{
	m_cx = 0;

	m_s = new QSocket(this);
	m_s->connectToHost("localhost", 10000);

	connect(m_s, SIGNAL(connected()), SLOT(slotConnected()));
	connect(m_s, SIGNAL(error(int)), SLOT(slotError(int)));
	connect(m_s, SIGNAL(readyRead()), SLOT(slotNetwork()));
}

void Client::slotConnected()
{
	qDebug("connection-at %i", m_s->socket());

	m_cx = new connectx();

	connect(m_cx,
		SIGNAL(signalNotification(connectxOpcodes::Opcode, const msg&)),
		SLOT(slotCommNotification(connectxOpcodes::Opcode, const msg&)));
	connect(m_cx, SIGNAL(signalError()), SLOT(slotCommError()));

	m_cx->ggzcomm_set_socket(m_s);
}

void Client::slotNetwork()
{
	qDebug("data arrived!");

	m_cx->ggzcomm_network_main();
}

void Client::slotError(int code)
{
	qDebug("error!");

	qApp->quit();
}

void Client::slotCommNotification(connectxOpcodes::Opcode messagetype, const msg& message)
{
	qDebug("comm notification!");
}

void Client::slotCommError()
{
	qDebug("comm error");

	qApp->quit();
}

int main(int argc, char **argv)
{
	Client *c = new Client();

	QApplication a(argc, argv, false);
	return a.exec();
}

