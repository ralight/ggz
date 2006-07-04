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
#include "KGGZInput.h"

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

	input_team = NULL;
	input_member = NULL;

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
	connect(b_found, SIGNAL(clicked()), SLOT(slotFound()));
	connect(b_add, SIGNAL(clicked()), SLOT(slotAdd()));

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
}

void KGGZTeam::slotAdd()
{
	if(!input_member)
	{
		input_member = new KGGZInput(NULL, NULL,
			i18n("New team member"), i18n("Name of the player who joins the team."));
		connect(input_member, SIGNAL(signalText(QString)), SLOT(slotAdded(QString)));
	}
	input_member->show();
}

void KGGZTeam::slotFound()
{
	if(!input_team)
	{
		input_team = new KGGZInput(NULL, NULL,
			i18n("Team foundation"), i18n("What should the name of the new team be?"));
		connect(input_team, SIGNAL(signalText(QString)), SLOT(slotFounded(QString)));
	}
	input_team->show();
}

void KGGZTeam::slotAdded(QString name)
{
	QListViewItem *tmp, *tmp2;

	if(name.isEmpty()) return;
	
	tmp = list->currentItem();
	if(!tmp) return;
	if(tmp->parent()) return;

	if(!tmp->childCount())
	{
		tmp2 = new QListViewItem(tmp, name, "founder", "100");
		tmp2->setPixmap(1, QPixmap(KGGZ_DIRECTORY "/images/icons/players/team-founder.png"));
	}
	else
	{
		tmp2 = new QListViewItem(tmp, name, "member", "80");
		tmp2->setPixmap(1, QPixmap(KGGZ_DIRECTORY "/images/icons/players/team-member.png"));
	}
}

void KGGZTeam::slotFounded(QString name)
{
	if(name.isEmpty()) return;

	(void)new QListViewItem(list, name, "", "0");
}

