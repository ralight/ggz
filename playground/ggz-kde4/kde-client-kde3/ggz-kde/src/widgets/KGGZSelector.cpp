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

// KGGZ includes
#include "KGGZCommon.h"

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
void KGGZSelector::addFrontend(QString name, QString frontend, int position)
{
	QString pixname = "game.png";
	if((frontend == "gtk") || (frontend == "gnome")) pixname = "game_gnome.png";
	if((frontend == "qt") || (frontend == "kde")) pixname = "game_kde.png";
	if(frontend == "sdl") pixname = "game_sdl.png";
	if(frontend == "x11") pixname = "game_x11.png";
	if(frontend == "console") pixname = "game_console.png";
	if(frontend == "guru") pixname = "game_grubby.png";

	QPixmap pix(QString(KGGZ_DIRECTORY) + "/kcmggz/icons/" + pixname);

	m_box->insertItem(pix, QString("%1 (%2)").arg(name).arg(frontend));
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

