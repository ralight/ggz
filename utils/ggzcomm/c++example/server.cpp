#include "connectx_server.h"
#include "server.h"

#include <qapplication.h>

Server::Server()
: QServerSocket(10000, 128, this)
{
}

void Server::newConnection(int socket)
{
	qDebug("new-connection-at %i", socket);

	connectx *cx = new connectx();
	cx->ggzcomm_set_fd(socket);

	reqoptions opt;
	opt.minboardwidth = 3;
	opt.maxboardwidth = 10;
	opt.minboardheight = 3;
	opt.maxboardheight = 10;
	opt.minconnectlength = 4;
	opt.maxconnectlength = 7;

	cx->ggzcomm_reqoptions(opt);
	cx->ggzcomm_network_main();
}

int main(int argc, char **argv)
{
	Server *s = new Server();

	QApplication a(argc, argv, false);
	return a.exec();

	return 0;
}

