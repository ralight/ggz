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

#ifndef KEEPALIVE_CANVAS_H
#define KEEPALIVE_CANVAS_H

// Qt includes
#include <qcanvas.h>

// Class declarations
class QDataStream;
class QSocketDevice;
class QCanvasSprite;
class Network;

// The main game canvas
class Canvas : public QCanvas
{
	Q_OBJECT
	public:
		Canvas(QWidget *parent = NULL, const char *name = NULL);
		~Canvas();
		void init();
		void load();
		void move(int x, int y);
		void moveTo(int x, int y);
		void login(QString username, QString password);
		void chat(QString message);

	signals:
		void signalLoggedin(QString name);
		void signalUnit(QCanvasPixmapArray *a);

	public slots:
		void slotInput();

	//protected slots:
	//	void keyPressEvent(QKeyEvent *e);
	protected:
		void timerEvent(QTimerEvent *e);

	private:
		void moves();
		void domove(int x, int y);

		QCanvasSprite *m_player;
		QSocketDevice *m_dev;
		QDataStream *m_net;
		int m_spectator;
		Network *m_network;
		int m_targetx, m_targety;
};

#endif

