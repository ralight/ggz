// Koenig - KDE client for the GGZ chess game
// Copyright (C) 2001 Tobias König, tokoe82@yahoo.de
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

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

