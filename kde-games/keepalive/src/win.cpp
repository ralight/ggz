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
#include "win.h"

// Keepalive includes
#include "login.h"
#include "world.h"
#include "canvas.h"

// Qt includes
#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>

// Constructor
Win::Win(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;

	m_canvas = new Canvas(this);
	m_world = new World(m_canvas, this);
	m_world->hide();
	m_login = new Login(this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(m_world);
	vbox->add(m_login);

	connect(m_login, SIGNAL(signalLogin(QString, QString)),
		SLOT(slotLogin(QString, QString)));
	connect(m_canvas, SIGNAL(signalLoggedin(QString)),
		SLOT(slotLoggedin(QString)));

	setFixedSize(400, 300);
	setCaption("Keepalive: Login");
	show();
}

// Destructor
Win::~Win()
{
}

// Try to log into the server
void Win::slotLogin(QString username, QString password)
{
	m_canvas->login(username, password);
}

// Login and show game screen
void Win::slotLoggedin(QString name)
{
	setCaption(QString("Keepalive: Logged in as %1").arg(name));
	m_login->hide();
	m_world->show();
}

// Forward key presses to the canvas
void Win::keyPressEvent(QKeyEvent *e)
{
	int x, y;

	x = 0;
	y = 0;
	if(e->key() == Key_Left) x = -1;
	if(e->key() == Key_Right) x = 1;
	if(e->key() == Key_Up) y = -1;
	if(e->key() == Key_Down) y = 1;
	m_canvas->move(x, y);
}

