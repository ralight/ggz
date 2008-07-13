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

// ShadowBridge includes
#include "shadownet.h"

// KDE includes
#include <klocale.h>

// Qt includes
#include <qsocketnotifier.h>
#include <qdatastream.h>
#include <qsocketdevice.h>

// System includes
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

ShadowNet::ShadowNet()
{
	net = NULL;
	m_activated = 0;
}

ShadowNet::~ShadowNet()
{
	if(net) delete net;
}

void ShadowNet::simpleexec(const char *program)
{
	char *tok;
	char **l;
	int i;

	tok = strtok(strdup(program), " ");
	if(!tok) return;

	l = (char**)malloc(sizeof(char*) * 2);
	l[0] = (char*)malloc(sizeof(char*));
	l[1] = (char*)malloc(sizeof(char*));
	l[0] = tok;

	i = 1;
	while(tok)
	{
		tok = strtok(NULL, " ");
		l = (char**)realloc(l, i + 2);
		l[i + 1] = (char*)malloc(sizeof(char*));
		l[i] = tok;
		i++;
	}

	l[i] = NULL;
	execvp(l[0], l);
}

void ShadowNet::addClient(const QString& exec, const QString& fdin, const QString& fdout)
{
	int fd[2];
	QSocketNotifier *snr;
	int ret;
	QDataStream *net2;
	QSocketDevice *dev;

	ret = socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
	if(ret < 0)
	{
		emit signalError(i18n("Couldn't create socket pair."));
		return;
	}

	switch(fork())
	{
		case -1:
			emit signalError(i18n("Couldn't fork,"));
			return;
			break;
		case 0:
			dup2(fd[0], fdin.toInt());
			dup2(fd[0], fdout.toInt());
			simpleexec(exec.latin1());
			exit(-1);
			break;
		default:
			break;
	}

	snr = new QSocketNotifier(fd[1], QSocketNotifier::Read, this);
	connect(snr, SIGNAL(activated(int)), SLOT(slotRead(int)));

	dev = new QSocketDevice(fd[1], QSocketDevice::Stream);
	net2 = new QDataStream(dev);
	list.append(net2);
}

void ShadowNet::start()
{
	int fd;
	QSocketNotifier *snw;
	QSocketDevice *dev;

	fd = 3;

	snw = new QSocketNotifier(fd, QSocketNotifier::Read, this);
	connect(snw, SIGNAL(activated(int)), SLOT(slotWrite(int)));

	dev = new QSocketDevice(fd, QSocketDevice::Stream);
	net = new QDataStream(dev);
}

void ShadowNet::slotRead(int sock)
{
	QDataStream *s;
	Q_INT8 transmit = '\0';

	Q_UNUSED(sock);

	//cout << "game -> ggzcore: " << sock << endl;

	s = list.at(m_activated);
	if((!s) || (!net)) return;

	*s >> transmit;
	*net << transmit;

	if(!m_incoming.isEmpty())
	{
		emit signalIncoming(m_incoming);
		m_incoming = "";
	}
	m_outgoing.append(convert(transmit));
}

void ShadowNet::slotWrite(int sock)
{
	QDataStream *s;
	Q_INT8 transmit = '\0';

	Q_UNUSED(sock);

	//cout << "ggzcore -> game " << sock << endl;

	s = list.at(m_activated);
	if((!s) || (!net)) return;

	*net >> transmit;
	*s << transmit;

	if(!m_outgoing.isEmpty())
	{
		emit signalOutgoing(m_outgoing);
		m_outgoing = "";
	}
	m_incoming.append(convert(transmit));
}

void ShadowNet::slotActivated(int index)
{
	m_activated = index;
}

QString ShadowNet::convert(char byte)
{
	if((byte > 31) && (byte < 123)) return QString(QChar(byte));
	return QString("(0x%1%2)").arg((byte & 0xF0) >> 4).arg(byte & 0x0F);
}

void ShadowNet::sync()
{
	if(!m_outgoing.isEmpty())
	{
		emit signalOutgoing(m_outgoing);
		m_outgoing = "";
	}
	if(!m_incoming.isEmpty())
	{
		emit signalIncoming(m_incoming);
		m_incoming = "";
	}
}

