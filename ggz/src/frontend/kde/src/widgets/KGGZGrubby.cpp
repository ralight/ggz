/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.4                //
//    Copyright (C) 2000, 2001 Josef Spillner - dr_maux@users.sourceforge.net      //
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
// KGGZGrubby: Show a visual grubby dialog which allows one to talk with this guy. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZGrubby.h"

// KGGZ includes
#include <KGGZCommon.h>

// KDE includes
#include <klocale.h>

// Qt includes
#include <qlayout.h>
#include <qframe.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlabel.h>

KGGZGrubby::KGGZGrubby(QWidget *parent = NULL, char *name = NULL)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QVBoxLayout *vbox, *vbox2;
	QHBoxLayout *hbox, *hbox2, *hbox3;
	QMultiLineEdit *mle;
	QComboBox *combo, *player;
	QLineEdit *ed;
	QPushButton *go, *quit;
	QFrame *image;
	QLabel *label;

	m_lastaction = 0;

	go = new QPushButton(i18n("Go!"), this);
	quit = new QPushButton(i18n("Quit"), this);
	ed = new QLineEdit(this);
	mle = new QMultiLineEdit(this);

	image = new QFrame(this);
	image->setFixedSize(128, 128);
	image->setBackgroundPixmap(QPixmap(KGGZ_DIRECTORY "/images/grubbybig.png"));

	combo = new QComboBox(this);
	combo->insertItem(i18n("Grubby, have you seen"), actionseen);
	combo->insertItem(i18n("Thanks for taking the time!"), actionbye);

	label = new QLabel(i18n("This is grubby:"), this);
	player = new QComboBox(this);

	vbox = new QVBoxLayout(this, 5);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(image);
	vbox2 = new QVBoxLayout(hbox, 5);
	hbox3 = new QHBoxLayout(vbox2, 5);
	hbox3->add(label);
	hbox3->add(player);
	vbox2->add(combo);
	hbox2 = new QHBoxLayout(vbox2, 5);
	hbox2->add(ed);
	hbox2->add(go);
	hbox2->add(quit);
	vbox->add(mle);

	connect(quit, SIGNAL(clicked()), SLOT(close()));
	connect(go, SIGNAL(clicked()), SLOT(slotInvoke()));
	connect(combo, SIGNAL(activated(int)), SLOT(slotActivated(int)));

	mle->setText(i18n("Please select who is grubby, and start a query!"));

	setFixedSize(330, 190);
	setCaption("GGZ Grubby");
	show();
}

KGGZGrubby::~KGGZGrubby()
{
}

void KGGZGrubby::slotInvoke()
{
	KGGZDEBUG("Perform action: %i\n", m_lastaction);
	emit signalAction(m_lastaction);
}

void KGGZGrubby::slotActivated(int index)
{
	m_lastaction = index;
}

