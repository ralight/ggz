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
#include "login.h"
#include "config.h"

// KDE includes
#include <klocale.h>

// Qt includes
#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qbitmap.h>

// Useful definitions
#define KEEPALIVE_DIR GGZDATADIR "/keepalive"

// Constructor: show mask to query username/password
Login::Login(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox, *vboxroot;
	QHBoxLayout *hbox;
	QLabel *lusername, *lpassword;
	QPushButton *ok;
	QFrame *frame;
	QLabel *explanation;
	QWidget *logo;
	QPixmap logopix;
	QBitmap logomap;

	frame = new QFrame(this);
	frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	frame->setFixedHeight(200);

	explanation = new QLabel(i18n("Enter the game Keepalive!\n"
		"If you are new to this game, please select a login name and a password for it.\n"
		"Otherwise log in with your data to restore your avatar.\n"),
		frame);

	logo = new QWidget(this);
	logopix = QPixmap(KEEPALIVE_DIR "/keepalive.png");
	logo->setErasePixmap(logopix);
	logomap = logopix.createHeuristicMask();
	logo->setMask(logomap);
	logo->setFixedSize(logopix.width(), logopix.height());

	lusername = new QLabel(i18n("Username"), frame);
	lpassword = new QLabel(i18n("Password"), frame);
	m_username = new QLineEdit(frame);
	m_password = new QLineEdit(frame);
	ok = new QPushButton(i18n("Login..."), frame);
	ok->setFixedWidth(100);

	vbox = new QVBoxLayout(frame, 5);
	vbox->add(explanation);
	vbox->addSpacing(0);
	vbox->add(lusername);
	vbox->add(m_username);
	vbox->add(lpassword);
	vbox->add(m_password);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->addStretch();
	hbox->add(ok);

	vboxroot = new QVBoxLayout(this, 5);
	vboxroot->add(logo);
	vboxroot->addStretch();
	vboxroot->add(frame);

	setErasePixmap(QPixmap(KEEPALIVE_DIR "/bg.png"));
	frame->setErasePixmap(QPixmap(KEEPALIVE_DIR "/bg.png"));
	explanation->setErasePixmap(QPixmap(KEEPALIVE_DIR "/bg.png"));
	lusername->setErasePixmap(QPixmap(KEEPALIVE_DIR "/bg.png"));
	lpassword->setErasePixmap(QPixmap(KEEPALIVE_DIR "/bg.png"));

	setFocusPolicy(StrongFocus);
	lusername->setFocus();

	connect(ok, SIGNAL(clicked()), SLOT(slotLogin()));

	show();
}

// Destructor
Login::~Login()
{
}

// Send login attempt to the server
void Login::slotLogin()
{
	emit signalLogin(m_username->text(), m_password->text(), QString::null);
}

