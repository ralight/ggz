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
// KGGZTeam: A dialog to manage the relationships between players and their teams. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZTeam.h"

// KGGZ includes
#include "KGGZCaption.h"
#include "KGGZLineSeparator.h"
#include "KGGZCommon.h"

// KDE includes
#include <klocale.h>
#include <klistview.h>

// Qt includes
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qpixmap.h>

// Constructor
KGGZTeam::KGGZTeam(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	KGGZCaption *title;
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QPushButton *b_ok, *b_add, *b_found;
	KGGZLineSeparator *sep;

	title = new KGGZCaption(i18n("Teams"), i18n("Manage your team memberships."), this);

	list = new KListView(this);
	list->addColumn(i18n("Teams/Players"));
	list->addColumn(i18n("Status"));
	list->addColumn(i18n("Score"));
	list->setRootIsDecorated(true);

	sep = new KGGZLineSeparator(this);

	b_ok = new QPushButton(i18n("Close"), this);
	b_add = new QPushButton(i18n("Add player"), this);
	b_found = new QPushButton(i18n("Found new team"), this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(title);
	vbox->add(list);
	vbox->add(sep);

	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(b_found);
	hbox->add(b_add);
	hbox->add(b_ok);

	connect(b_ok, SIGNAL(clicked()), SLOT(slotAccept()));

	setCaption(i18n("Teams"));
	resize(300, 300);
	show();

	load();
}

// Destructor
KGGZTeam::~KGGZTeam()
{
}

void KGGZTeam::slotAccept()
{
	close();
}

void KGGZTeam::load()
{
	QListViewItem *tmp, *tmp2;

	tmp = new QListViewItem(list, "Bielefeld Connection", "", "102");
	tmp2 = new QListViewItem(tmp, "Some guy", "founder", "57");
	tmp2->setPixmap(1, QPixmap(KGGZ_DIRECTORY "/images/icons/players/team-founder.png"));
	tmp2 = new QListViewItem(tmp, "Some other guy", "member", "45");
	tmp2->setPixmap(1, QPixmap(KGGZ_DIRECTORY "/images/icons/players/team-member.png"));

	tmp = new QListViewItem(list, "Joe's Chicken Club", "", "0");
	tmp2 = new QListViewItem(tmp, "Anonymous", "member", "0");
	tmp2->setPixmap(1, QPixmap(KGGZ_DIRECTORY "/images/icons/players/team-member.png"));
}

