//
//    Competition Calendar
//
//    Copyright (C) 2002 Josef Spillner <dr_maux@users.sourceforge.net>
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

#include "app.h"

#include "display.h"
#include "parser.h"
#include "series.h"
#include "eventgroup.h"
#include "event.h"

#include <klocale.h>
#include <klistview.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kstandarddirs.h>
#include <kio/netaccess.h>

#include <kdebug.h>

#include <qlayout.h>
#include <qcanvas.h>
#include <qpixmap.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qtextstream.h>

App::App(QWidget *parent, const char *name)
: QSplitter(Qt::Horizontal, parent, name)
{
	QHBoxLayout *hbox;

	m_view = sorted;
	p = NULL;

	m_list = new KListView(this);
	m_list->addColumn(i18n("Date"));
	m_list->addColumn(i18n("Event"));
	m_list->setRootIsDecorated(true);

	m_display = new Eventdisplay(this);

	hbox = new QHBoxLayout(this, 5);
	hbox->add(m_list);
	hbox->add(m_display);

	connect(m_list, SIGNAL(clicked(QListViewItem*)), SLOT(slotEvent(QListViewItem*)));

	//load();
	fetch();
}

App::~App()
{
}

void App::fetch()
{
	QString src, dst;

	if(p)
	{
		delete p;
		p = NULL;
	}

	src = "http://mindx.dyndns.org/ggz/calendar/calendar.xml";

	if(KIO::NetAccess::download(src, dst))
	{
		p = new Parser(dst.latin1());
		load();
	}
	else KMessageBox::error(this, i18n("Could not fetch event list."), i18n("Fetch error"));
}

void App::view(int mode)
{
	m_view = mode;
	load();
}

void App::load()
{
	QListViewItem *tmp = NULL, *tmp2 = NULL, *tmp3 = NULL;
	KStandardDirs d;
	Series *s;
	QString icontheme;

#if ((KDE_VERSION_MAJOR == 3) && (KDE_VERSION_MINOR >= 1) || (KDE_VERSION_MAJOR > 3))
	icontheme = "crystalsvg";
#else
	icontheme = "hicolor";
#endif

	if(!p) return;

	m_list->clear();

	s = p->series();
	if(s)
	{
		if(m_view == sorted)
		{
			tmp = new QListViewItem(m_list, s->title());
			tmp->setPixmap(0, d.findResource("icon", icontheme + "/16x16/filesystems/folder_yellow.png"));
		}
		QPtrList<Eventgroup> eventgrouplist = s->eventgroups();
		for(Eventgroup *eventgroup = eventgrouplist.first(); eventgroup; eventgroup = eventgrouplist.next())
		{
			if(m_view == sorted)
			{
				tmp2 = new QListViewItem(tmp, eventgroup->title());
				tmp2->setPixmap(0, d.findResource("icon", icontheme + "/16x16/filesystems/folder_green.png"));
			}
			QPtrList<Event> eventlist = eventgroup->events();
			for(Event *event = eventlist.first(); event; event = eventlist.next())
			{
				if(m_view == sorted)
				{
					tmp3 = new QListViewItem(tmp2, QString("%1").arg(event->date()), event->title());
				}
				else
				{
					tmp3 = new QListViewItem(m_list, QString("%1").arg(event->date()), event->title());
				}
				tmp3->setPixmap(0, d.findResource("icon", icontheme + "/16x16/filesystems/exec.png"));
			}
		}
	}
}

void App::slotEvent(QListViewItem *item)
{
	Series *s;

	if(!item) return;
	if(item->text(1) == QString::null) return;
	if(!p) return;

	kdDebug() << "Event: " << item->text(1) << endl;

	s = p->series();
	if(s)
	{
		QPtrList<Eventgroup> eventgrouplist = s->eventgroups();
		for(Eventgroup *eventgroup = eventgrouplist.first(); eventgroup; eventgroup = eventgrouplist.next())
		{
			QPtrList<Event> eventlist = eventgroup->events();
			for(Event *event = eventlist.first(); event; event = eventlist.next())
			{
				if((event->title() == item->text(1)) && (event->date() == item->text(0)))
				{
					kdDebug() << "got it!" << endl;
					m_display->setEvent(event);
				}
			}
		}
	}
}

