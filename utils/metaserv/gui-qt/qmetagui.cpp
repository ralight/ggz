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
	QPushButton *uri, *xml, *submit, *update;
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QLabel *lhost, *lport, *lrawresult, *lresult, *ldata;
	
	host = new QLineEdit("localhost", this);
	port = new QLineEdit("15689", this);
	uri = new QPushButton("Simple URI", this);
	xml = new QPushButton("Advanced XML", this);
	update = new QPushButton("Update", this);
	submit = new QPushButton("Submit Query", this);
	view = new QListView(this);
	view->addColumn("URI");
	view->addColumn("Preference");
	lhost = new QLabel("Host", this);
	lport = new QLabel("Port", this);
	lresult = new QLabel("Result", this);
	lrawresult = new QLabel("Raw result", this);
	ldata = new QLabel("Data to submit:", this);
	raw = new QMultiLineEdit(this);
	data = new QMultiLineEdit(this);

	vbox = new QVBoxLayout(this, 5);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(lhost);
	hbox->add(host);
	hbox->add(lport);
	hbox->add(port);
	hbox->add(uri);
	hbox->add(xml);
	hbox->add(update);
	hbox->add(submit);
	vbox->add(ldata);
	vbox->add(data);
	vbox->add(lrawresult);
	vbox->add(raw);
	vbox->add(lresult);
	vbox->add(view);

	setCaption("GGZ Gaming Zone Meta Server GUI");
	resize(600, 300);

	connect(uri, SIGNAL(clicked()), SLOT(slotURI()));
	connect(xml, SIGNAL(clicked()), SLOT(slotXML()));
	connect(submit, SIGNAL(clicked()), SLOT(slotSubmit()));
	connect(update, SIGNAL(clicked()), SLOT(slotUpdate()));
}

QMetaGUI::~QMetaGUI()
{
}

void QMetaGUI::slotURI()
{
	//m_type = 0;
	data->setText("query://ggz/connection/0.0.6\n");
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
	//m_type = 1;
	data->setText("<?xml version=\"1.0\"><query class=\"ggz\" type=\"connection\">0.0.6</query>\n");
}

void QMetaGUI::slotUpdate()
{
	data->setText("<?xml version=\"1.0\"><update class=\"ggz\" type=\"connection\" username=\"???\" password=\"???\"><option name=\"mode\">add</option><option name=\"uri\">ggz://somewhere</option><option name=\"version\">0.0.7pre</option><option name=\"preference\">30</option></update>\n");
}

void QMetaGUI::slotSubmit()
{
	doconnection();
}

void QMetaGUI::slotConnected()
{
	//QString data;

	//if(m_type == 0) data = "query://ggz/connection/0.0.4\n";
	//else data = "<?xml version=\"1.0\"><query class=\"ggz\" type=\"connection\">0.0.4</query>\n";

	sock->writeBlock(data->text().latin1(), data->text().length());
	sock->flush();
}

void QMetaGUI::slotRead()
{
	QString rdata;
	QDomDocument dom;
	QDomNode node;
	QDomElement element;
	QString pref;

	rdata = sock->readLine();
	raw->setText(rdata);

	rdata.truncate(rdata.length() - 1);
	view->clear();

	if(!rdata.contains("<?xml "))
		(void)new QListViewItem(view, rdata, "(irrelevant)");
	else
	{
		dom.setContent(rdata);
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

