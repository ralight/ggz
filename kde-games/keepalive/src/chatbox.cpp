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
#include "chatbox.h"

// Qt includes
#include <qlineedit.h>
#include <qlayout.h>

// Constructor
Chatbox::Chatbox(QWidget *parent, const char *name)
: QWidget(parent, name, WType_TopLevel)
{
	QVBoxLayout *vbox;

	edit = new QLineEdit(this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(edit);

	edit->setFocus();

	connect(edit, SIGNAL(returnPressed()), SLOT(slotChat()));

	resize(400, 50);
	show();
}

// Destructor
Chatbox::~Chatbox()
{
}

void Chatbox::slotChat()
{
	emit signalChat(edit->text());
}

