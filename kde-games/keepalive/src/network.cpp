// Keepalive - Experimental GGZ game
// Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net
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

// Header file
#include "network.h"

// Qt includes
#include <qsocketnotifier.h>

// Static variables
Network *Network::self = NULL;

Network::Network()
: QObject()
{
	self = this;
	m_fd = -1;
	m_controlfd = -1;
	mod = NULL;
}

Network::~Network()
{
	if(mod)
	{
		ggzmod_disconnect(mod);
		ggzmod_free(mod);
	}
}

void Network::doconnect()
{
	QSocketNotifier *sn;

	mod = ggzmod_new(GGZMOD_GAME);
	ggzmod_set_handler(mod, GGZMOD_EVENT_SERVER, &callbackwrap);
	ggzmod_connect(mod);
	m_controlfd = ggzmod_get_fd(mod);

	sn = new QSocketNotifier(m_controlfd, QSocketNotifier::Read, this);
	connect(sn, SIGNAL(activated(int)), SLOT(slotControl()));
}

int Network::fd()
{
	return m_fd;
}

void Network::slotControl()
{
	ggzmod_dispatch(mod);
}

void Network::callback(GGZMod *mod, const void *data)
{
	QSocketNotifier *sn;

	m_fd = *(const int*)data;
	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);

	sn = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
	connect(sn, SIGNAL(activated(int)), SIGNAL(signalData()));
}

void Network::callbackwrap(GGZMod *mod, GGZModEvent e, const void *data)
{
	self->callback(mod, data);
}

