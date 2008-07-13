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

#ifndef KEEPALIVE_NETWORK_H
#define KEEPALIVE_NETWORK_H

// GGZ includes
#include <ggzmod.h>

// Qt includes
#include <qobject.h>
#include <qsocket.h>

// Network resource class
class Network : public QObject
{
	Q_OBJECT
	public:
		Network();
		~Network();
		int fd();
		void doconnect();
		QSocket *socket();

	signals:
		void signalData();

	public slots:
		void slotControl();

	private:
		static Network *self;
		void callback(GGZMod *mod, const void *data);
		static void callbackwrap(GGZMod *mod, GGZModEvent e,
					 const void *data);
		GGZMod *mod;
		int m_fd, m_controlfd;
		QSocket *m_sock;
};

#endif

