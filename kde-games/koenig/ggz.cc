#include <qsocket.h>
#include <qstring.h>

#include "ggz.h"
#include "ggz.moc"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <kdebug.h>
#include <iostream>

GGZ::GGZ(void)
{
	socket = new QSocket;
}

GGZ::~GGZ(void)
{
	if (socket)
		delete socket;
	
	socket = 0;
}

void GGZ::connect(QString& name)
{
	int fd;
//	struct sockaddr_un addr;
			 
	name.sprintf("/tmp/%s.%d", name.latin1(), getpid());
		 
//	if ((fd = ::socket(PF_LOCAL, SOCK_STREAM, 0)) < 0) {
//		kdDebug << "can't create domain socket" << endl;
//		exit(-1);
//	}
	
//	bzero(&addr, sizeof(addr));
//	addr.sun_family = AF_LOCAL;
//	strcpy(addr.sun_path, name.latin1());
					 
//	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
//		kdDebug << "can't connect to domain socket" << endl;
//		exit(-2);
//	}

	socket->setSocket(fd);
}    

QString GGZ::getString(void)
{
	QString msg;
	int size;

	socket->readBlock((char*)size, 4);
	
	for (int i = 0; i < size; i++)
		msg.append(socket->getch());

	return msg;
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

