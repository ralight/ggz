#include "server4.h"

#include <qcoreapplication.h>
#include <qtcpserver.h>
#include <qtcpsocket.h>

Server::Server()
: QObject()
{
	m_cx = 0;

	m_s = new QTcpServer();
	m_s->listen(QHostAddress::Any, 10000);
	connect(m_s, SIGNAL(newConnection()), SLOT(slotConnection()));
}

void Server::slotConnection()
{
	QTcpSocket *conn = m_s->nextPendingConnection();

	qDebug("new-connection-at %i", conn->socketDescriptor());

	//connect(conn, SIGNAL(error(int)), SLOT(slotError(int)));
	connect(conn, SIGNAL(readyRead()), SLOT(slotNetwork()));

	m_cx = new connectx();

	connect(m_cx,
		SIGNAL(signalNotification(connectxOpcodes::Opcode, const msg&)),
		SLOT(slotCommNotification(connectxOpcodes::Opcode, const msg&)));
	connect(m_cx, SIGNAL(signalError()), SLOT(slotCommError()));

	m_cx->ggzcomm_set_socket(conn);

	reqoptions opt;
	opt.minboardwidth = 3;
	opt.maxboardwidth = 10;
	opt.minboardheight = 3;
	opt.maxboardheight = 10;
	opt.minconnectlength = 4;
	opt.maxconnectlength = 7;

	m_cx->ggzcomm_reqoptions(opt);
	qDebug("sent reqoptions! now sleep");
}

void Server::slotNetwork()
{
	qDebug("wake up!");
	m_cx->ggzcomm_network_main();
}

void Server::slotCommNotification(connectxOpcodes::Opcode messagetype, const msg& message)
{
	qDebug("comm notification!");
}

void Server::slotCommError()
{
	qDebug("comm error");

	QCoreApplication::instance()->quit();
}

int main(int argc, char **argv)
{
	Server *s = new Server();

	QCoreApplication a(argc, argv);
	return a.exec();
}

