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
// KGGZMeta: Present a list of GGZ servers which are fetched from metaserver list. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZMeta.h"

// KGGZ includes
#include "KGGZCommon.h"
#include "KGGZCaption.h"

// KDE includes
#include <klocale.h>
#include <klistview.h>

// Qt includes
#include <qlayout.h>
#include <qdom.h>
#include <qpushbutton.h>
#include <qsocket.h>
#include <qstringlist.h>

KGGZMeta::KGGZMeta(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QPushButton *cancel;
	KGGZCaption *caption;

	caption = new KGGZCaption(i18n("Server selection"), i18n("Metaserver-based GGZ server selection."), this);

	m_ok = new QPushButton("OK", this);
	m_ok->setEnabled(false);
	cancel = new QPushButton(i18n("Cancel"), this);

	m_view = new KListView(this);
	m_view->addColumn("URI");
	m_view->addColumn("Preference");

	vbox = new QVBoxLayout(this, 5);
	vbox->add(caption);
	vbox->add(m_view);

	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(m_ok);
	hbox->add(cancel);

	connect(m_ok, SIGNAL(clicked()), SLOT(slotAccept()));
	connect(cancel, SIGNAL(clicked()), SLOT(close()));
	connect(m_view, SIGNAL(pressed(QListViewItem*)), SLOT(slotSelection(QListViewItem*)));

	setFixedSize(300, 300);
	setCaption(i18n("Server selection"));
	show();

	load();
}

KGGZMeta::~KGGZMeta()
{
}

void KGGZMeta::slotAccept()
{
	QString host, port;
	QStringList list;

	if(m_view->selectedItem())
	{
		list = list.split(':', m_view->selectedItem()->text(0));
		if(list.count() == 3)
		{
			host = *(list.at(1));
			port = *(list.at(2));
			host = host.right(host.length() - 2);
			emit signalData(host, port);
		}
	}
}

void KGGZMeta::load()
{
	m_sock = new QSocket();
	connect(m_sock, SIGNAL(connected()), SLOT(slotConnected()));
	connect(m_sock, SIGNAL(readyRead()), SLOT(slotRead()));
	m_sock->connectToHost("live.ggzgamingzone.org", 15689);
}

void KGGZMeta::slotConnected()
{
	QString s;

	s = QString("<?xml version=\"1.0\"?><query class=\"ggz\" type=\"connection\">%1</query>\n").arg(KGGZVERSION);
	m_sock->writeBlock(s.latin1(), s.length());
	m_sock->flush();
}

void KGGZMeta::slotRead()
{
	QString rdata;
	QDomDocument dom;
	QDomNode node;
	QDomElement element;
	QString pref;
	QListViewItem *item;

	rdata = m_sock->readLine();
	rdata.truncate(rdata.length() - 1);

	dom.setContent(rdata);
	node = dom.documentElement().firstChild();

	while(!node.isNull())
	{
		element = node.toElement();
		if(!element.firstChild().isNull())
		{
			pref = element.attribute("preference", "20");
			element = element.firstChild().toElement();
			item = new QListViewItem(m_view, element.text(), pref);
			item->setPixmap(0, QPixmap(KGGZ_DIRECTORY "/images/icons/serverred.png"));
		}
		node = node.nextSibling();
	}

	delete m_sock;
}

void KGGZMeta::slotSelection(QListViewItem *item)
{
	if((item) || (m_ok->isEnabled())) m_ok->setEnabled(true);
	else m_ok->setEnabled(false);
}

