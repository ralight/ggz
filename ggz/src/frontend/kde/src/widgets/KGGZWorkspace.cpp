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
// KGGZWorkspace: Hold together chat, table views, player views and the KGGZ logo. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZWorkspace.h"

// KGGZ includes
#include "KGGZCommon.h"

// Qt includes
#include <qlayout.h>

KGGZWorkspace::KGGZWorkspace(QWidget *parent, const char *name)
:  QWidget(parent, name)
{
	QHBoxLayout *hbox;

	m_firstresize = 4;

	m_vsbox = new QSplitter(this);
	m_vsbox->setOrientation(QSplitter::Vertical);

	m_widget = new QWidget(m_vsbox);
	m_widget->setMinimumHeight(74);

	m_logo = new KGGZLogo(m_widget);

	m_tables = new KGGZTables(m_widget, "workspace_tables");
	m_tables->setMinimumHeight(64);

	m_hsbox = new QSplitter(QSplitter::Horizontal, m_vsbox);

	m_userlist = new KGGZUsers(m_hsbox, "workspace_userlist");
	m_userlist->setMinimumWidth(1);

	m_chat = new KGGZChat(m_hsbox, "workspace_chat");

	hbox = new QHBoxLayout(m_widget, 5);
	hbox->add(m_logo);
	hbox->add(m_tables);
}

KGGZWorkspace::~KGGZWorkspace()
{
}

KGGZChat *KGGZWorkspace::widgetChat()
{
	return m_chat;
}

KGGZTables *KGGZWorkspace::widgetTables()
{
	return m_tables;
}

KGGZUsers *KGGZWorkspace::widgetUsers()
{
	return m_userlist;
}

void KGGZWorkspace::resizeEvent(QResizeEvent *e)
{
	QValueList<int> sizelist;

	m_vsbox->resize(e->size());

	if(m_firstresize)
	{
		m_firstresize = 0;
		sizelist.append(64);
		sizelist.append(height() - 79);
		m_vsbox->setSizes(sizelist);
		sizelist.clear();
		sizelist.append(150);
		sizelist.append(width() - 165);
		m_hsbox->setSizes(sizelist);
	}

	updateGeometry();
}

KGGZLogo *KGGZWorkspace::widgetLogo()
{
	return m_logo;
}

void KGGZWorkspace::hideBar(int hide)
{
	if(hide) m_widget->hide();
	else m_widget->show();
}

