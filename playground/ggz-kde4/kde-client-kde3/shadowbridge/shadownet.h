// ShadowBridge - Game developer tool to visualize network protocols
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

#ifndef SHADOW_NET_H
#define SHADOW_NET_H

// Qt includes
#include <qobject.h>
#include <qptrlist.h>

// Forward declarations
class QDataStream;

// Networking and child handling class
class ShadowNet : public QObject
{
	Q_OBJECT
	public:
		ShadowNet();
		~ShadowNet();
		void addClient(const QString& exec, const QString& fdin, const QString& fdout);
		void start();
		void sync();

	public slots:
		void slotRead(int sock);
		void slotWrite(int sock);
		void slotActivated(int index);

	signals:
		void signalIncoming(const QString& data);
		void signalOutgoing(const QString& data);
		void signalError(const QString& error);

	private:
		QString convert(char byte);
		void simpleexec(const char *program);

		QPtrList<QDataStream> list;
		QDataStream *net;
		int m_activated;
		QString m_incoming, m_outgoing;
};

#endif

