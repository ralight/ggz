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

#ifndef CHESS_GGZ_H
#define CHESS_GGZ_H

#include <ggzmod.h>

class QSocketDevice;
class QDataStream;

class GGZ : public QObject
{
	Q_OBJECT
	public:
		GGZ();
		~GGZ();
	
		void connect(const QString&);
		void disconnect();

		void fetch(int bytes);

		char getChar(void);
		void putChar(char value);

		int getInt(void);
		void putInt(int value);
	
		char *getString(void);
		QString getString(int maxlength);
		void putString(const QString&);
	
	public slots:
		void recvControl();
		void recvRawData();

	signals:
		void recvData();

	private:
		static void recvEvent(GGZMod *mod, GGZModEvent e,
				      const void *data);

		QSocketDevice *socket;
		QDataStream *net;
		GGZMod *mod;
		static GGZ *self;
};

#endif

