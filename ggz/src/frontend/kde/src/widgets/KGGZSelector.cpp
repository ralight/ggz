/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.5pre             //
//    Copyright (C) 2000 - 2002 Josef Spillner - dr_maux@users.sourceforge.net     //
//    The MindX Open Source Project - http://mindx.sourceforge.net                 //
//    Published under GNU GPL conditions - view COPYING for details                //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    This program is free software; you can redistribute it and/or modify         //
//    it under the terms of the GNU General Public License as published by         //
//    the Free Software Foundation; either version 2 of the License, or            //
//    (at your option) any later version.                                          //
//                                                                                 //
//    This program is distributed in the hope that it will be useful,              //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of               //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                //
//    GNU General Public License for more details.                                 //
//                                                                                 //
//    You should have received a copy of the GNU General Public License            //
//    along with this program; if not, write to the Free Software                  //
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// KGGZSelector: Let's the player select from the available game client frontends. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZSelector.h"

// KDE includes
#include <klocale.h>

// Qt includes
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>

// Constructor
KGGZSelector::KGGZSelector(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QLayout *vbox;
	QLabel *label;
	QPushButton *ok;

	label = new QLabel(i18n("This game has multiple frontends.\nSelect your favorite one."), this);

	m_box = new QComboBox(this);

	ok = new QPushButton("OK", this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(label);
	vbox->add(m_box);
	vbox->add(ok);

	connect(ok, SIGNAL(clicked()), SLOT(slotAccept()));

	setCaption(i18n("Frontend Selection"));
	setFixedSize(200, 100);
	show();
}

// Destructor
KGGZSelector::~KGGZSelector()
{
}

// Adds a frontend
void KGGZSelector::addFrontend(const char *frontend, int position)
{
	m_box->insertItem(frontend);
	m_positions[m_box->count() - 1] = position;
}

// User pressed ok
void KGGZSelector::slotAccept()
{
	int position;

	position = m_positions[m_box->currentItem()];
	emit signalFrontend(position);
	close();
}

