/*
 * The GGZ Gaming Zone Metaserver Project
 * Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
 * Published under GNU GPL conditions.
 */

#include "qmetagui.h"
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qdom.h>

QMetaGUI::QMetaGUI()
: QWidget()
{
	QPushButton *uri, *xml;
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QLabel *lhost, *lport;
	
	host = new QLineEdit("localhost", this);
	port = new QLineEdit("15689", this);
	uri = new QPushButton("Simple URI Query", this);
	xml = new QPushButton("Advanced XML Query", this);
	view = new QListView(this);
	view->addColumn("URI");
	view->addColumn("Preference");
	lhost = new QLabel("Host", this);
	lport = new QLabel("Port", this);
	raw = new QMultiLineEdit(this);

	vbox = new QVBoxLayout(this, 5);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(lhost);
	hbox->add(host);
	hbox->add(lport);
	hbox->add(port);
	hbox->add(uri);
	hbox->add(xml);
	vbox->add(raw);
	vbox->add(view);

	setCaption("GGZ Gaming Zone Meta Server GUI");
	setFixedSize(500, 300);

	connect(uri, SIGNAL(clicked()), SLOT(slotURI()));
	connect(xml, SIGNAL(clicked()), SLOT(slotXML()));
}

QMetaGUI::~QMetaGUI()
{
}

void QMetaGUI::slotURI()
{
	m_type = 0;
	doconnection();
}

void QMetaGUI::doconnection()
{
	sock = new QSocket();
	connect(sock, SIGNAL(connected()), SLOT(slotConnected()));
	connect(sock, SIGNAL(readyRead()), SLOT(slotRead()));
	sock->connectToHost(host->text(), port->text().toInt());
}

void QMetaGUI::slotXML()
{
	m_type = 1;
	doconnection();
}

void QMetaGUI::slotConnected()
{
	QString data;

	if(m_type == 0) data = "query://ggz/connection/0.0.4\n";
	else data = "<?xml version=\"1.0\"><query class=\"ggz\" type=\"connection\">0.0.4</query>\n";

	sock->writeBlock(data.latin1(), data.length());
	sock->flush();
}

void QMetaGUI::slotRead()
{
	QString data;
	QDomDocument dom;
	QDomNode node;
	QDomElement element;
	QString pref;

	data = sock->readLine();
	raw->setText(data);

	data.truncate(data.length() - 1);
	view->clear();

	if(!data.contains("<?xml "))
		(void)new QListViewItem(view, data, "(irrelevant)");
	else
	{
		dom.setContent(data);
		node = dom.documentElement().firstChild();
		while(!node.isNull())
		{
			element = node.toElement();
			if(!element.firstChild().isNull())
			{
				pref = element.attribute("preference", "20");
				element = element.firstChild().toElement();
				(void)new QListViewItem(view, element.text(), pref);
			}
			node = node.nextSibling();
		}
	}

	delete sock;
}

