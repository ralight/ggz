#include "server.h"

#include <qapplication.h>
#include <qsocketnotifier.h>

Server::Server()
: QServerSocket(10000, 128, this)
{
	m_notifier = 0;
	m_cx = 0;
}

void Server::newConnection(int socket)
{
	qDebug("new-connection-at %i", socket);

	if(m_notifier) return;
	m_notifier = new QSocketNotifier(socket, QSocketNotifier::Read, this);
	connect(m_notifier, SIGNAL(activated(int)), SLOT(slotNetwork(int)));

	m_cx = new connectx();

	connect(m_cx,
		SIGNAL(signalNotification(connectxOpcodes::Opcode, const msg&)),
		SLOT(slotCommNotification(connectxOpcodes::Opcode, const msg&)));
	connect(m_cx, SIGNAL(signalError()), SLOT(slotCommError()));

	m_cx->ggzcomm_set_fd(socket);

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

void Server::slotNetwork(int fd)
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

	qApp->quit();
}

int main(int argc, char **argv)
{
	Server *s = new Server();

	QApplication a(argc, argv, false);
	return a.exec();
}

