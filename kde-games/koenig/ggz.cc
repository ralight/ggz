// Koenig - KDE client for the GGZ chess game
// Copyright (C) 2001 Tobias König, tokoe82@yahoo.de
// Copyright (C) 2001 - 2004 Josef Spillner, josef@ggzgamingzone.org
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

#include <qsocketdevice.h>
#include <qsocketnotifier.h>
#include <qstring.h>
#include <qapplication.h>

#include <kdebug.h>

#include "ggz.h"
#include "ggz.moc"

GGZ *GGZ::self = NULL;

GGZ::GGZ(void)
{
	self = this;
	mod = NULL;
	socket = NULL;
}

GGZ::~GGZ(void)
{
	if(socket)
		delete socket;

	if (mod)
	{
		ggzmod_disconnect(mod);
		ggzmod_free(mod);
	}

	socket = NULL;
}

void GGZ::connect(const QString& name)
{
	int fd, ret;
	QSocketNotifier *sn;

	mod = ggzmod_new(GGZMOD_GAME);
	ggzmod_set_handler(mod, GGZMOD_EVENT_SERVER, &recvEvent);
	ret = ggzmod_connect(mod);
	fd = ggzmod_get_fd(mod);

	sn = new QSocketNotifier(fd, QSocketNotifier::Read, this);
	QObject::connect(sn, SIGNAL(activated(int)), SLOT(recvControl()));
}

void GGZ::recvControl()
{
	ggzmod_dispatch(mod);
}

void GGZ::recvRawData()
{
	emit recvData();
}

void GGZ::recvEvent(GGZMod *mod, GGZModEvent e, void *data)
{
	QSocketNotifier *sn;
	self->socket = new QSocketDevice(*(int*)data, QSocketDevice::Stream);

	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);

	self->net = new QDataStream(self->socket);
	self->net->setVersion(5);

	sn = new QSocketNotifier(*(int*)data, QSocketNotifier::Read, self);
	QObject::connect(sn, SIGNAL(activated(int)), self, SLOT(recvRawData()));
}

void GGZ::fetch(int bytes)
{
	int cycles = 0;

	while(socket->bytesAvailable() < bytes)
	{
		socket->waitForMore(100);
		//qApp->processEvents();
		cycles++;
	}
	kdDebug() << ":: waitformore: " << cycles << " bytes: " << socket->bytesAvailable() << endl;
}

char *GGZ::getString(void)
{
	char *s;

	fetch(5);
	*net >> s;
	//kdDebug() << "--- " << s << endl;
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
	
	for(unsigned int i = 0; i < msg.length(); i++)
		socket->putch(msg[i]);
}

char GGZ::getChar(void)
{
	char value;
	Q_INT8 v;

	fetch(sizeof(char));
	*net >> v;
	value = v;
	//kdDebug() << "--- " << v << endl;
	return value;
}

void GGZ::putChar(char value)
{
	*net << (Q_INT8)value;
}

int GGZ::getInt(void)
{
	int value;

	fetch(sizeof(int));
	*net >> value;
	//kdDebug() << "--- " << value << endl;
	return value;
}

void GGZ::putInt(int value)
{
	*net << (Q_INT32)value;
}

