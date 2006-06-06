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
#include <kmessagebox.h>

// Qt includes
#include <qlayout.h>
#include <qdom.h>
#include <qpushbutton.h>
#include <qsocket.h>
#include <qstringlist.h>

// FIXME: see KGGZConnect.cpp
#define PRIMARY_METASERVER "live.ggzgamingzone.org"

// FIXME: define in central place, e.g. libggzmeta?
#define GGZ_PROTOCOL_VERSION "10"

KGGZMeta::KGGZMeta(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QPushButton *cancel;
	KGGZCaption *caption;

	caption = new KGGZCaption(i18n("Server selection"),
		i18n("Metaserver-based GGZ server selection."), this);

	m_ok = new QPushButton(i18n("OK"), this);
	m_ok->setEnabled(false);
	cancel = new QPushButton(i18n("Cancel"), this);

	m_view = new KListView(this);
	m_view->addColumn(i18n("Name"));
	m_view->addColumn(i18n("Version"));
	m_view->addColumn(i18n("Connection URI"));
	m_view->addColumn(i18n("Preference"));
	m_view->addColumn(i18n("Location"));
	m_view->addColumn(i18n("Speed"));

	vbox = new QVBoxLayout(this, 5);
	vbox->add(caption);
	vbox->add(m_view);

	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(m_ok);
	hbox->add(cancel);

	connect(m_ok, SIGNAL(clicked()), SLOT(slotAccept()));
	connect(cancel, SIGNAL(clicked()), SLOT(close()));
	connect(m_view, SIGNAL(pressed(QListViewItem*)), SLOT(slotSelection(QListViewItem*)));

	resize(800, 300);
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
		list = list.split(':', m_view->selectedItem()->text(2));
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
	connect(m_sock, SIGNAL(error(int)), SLOT(slotError(int)));
	m_sock->connectToHost(PRIMARY_METASERVER, 15689);
}

void KGGZMeta::slotError(int code)
{
	KMessageBox::error(this,
		i18n("The GGZ metaserver could not be contacted."),
		i18n("Connection"));
	delete m_sock;
	close();
}

void KGGZMeta::slotConnected()
{
	QString s;

	s = QString("<?xml version=\"1.0\"?>");
	s.append("<query class=\"ggz\" type=\"connection\">");
	s.append(GGZ_PROTOCOL_VERSION);
	s.append("</query>");
	s.append("\n");
	m_sock->writeBlock(s.utf8(), s.length());
	m_sock->flush();
}

void KGGZMeta::slotRead()
{
	QString rdata;
	QDomDocument dom;
	QDomNode resultnode, optionnode;
	QDomElement element;
	QListViewItem *item;
	QString pref, uri, name, location, speed, version, protocol;
	QString option;

	rdata = m_sock->readLine();
	rdata.truncate(rdata.length() - 1);

	dom.setContent(rdata);
	resultnode = dom.documentElement().firstChild();

	while(!resultnode.isNull())
	{
		element = resultnode.toElement();
		uri = element.attribute("uri");
		name = "";
		pref = "";
		location = "";
		speed = "";
		protocol = "";

		optionnode = resultnode.firstChild();
		while(!optionnode.isNull())
		{
			element = optionnode.toElement();
			option = element.attribute("name");

			if(option == "speed") speed = element.text();
			else if(option == "location") location = element.text();
			else if(option == "preference") pref = element.text();
			else if(option == "name") name = element.text();
			else if(option == "version") version = element.text();
			else if(option == "protocol") protocol = element.text();

			optionnode = optionnode.nextSibling();
		}

		// FIXME: discard if protocol not matching?

		item = new QListViewItem(m_view, name, version, uri, pref, location, speed);
		item->setPixmap(0, QPixmap(KGGZ_DIRECTORY "/images/icons/serverred.png"));

		resultnode = resultnode.nextSibling();
	}

	delete m_sock;
}

void KGGZMeta::slotSelection(QListViewItem *item)
{
	if((item) || (m_ok->isEnabled())) m_ok->setEnabled(true);
	else m_ok->setEnabled(false);
}

