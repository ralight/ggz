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

#ifndef KEEPALIVE_WIN_H
#define KEEPALIVE_WIN_H

// Qt includes
#include <qwidget.h>

// Class declarations
class Login;
class World;
class Canvas;
class Chatbox;
class QCanvasPixmapArray;
class AnimDialog;
class Intro;

// The top level window
class Win : public QWidget
{
	Q_OBJECT
	public:
		Win(QWidget *parent = NULL, const char *name = NULL);
		~Win();
		void init();

	public slots:
		void slotLogin(QString username, QString password, QString hostname);
		void slotLoggedin(QString name);
		void slotChat(QString message);
		void slotUnit(QCanvasPixmapArray *a);
		void slotMouse(int x, int y);

	protected:
		void keyPressEvent(QKeyEvent *e);

	private:
		Login *m_login;
		World *m_world;
		Canvas *m_canvas;
		Chatbox *chatbox;
		AnimDialog *m_anim;
		Intro *m_intro;
};

#endif

