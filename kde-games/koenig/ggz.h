// Koenig - KDE client for the GGZ chess game
// Copyright (C) 2001 Tobias K�nig, tokoe82@yahoo.de
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

#ifndef CHESS_GGZ_H
#define CHESS_GGZ_H

#include <qsocket.h>
#include <qdatastream.h>

class GGZ : public QObject
{
	Q_OBJECT
	public:
		GGZ();
		~GGZ();
	
		void connect(const QString&);
		void disconnect();

		char getChar(void) { char value; Q_INT8 v; /*socket->readBlock(&value, sizeof(char));*/ *net >> v; value = v; return value; }
		void putChar(char value) { /*socket->writeBlock(&value, sizeof(char));*/ *net << (Q_INT8)value; }

		int getInt(void) { int value; /*socket->readBlock(&((char)value), sizeof(int));*/ *net >> value; return value; }
		void putInt(int value) { /*socket->writeBlock(&((char)value), sizeof(int));*/ *net << (Q_INT32)value;  }
	
		char *getString(void);
		QString getString(int maxlength);
		void putString(const QString&);
	
		// int readSocketDesc(void);

	public slots:
		void recvRawData();

	signals:
		void recvData();

	private:
		QSocket *socket;
		QDataStream *net;
};

#endif

