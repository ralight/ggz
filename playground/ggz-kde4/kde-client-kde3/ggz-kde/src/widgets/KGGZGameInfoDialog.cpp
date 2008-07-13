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
// KGGZGameInfoDialog: Show detailed game client and server information and specs. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZGameInfoDialog.h"

// KGGZ includes
#include "KGGZCaption.h"
#include "KGGZLineSeparator.h"

// KDE includes
#include <klocale.h>
#include <ktextbrowser.h>

// Qt includes
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>

// Constructor
KGGZGameInfoDialog::KGGZGameInfoDialog(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	KGGZCaption *title;
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QPushButton *ok;
	KGGZLineSeparator *sep;

	title = new KGGZCaption(i18n("Game information"), i18n("Properties of the game played in this room."), this);

	m_information = new KTextBrowser(this);
	m_information->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	sep = new KGGZLineSeparator(this);

	ok = new QPushButton(i18n("OK"), this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(title);
	vbox->add(m_information);
	vbox->add(sep);

	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(ok);

	connect(ok, SIGNAL(clicked()), SLOT(slotAccept()));

	setCaption(i18n("Game information"));
	resize(300, 300);
	show();
}

// Destructor
KGGZGameInfoDialog::~KGGZGameInfoDialog()
{
}

void KGGZGameInfoDialog::slotAccept()
{
	close();
}

void KGGZGameInfoDialog::setInformation(const QString& information)
{
	m_information->setText(information);
}

