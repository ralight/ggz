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
// KGGZInput: Pop up a little dialog which asks the user for a string or a number. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZInput.h"
#include "KGGZCaption.h"

// KDE includes
#include <klocale.h>

// Qt includes
#include <qlayout.h>

KGGZInput::KGGZInput(QWidget *parent, const char *name, QString title, QString description)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QPushButton *cancel;
	KGGZCaption *caption;

	caption = new KGGZCaption(title, description, this);

	m_ok = new QPushButton("OK", this);
	m_ok->setEnabled(false);
	cancel = new QPushButton(i18n("Cancel"), this);

	m_edit = new QLineEdit(this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(caption);
	vbox->add(m_edit);

	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(m_ok);
	hbox->add(cancel);

	connect(m_edit, SIGNAL(textChanged(const QString&)), SLOT(slotChanged()));
	connect(m_ok, SIGNAL(clicked()), SLOT(slotAccept()));
	connect(cancel, SIGNAL(clicked()), SLOT(slotAccept()));

	setFixedSize(200, 100);
	setCaption(i18n("New Profile"));
	show();
}

KGGZInput::~KGGZInput()
{
}

void KGGZInput::slotAccept()
{
	emit signalText(m_edit->text());
	close();
}

void KGGZInput::slotChanged()
{
	if(m_edit->text().length() == 0) m_ok->setEnabled(FALSE);
	else m_ok->setEnabled(TRUE);
}

