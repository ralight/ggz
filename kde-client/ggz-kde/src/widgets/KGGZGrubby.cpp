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
// KGGZGrubby: Show a visual grubby dialog which allows one to talk with this guy. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZGrubby.h"

// KGGZ includes
#include "KGGZCommon.h"
#include "KGGZCaption.h"
#include "KGGZLineSeparator.h"

// KDE includes
#include <klocale.h>

// Qt includes
#include <qlayout.h>
#include <qframe.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlineedit.h>

KGGZGrubby::KGGZGrubby(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QVBoxLayout *vbox, *vbox2;
	QHBoxLayout *hbox, *hbox2, *hbox3;
	QPushButton *quit;
	QFrame *image;
	QLabel *label, *label2, *label3;
	KGGZCaption *caption;
	KGGZLineSeparator *sep;

	m_lastaction = 0;

	caption = new KGGZCaption(i18n("Grubby dialog"),
		i18n("If you want to talk with grubbies (the GGZ\n"
		"chat bots), you can do so using this dialog."), this);

	sep = new KGGZLineSeparator(this);

	m_go = new QPushButton(i18n("Go!"), this);
	quit = new QPushButton(i18n("Quit"), this);
	m_ed = new QLineEdit(this);
	m_mle = new QMultiLineEdit(this);

	image = new QFrame(this);
	image->setFixedSize(128, 128);
	image->setErasePixmap(QPixmap(KGGZ_DIRECTORY "/images/grubbybig.png"));

	m_combo = new QComboBox(this);
	m_combo->insertItem(i18n("Have you seen ...:"));
	m_combo->insertItem(i18n("Do you have any messages for me?"));
	m_combo->insertItem(i18n("Alert this player if you see him:"));
	m_combo->insertItem(i18n("What do you know about ...:"));
	m_combo->insertItem(i18n("Who is ...:"));
	m_combo->insertItem(i18n("I need help!"));
	m_combo->insertItem(i18n("Tell me something about you"));
	m_combo->insertItem(i18n("Thanks for taking the time!"));
	
	label = new QLabel(i18n("This is grubby:"), this);
	label2 = new QLabel(i18n("Select a command:"), this);
	label3 = new QLabel(i18n("Specify a command option:"), this);
	m_player = new QComboBox(this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(caption);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(image);
	vbox2 = new QVBoxLayout(hbox, 5);
	hbox3 = new QHBoxLayout(vbox2, 5);
	hbox3->add(label);
	hbox3->add(m_player);
	vbox2->add(label2);
	vbox2->add(m_combo);
	vbox2->add(label3);
	vbox2->add(m_ed);
	hbox2 = new QHBoxLayout(vbox2, 5);
	hbox2->add(m_go);
	hbox2->add(quit);
	vbox->add(sep);
	vbox->add(m_mle);

	connect(quit, SIGNAL(clicked()), SLOT(close()));
	connect(m_go, SIGNAL(clicked()), SLOT(slotInvoke()));
	connect(m_combo, SIGNAL(activated(int)), SLOT(slotActivated(int)));

	m_mle->setText(i18n("Please select grubby and start a query!"));

	setFixedSize(330, 290);
	setCaption("GGZ Grubby");
	show();
}

KGGZGrubby::~KGGZGrubby()
{
}

void KGGZGrubby::slotInvoke()
{
	KGGZDEBUG("current item: %i\n", m_player->currentItem());
	KGGZDEBUG("current text: %s\n", m_player->currentText().utf8().data());

	KGGZDEBUG("Perform action: %i on %i\n", m_lastaction, m_player->currentItem());
	emit signalAction(m_player->currentText(), m_ed->text(), m_lastaction);
	m_mle->clear();
}

void KGGZGrubby::slotActivated(int index)
{
	m_lastaction = index;

	switch(index)
	{
		case actionbye:
		case actionmessages:
		case actionhelp:
		case actionabout:
			m_ed->setEnabled(FALSE);
			break;
		case actionalertadd:
		case actionseen:
		case actionteach:
		case actionwhois:
			m_ed->setEnabled(TRUE);
			break;
	}
}

void KGGZGrubby::removeAll()
{
	m_player->clear();
	m_player->setEnabled(false);
	m_ed->setEnabled(false);
	m_go->setEnabled(false);
	m_combo->setEnabled(false);
	m_mle->setEnabled(false);
}

void KGGZGrubby::addPlayer(QString player)
{
	m_player->insertItem(player);
	if(m_player->count() > 1) m_player->setEnabled(true);
	m_ed->setEnabled(true);
	m_go->setEnabled(true);
	m_combo->setEnabled(true);
	m_mle->setEnabled(true);
}

void KGGZGrubby::answer(QString message)
{
	m_mle->append(message + "\n");
}

