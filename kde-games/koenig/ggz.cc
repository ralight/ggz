#include <qsocket.h>
#include <qstring.h>

#include "ggz.h"
#include "ggz.moc"

#include <stdlib.h>

GGZ::GGZ(void)
{
	socket = new QSocket();
}

GGZ::~GGZ(void)
{
	if (socket)
		delete socket;
	
	socket = NULL;
}

void GGZ::connect(const QString& name)
{
	int fd;

	fd = 3; // ggz_connect();

	socket->setSocket(fd);
	QObject::connect(socket, SIGNAL(readyRead()), SLOT(recvRawData()));
	net = new QDataStream(socket);
}

void GGZ::recvRawData()
{
	while(socket->size())
	{
		emit recvData();
	}
}

char *GGZ::getString(void)
{
	char *s;

	*net >> s;
	return s;
}

QString GGZ::getString(int maxsize)
{
	// quick!
	return getString();
}

void GGZ::putString(const QString& msg)
{
	socket->writeBlock((const char*)(msg.length()), 4);
	
	for (int i = 0; i < msg.length(); i++)
		socket->putch(msg[i]);
}

