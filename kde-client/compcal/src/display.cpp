//
//    Competition Calendar
//
//    Copyright (C) 2002, 2003 Josef Spillner <josef@ggzgamingzone.org>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "display.h"

#include "event.h"
#include "eventgroup.h"
#include "participant.h"

#include <klocale.h>
#include <klistview.h>
#include <kmessagebox.h>
#include <kurllabel.h>
#include <kprocess.h>
#include <kdebug.h>
#include <kio/netaccess.h>
#include <kstandarddirs.h>

#include <qlayout.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qptrlist.h>

Eventdisplay::Eventdisplay(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox, *vbox2, *vbox3;
	QHBoxLayout *hbox, *hbox2;

	m_list = new KListView(this);
	m_list->addColumn(i18n("Teams/Players"));
	m_list->addColumn(i18n("Score"));
	m_list->setRootIsDecorated(true);

	participants = new QLabel(i18n("Participants"), this);

	image = new QFrame(this);
	image->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	image->setFixedSize(128, 128);

	title = new QLabel("", this);
	location = new KURLLabel("", "", this);
	date = new QLabel("", this);
	description = new QTextEdit("", QString::null, this);
	description->setTextFormat(Qt::PlainText);
	description->setEnabled(false);

	logo = new QWidget(this);
	logo->setFixedSize(64, 64);

	vbox = new QVBoxLayout(this, 5);
	hbox2 = new QHBoxLayout(vbox, 5);
	hbox2->add(image);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->addStretch();
	vbox3 = new QVBoxLayout(hbox, 5);
	vbox3->add(logo);
	vbox2 = new QVBoxLayout(hbox, 5);
	vbox2->add(title);
	vbox2->add(date);
	vbox2->add(location);
	hbox->addStretch();
	vbox->add(description);
	vbox->add(participants);
	vbox->add(m_list);

	connect(location, SIGNAL(leftClickedURL(const QString&)), SLOT(slotUrl(const QString&)));

	setCaption(i18n("GGZ Event"));

	restoreDefaults();
}

Eventdisplay::~Eventdisplay()
{
}

void Eventdisplay::restoreDefaults()
{
	KStandardDirs d;

	image->setBackgroundPixmap(QPixmap(d.findResource("data", "compcal/event.png")));
	image->setFixedSize(128, 128);

	logo->setBackgroundPixmap(QPixmap());
	logo->setFixedSize(64, 64);
}

void Eventdisplay::setEvent(Event *event)
{
	QPtrList<Participant> plist;
	Participant *p;
	QListViewItem *tmp, *tmp2;
	QString src, dst;
	QPixmap pix;

	m_list->clear();

	plist = event->participants();
	for(p = plist.first(); p; p = plist.next())
	{
		tmp = new QListViewItem(m_list, p->team(), "1");
		tmp2 = new QListViewItem(tmp, p->name());
	}

	description->setText(event->description());
	location->setText(event->location());
	location->setURL(event->location());
	title->setText(QString("<b>%1</b>").arg(event->title()));
	date->setText(event->date());

	restoreDefaults();

	src = event->parent()->image();
	if(KIO::NetAccess::download(src, dst))
	{
		pix = QPixmap(dst);
		image->setBackgroundPixmap(pix);
		image->setFixedSize(pix.width(), pix.height());
	}

	src = event->image();
	if(src.isEmpty())
	{
		KStandardDirs d;
		QString pixfile;

		pixfile = d.findResource("data", QString("kggz/images/icons/games/%1.png").arg(event->parent()->game()));
		pix = QPixmap(pixfile);
		logo->setBackgroundPixmap(pix);
		logo->setFixedSize(pix.width(), pix.height());
	}
	else
	{
		if(KIO::NetAccess::download(src, dst))
		{
			pix = QPixmap(dst);
			logo->setBackgroundPixmap(pix);
			logo->setFixedSize(pix.width(), pix.height());
		}
	}
}

void Eventdisplay::slotUrl(const QString& url)
{
	KProcess *proc = new KProcess();
	*proc << "kggz" << url.latin1();
	connect(proc, SIGNAL(processExited(KProcess*)), SLOT(slotProcess(KProcess*)));
	proc->start();
	kdDebug() << "start" << endl;
}


void Eventdisplay::slotProcess(KProcess *proc)
{
	Q_UNUSED(proc);

	kdDebug() << "fini" << endl;
}

