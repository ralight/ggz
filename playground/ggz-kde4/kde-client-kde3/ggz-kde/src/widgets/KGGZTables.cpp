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
// KGGZTables: Display all active tables, including the number of players on them. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZTables.h"

// KGGZ includes
#include "KGGZCommon.h"

// KDE includes
#include <klocale.h>

// Qt includes
#include <qstring.h>

KGGZTables::KGGZTables(QWidget *parent, const char *name)
: QIconView(parent, name)
{
}

KGGZTables::~KGGZTables()
{
}


void KGGZTables::reset()
{
	clear();
}

void KGGZTables::add(QString gametype, QString name, int used, int total)
{
	QIconViewItem *tmp;
	QString buffer;

	buffer = i18n("(%1) %2\n(%3 seats, %4 open)").arg(
		gametype).arg(name).arg(total).arg(total - used);
	tmp = new QIconViewItem(this, buffer,
		QPixmap(KGGZ_DIRECTORY "/images/icons/ggz.png"));
}

int KGGZTables::tablenum()
{
	QIconViewItem *tmp;
	int index;

	tmp = currentItem();
	if(!tmp) return -1;
	index = tmp->index();
	KGGZDEBUG("Selected table to join is: %i\n", index);
	return index;
}
