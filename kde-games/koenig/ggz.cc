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
#include <qsocketnotifier.h>
#include <qstring.h>

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
	while(socket->size())
	{
		emit recvData();
	}
}

void GGZ::recvEvent(GGZMod *mod, GGZModEvent e, void *data)
{
	self->socket = new QSocket();
	self->socket->setSocket(*(int*)data);

	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);

	QObject::connect(self->socket, SIGNAL(readyRead()), self, SLOT(recvRawData()));
	self->net = new QDataStream(self->socket);
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
	
	for(unsigned int i = 0; i < msg.length(); i++)
		socket->putch(msg[i]);
}

